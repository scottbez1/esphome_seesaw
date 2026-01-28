#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/addressable_light.h"
#include "seesaw.h"

namespace esphome {
namespace seesaw {

enum SeesawColorOrder : uint8_t {
  SEESAW_COLOR_ORDER_RGB = 0,
  SEESAW_COLOR_ORDER_RBG = 1,
  SEESAW_COLOR_ORDER_GRB = 2,
  SEESAW_COLOR_ORDER_GBR = 3,
  SEESAW_COLOR_ORDER_BRG = 4,
  SEESAW_COLOR_ORDER_BGR = 5,
  SEESAW_COLOR_ORDER_RGBW = 6,
  SEESAW_COLOR_ORDER_RBGW = 7,
  SEESAW_COLOR_ORDER_GRBW = 8,
  SEESAW_COLOR_ORDER_GBRW = 9,
  SEESAW_COLOR_ORDER_BRGW = 10,
  SEESAW_COLOR_ORDER_BGRW = 11,
};

class SeesawNeoPixelLight : public light::AddressableLight {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void write_state(light::LightState *state) override;

  void set_parent(SeesawDevice *parent) { parent_ = parent; }
  void set_num_leds(uint16_t num_leds) { num_leds_ = num_leds; }
  void set_pin(uint8_t pin) { pin_ = pin; }
  void set_color_order(SeesawColorOrder order) { color_order_ = order; }

  int32_t size() const override { return num_leds_; }

  light::LightTraits get_traits() override {
    auto traits = light::LightTraits();
    if (is_rgbw_()) {
      traits.set_supported_color_modes({light::ColorMode::RGB_WHITE});
    } else {
      traits.set_supported_color_modes({light::ColorMode::RGB});
    }
    return traits;
  }

  void clear_effect_data() override {
    for (int i = 0; i < this->size(); i++) {
      this->effect_data_[i] = 0;
    }
  }

 protected:
  light::ESPColorView get_view_internal(int32_t index) const override;

  bool is_rgbw_() const { return color_order_ >= SEESAW_COLOR_ORDER_RGBW; }
  uint8_t bytes_per_pixel_() const { return is_rgbw_() ? 4 : 3; }

  // Get byte offsets for each color channel based on color order
  void get_color_offsets_(uint8_t *r_offset, uint8_t *g_offset, uint8_t *b_offset, uint8_t *w_offset) const;

  SeesawDevice *parent_{nullptr};
  uint16_t num_leds_{0};
  uint8_t pin_{3};  // Default for NeoKey 1x4
  SeesawColorOrder color_order_{SEESAW_COLOR_ORDER_GRB};  // Default for NeoKey 1x4

  uint8_t *buf_{nullptr};
  uint8_t *effect_data_{nullptr};
};

}  // namespace seesaw
}  // namespace esphome
