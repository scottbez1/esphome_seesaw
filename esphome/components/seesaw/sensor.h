#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "seesaw.h"

namespace esphome {
namespace seesaw {

class SeesawEncoderSensor : public sensor::Sensor, public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;

  void set_parent(SeesawDevice *parent) { parent_ = parent; }
  void set_encoder(uint8_t encoder) { encoder_ = encoder; }

  // Called by parent during update()
  void update_from_parent();

 protected:
  SeesawDevice *parent_{nullptr};
  uint8_t encoder_{0};
  int32_t last_position_{0};
};

}  // namespace seesaw
}  // namespace esphome
