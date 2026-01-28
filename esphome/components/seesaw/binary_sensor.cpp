#include "binary_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace seesaw {

static const char *const TAG = "seesaw.binary_sensor";

void SeesawGPIOBinarySensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Seesaw GPIO Binary Sensor on pin %d...", pin_);

  // Configure the GPIO pin
  uint32_t pin_mask = get_pin_mask();

  switch (pin_mode_) {
    case SEESAW_PIN_MODE_INPUT:
      if (!parent_->set_gpio_input(pin_mask)) {
        ESP_LOGE(TAG, "Failed to configure pin %d as input", pin_);
        this->mark_failed();
        return;
      }
      break;
    case SEESAW_PIN_MODE_INPUT_PULLUP:
      if (!parent_->set_gpio_input_pullup(pin_mask)) {
        ESP_LOGE(TAG, "Failed to configure pin %d as input with pull-up", pin_);
        this->mark_failed();
        return;
      }
      break;
  }

  // Register with parent for updates
  parent_->register_binary_sensor(this);

  ESP_LOGCONFIG(TAG, "Seesaw GPIO Binary Sensor on pin %d configured", pin_);
}

void SeesawGPIOBinarySensor::dump_config() {
  LOG_BINARY_SENSOR("", "Seesaw GPIO Binary Sensor", this);
  ESP_LOGCONFIG(TAG, "  Pin: %d", pin_);
  ESP_LOGCONFIG(TAG, "  Inverted: %s", inverted_ ? "true" : "false");
  const char *mode_str;
  switch (pin_mode_) {
    case SEESAW_PIN_MODE_INPUT:
      mode_str = "INPUT";
      break;
    case SEESAW_PIN_MODE_INPUT_PULLUP:
      mode_str = "INPUT_PULLUP";
      break;
    default:
      mode_str = "UNKNOWN";
  }
  ESP_LOGCONFIG(TAG, "  Pin Mode: %s", mode_str);
}

float SeesawGPIOBinarySensor::get_setup_priority() const {
  // Run after parent Seesaw device
  return setup_priority::DATA - 1.0f;
}

void SeesawGPIOBinarySensor::process_gpio_state(uint32_t gpio_state) {
  // Check if our pin is high or low
  bool pin_high = (gpio_state & get_pin_mask()) != 0;

  // Apply inversion (NeoKey buttons are active-low)
  bool state = inverted_ ? !pin_high : pin_high;

  // Only publish if changed (or first read)
  if (first_read_ || state != last_state_) {
    first_read_ = false;
    last_state_ = state;
    this->publish_state(state);
  }
}

}  // namespace seesaw
}  // namespace esphome
