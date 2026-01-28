#include "light.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

#include <algorithm>
#include <cstring>

namespace esphome {
namespace seesaw {

static const char *const TAG = "seesaw.light";

// Maximum bytes to send in one I2C transaction (Seesaw limit)
static const size_t SEESAW_NEOPIXEL_MAX_CHUNK = 30;

void SeesawNeoPixelLight::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Seesaw NeoPixel Light...");

  uint8_t bpp = bytes_per_pixel_();
  size_t buffer_size = num_leds_ * bpp;

  // Allocate buffers using ESPHome's allocator
  ExternalRAMAllocator<uint8_t> allocator(ExternalRAMAllocator<uint8_t>::ALLOW_FAILURE);
  this->buf_ = allocator.allocate(buffer_size);
  if (this->buf_ == nullptr) {
    ESP_LOGE(TAG, "Failed to allocate LED buffer");
    this->mark_failed();
    return;
  }

  this->effect_data_ = allocator.allocate(num_leds_);
  if (this->effect_data_ == nullptr) {
    ESP_LOGE(TAG, "Failed to allocate effect buffer");
    this->mark_failed();
    return;
  }

  // Initialize buffers to zero (LEDs off)
  memset(this->buf_, 0, buffer_size);
  memset(this->effect_data_, 0, num_leds_);

  // Initialize NeoPixel on Seesaw
  if (!parent_->init_neopixel(pin_, num_leds_, bpp)) {
    ESP_LOGE(TAG, "Failed to initialize NeoPixel on Seesaw");
    this->mark_failed();
    return;
  }

  // Register with parent
  parent_->register_neopixel_light(this);

  ESP_LOGCONFIG(TAG, "Seesaw NeoPixel Light initialized: %d LEDs, %d bytes/pixel", num_leds_, bpp);
}

void SeesawNeoPixelLight::dump_config() {
  ESP_LOGCONFIG(TAG, "Seesaw NeoPixel Light:");
  ESP_LOGCONFIG(TAG, "  Pin: %d", pin_);
  ESP_LOGCONFIG(TAG, "  Number of LEDs: %d", num_leds_);
  ESP_LOGCONFIG(TAG, "  Color Order: %s", is_rgbw_() ? "RGBW" : "RGB");
}

float SeesawNeoPixelLight::get_setup_priority() const {
  // Run after parent Seesaw device
  return setup_priority::DATA - 1.0f;
}

void SeesawNeoPixelLight::get_color_offsets_(uint8_t *r_offset, uint8_t *g_offset,
                                              uint8_t *b_offset, uint8_t *w_offset) const {
  // Map color order enum to byte offsets
  switch (color_order_) {
    case SEESAW_COLOR_ORDER_RGB:
      *r_offset = 0; *g_offset = 1; *b_offset = 2; *w_offset = 3;
      break;
    case SEESAW_COLOR_ORDER_RBG:
      *r_offset = 0; *g_offset = 2; *b_offset = 1; *w_offset = 3;
      break;
    case SEESAW_COLOR_ORDER_GRB:
      *r_offset = 1; *g_offset = 0; *b_offset = 2; *w_offset = 3;
      break;
    case SEESAW_COLOR_ORDER_GBR:
      *r_offset = 2; *g_offset = 0; *b_offset = 1; *w_offset = 3;
      break;
    case SEESAW_COLOR_ORDER_BRG:
      *r_offset = 1; *g_offset = 2; *b_offset = 0; *w_offset = 3;
      break;
    case SEESAW_COLOR_ORDER_BGR:
      *r_offset = 2; *g_offset = 1; *b_offset = 0; *w_offset = 3;
      break;
    case SEESAW_COLOR_ORDER_RGBW:
      *r_offset = 0; *g_offset = 1; *b_offset = 2; *w_offset = 3;
      break;
    case SEESAW_COLOR_ORDER_RBGW:
      *r_offset = 0; *g_offset = 2; *b_offset = 1; *w_offset = 3;
      break;
    case SEESAW_COLOR_ORDER_GRBW:
      *r_offset = 1; *g_offset = 0; *b_offset = 2; *w_offset = 3;
      break;
    case SEESAW_COLOR_ORDER_GBRW:
      *r_offset = 2; *g_offset = 0; *b_offset = 1; *w_offset = 3;
      break;
    case SEESAW_COLOR_ORDER_BRGW:
      *r_offset = 1; *g_offset = 2; *b_offset = 0; *w_offset = 3;
      break;
    case SEESAW_COLOR_ORDER_BGRW:
      *r_offset = 2; *g_offset = 1; *b_offset = 0; *w_offset = 3;
      break;
    default:
      // Default to GRB (NeoKey default)
      *r_offset = 1; *g_offset = 0; *b_offset = 2; *w_offset = 3;
      break;
  }
}

light::ESPColorView SeesawNeoPixelLight::get_view_internal(int32_t index) const {
  uint8_t bpp = bytes_per_pixel_();
  uint8_t r_off, g_off, b_off, w_off;
  get_color_offsets_(&r_off, &g_off, &b_off, &w_off);

  // Safety check - return dummy view if buffers not allocated yet
  if (this->buf_ == nullptr) {
    static uint8_t dummy_buf[4] = {0, 0, 0, 0};
    static uint8_t dummy_effect = 0;
    return light::ESPColorView(&dummy_buf[r_off], &dummy_buf[g_off], &dummy_buf[b_off], nullptr,
                               &dummy_effect, &this->correction_);
  }

  uint8_t *base = this->buf_ + (index * bpp);

  if (is_rgbw_()) {
    return light::ESPColorView(base + r_off, base + g_off, base + b_off, base + w_off,
                               this->effect_data_ + index, &this->correction_);
  } else {
    return light::ESPColorView(base + r_off, base + g_off, base + b_off, nullptr,
                               this->effect_data_ + index, &this->correction_);
  }
}

void SeesawNeoPixelLight::write_state(light::LightState *state) {
  if (this->is_failed() || this->buf_ == nullptr) {
    return;
  }

  uint8_t bpp = bytes_per_pixel_();
  size_t buffer_size = num_leds_ * bpp;

  // Send buffer to Seesaw in chunks
  size_t offset = 0;
  while (offset < buffer_size) {
    size_t chunk_size = std::min(SEESAW_NEOPIXEL_MAX_CHUNK, buffer_size - offset);

    if (!parent_->write_neopixel_buffer(offset, this->buf_ + offset, chunk_size)) {
      ESP_LOGW(TAG, "Failed to write NeoPixel buffer at offset %d", offset);
      return;
    }
    offset += chunk_size;
  }

  // Trigger LED update
  if (!parent_->show_neopixels()) {
    ESP_LOGW(TAG, "Failed to show NeoPixels");
  }
}

}  // namespace seesaw
}  // namespace esphome
