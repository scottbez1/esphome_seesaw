#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "seesaw.h"

namespace esphome {
namespace seesaw {

enum SeesawGPIOPinMode : uint8_t {
  SEESAW_PIN_MODE_INPUT = 0,
  SEESAW_PIN_MODE_INPUT_PULLUP = 1,
};

class SeesawGPIOBinarySensor : public binary_sensor::BinarySensor, public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;

  void set_parent(SeesawDevice *parent) { parent_ = parent; }
  void set_pin(uint8_t pin) { pin_ = pin; }
  void set_pin_mode(SeesawGPIOPinMode mode) { pin_mode_ = mode; }
  void set_inverted(bool inverted) { inverted_ = inverted; }

  uint8_t get_pin() const { return pin_; }
  uint32_t get_pin_mask() const { return 1UL << pin_; }

  // Called by parent during update()
  void process_gpio_state(uint32_t gpio_state);

 protected:
  SeesawDevice *parent_{nullptr};
  uint8_t pin_{0};
  SeesawGPIOPinMode pin_mode_{SEESAW_PIN_MODE_INPUT_PULLUP};
  bool inverted_{true};  // Default true for NeoKey buttons (active-low)
  bool last_state_{false};
  bool first_read_{true};
};

}  // namespace seesaw
}  // namespace esphome
