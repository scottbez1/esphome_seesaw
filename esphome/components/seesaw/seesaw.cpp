#include "seesaw.h"
#include "binary_sensor.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace seesaw {

static const char *const TAG = "seesaw";

void SeesawDevice::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Seesaw device...");

  // Optional software reset
  if (software_reset_) {
    ESP_LOGD(TAG, "Performing software reset...");
    write_register(SEESAW_STATUS_BASE, SEESAW_STATUS_SWRST, nullptr, 0);
    delay(10);  // Wait for reset to complete
  }

  // Verify hardware ID
  if (!verify_hardware_id_()) {
    ESP_LOGE(TAG, "Failed to verify Seesaw hardware ID");
    this->mark_failed();
    return;
  }

  // Configure GPIO pins for registered binary sensors
  configure_gpio_pins_();

  ESP_LOGCONFIG(TAG, "Seesaw device initialized (HW ID: 0x%02X)", hardware_id_);
}

void SeesawDevice::update() {
  if (binary_sensors_.empty()) {
    return;
  }

  uint32_t gpio_state;
  if (read_gpio_bulk(&gpio_state)) {
    notify_binary_sensors_(gpio_state);
  } else {
    ESP_LOGW(TAG, "Failed to read GPIO state");
  }
}

void SeesawDevice::dump_config() {
  ESP_LOGCONFIG(TAG, "Seesaw:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication failed");
  }
  ESP_LOGCONFIG(TAG, "  Hardware ID: 0x%02X", hardware_id_);
  LOG_UPDATE_INTERVAL(this);
}

float SeesawDevice::get_setup_priority() const { return setup_priority::DATA; }

bool SeesawDevice::verify_hardware_id_() {
  uint8_t hw_id;
  if (!read_register(SEESAW_STATUS_BASE, SEESAW_STATUS_HW_ID, &hw_id, 1)) {
    ESP_LOGE(TAG, "Failed to read hardware ID");
    return false;
  }

  hardware_id_ = hw_id;

  // Check for known hardware IDs
  switch (hw_id) {
    case SEESAW_HW_ID_CODE_SAMD09:
    case SEESAW_HW_ID_CODE_TINY806:
    case SEESAW_HW_ID_CODE_TINY807:
    case SEESAW_HW_ID_CODE_TINY816:
    case SEESAW_HW_ID_CODE_TINY817:
    case SEESAW_HW_ID_CODE_TINY1616:
    case SEESAW_HW_ID_CODE_TINY1617:
      ESP_LOGD(TAG, "Found valid Seesaw HW ID: 0x%02X", hw_id);
      return true;
    default:
      ESP_LOGE(TAG, "Unknown Seesaw HW ID: 0x%02X", hw_id);
      return false;
  }
}

void SeesawDevice::configure_gpio_pins_() {
  if (binary_sensors_.empty()) {
    return;
  }

  // Build pin mask from all registered binary sensors
  uint32_t pin_mask = 0;
  for (auto *sensor : binary_sensors_) {
    // Get pin from sensor - we'll need to add a getter
    // For now we'll configure in the binary sensor setup
  }
}

void SeesawDevice::notify_binary_sensors_(uint32_t gpio_state) {
  for (auto *sensor : binary_sensors_) {
    sensor->process_gpio_state(gpio_state);
  }
}

// Core I2C methods

bool SeesawDevice::write_register(uint8_t module, uint8_t reg, const uint8_t *data, size_t len) {
  // Seesaw uses two-byte addressing: [module_base, function_register]
  std::vector<uint8_t> buffer;
  buffer.reserve(2 + len);
  buffer.push_back(module);
  buffer.push_back(reg);
  if (data != nullptr && len > 0) {
    buffer.insert(buffer.end(), data, data + len);
  }

  auto result = this->write(buffer.data(), buffer.size());
  if (result != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "I2C write failed: module=0x%02X reg=0x%02X error=%d", module, reg, result);
    return false;
  }
  return true;
}

bool SeesawDevice::write_register(uint8_t module, uint8_t reg) {
  return write_register(module, reg, nullptr, 0);
}

