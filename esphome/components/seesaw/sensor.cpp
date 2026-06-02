#include "sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace seesaw {

static const char *const TAG = "seesaw.sensor";

void SeesawEncoderSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Seesaw Encoder Sensor (encoder %d)...", encoder_);
  parent_->register_encoder_sensor(this);
}

void SeesawEncoderSensor::dump_config() {
  LOG_SENSOR("", "Seesaw Encoder Sensor", this);
  ESP_LOGCONFIG(TAG, "  Encoder: %d", encoder_);
}

float SeesawEncoderSensor::get_setup_priority() const {
  return setup_priority::DATA - 1.0f;
}

void SeesawEncoderSensor::update_from_parent() {
  int32_t position;
  if (!parent_->read_encoder_position(encoder_, &position)) {
    ESP_LOGW(TAG, "Failed to read encoder %d position", encoder_);
    return;
  }
  if (position != last_position_) {
    last_position_ = position;
    this->publish_state((float) position);
  }
}

}  // namespace seesaw
}  // namespace esphome