bool SeesawDevice::read_register(uint8_t module, uint8_t reg, uint8_t *data, size_t len) {
  // Write the address first
  uint8_t addr[2] = {module, reg};
  auto write_result = this->write(addr, 2, false);  // Don't send stop
  if (write_result != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "I2C address write failed: module=0x%02X reg=0x%02X error=%d", module, reg, write_result);
    return false;
  }

  // Seesaw requires a delay between write and read
  delayMicroseconds(SEESAW_DELAY_US);

  // Now read the data
  auto read_result = this->read(data, len);
  if (read_result != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "I2C read failed: module=0x%02X reg=0x%02X error=%d", module, reg, read_result);
    return false;
  }

  return true;
}

// GPIO helpers

bool SeesawDevice::read_gpio_bulk(uint32_t *value) {
  uint8_t buf[4];
  if (!read_register(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK, buf, 4)) {
    return false;
  }

  // Big-endian 32-bit value
  *value = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) |
           ((uint32_t)buf[2] << 8) | buf[3];
  return true;
}

bool SeesawDevice::set_gpio_input_pullup(uint32_t pin_mask) {
  // First set direction to input
  uint8_t mask_buf[4] = {
    (uint8_t)(pin_mask >> 24),
    (uint8_t)(pin_mask >> 16),
    (uint8_t)(pin_mask >> 8),
    (uint8_t)(pin_mask)
  };

  if (!write_register(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, mask_buf, 4)) {
    return false;
  }

  // Then enable pull-up
  if (!write_register(SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, mask_buf, 4)) {
    return false;
  }

  // Set the pin high to select pull-up (vs pull-down)
  if (!write_register(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_SET, mask_buf, 4)) {
    return false;
  }

  return true;
}

bool SeesawDevice::set_gpio_input(uint32_t pin_mask) {
  uint8_t mask_buf[4] = {
    (uint8_t)(pin_mask >> 24),
    (uint8_t)(pin_mask >> 16),
    (uint8_t)(pin_mask >> 8),
    (uint8_t)(pin_mask)
  };

  return write_register(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, mask_buf, 4);
}

// NeoPixel helpers

bool SeesawDevice::init_neopixel(uint8_t pin, uint16_t num_pixels, uint8_t bytes_per_pixel) {
  // Set NeoPixel output pin
  if (!write_register(SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_PIN, &pin, 1)) {
    ESP_LOGE(TAG, "Failed to set NeoPixel pin");
    return false;
  }

  // Set speed to 800kHz
  uint8_t speed = SEESAW_NEOPIXEL_SPEED_800KHZ;
  if (!write_register(SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_SPEED, &speed, 1)) {
    ESP_LOGE(TAG, "Failed to set NeoPixel speed");
    return false;
  }

  // Set buffer length (big-endian 16-bit)
  uint16_t buf_len = num_pixels * bytes_per_pixel;
  uint8_t len_buf[2] = {(uint8_t)(buf_len >> 8), (uint8_t)(buf_len)};
  if (!write_register(SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_BUF_LENGTH, len_buf, 2)) {
    ESP_LOGE(TAG, "Failed to set NeoPixel buffer length");
    return false;
  }

  ESP_LOGD(TAG, "NeoPixel initialized: pin=%d, pixels=%d, bytes_per_pixel=%d",
           pin, num_pixels, bytes_per_pixel);
  return true;
}

bool SeesawDevice::write_neopixel_buffer(uint16_t offset, const uint8_t *data, size_t len) {
  // NeoPixel buffer write format: [offset_high, offset_low, data...]
  // Max chunk size is limited by I2C buffer (typically 30 bytes of data)
  std::vector<uint8_t> buffer;
  buffer.reserve(2 + len);
  buffer.push_back((uint8_t)(offset >> 8));
  buffer.push_back((uint8_t)(offset));
  buffer.insert(buffer.end(), data, data + len);

  return write_register(SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_BUF, buffer.data(), buffer.size());
}

bool SeesawDevice::show_neopixels() {
  return write_register(SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_SHOW);
}

}  // namespace seesaw
}  // namespace esphome
