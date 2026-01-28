#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include <vector>

#include "seesaw_registers.h"

namespace esphome {
namespace seesaw {

class SeesawGPIOBinarySensor;
class SeesawNeoPixelLight;

class SeesawDevice : public PollingComponent, public i2c::I2CDevice {
 public:
  SeesawDevice() = default;

  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override;

  // Child registration
  void register_binary_sensor(SeesawGPIOBinarySensor *sensor) { binary_sensors_.push_back(sensor); }
  void register_neopixel_light(SeesawNeoPixelLight *light) { neopixel_light_ = light; }

  // Core I2C methods (two-byte addressing with read delay)
  bool write_register(uint8_t module, uint8_t reg, const uint8_t *data, size_t len);
  bool write_register(uint8_t module, uint8_t reg);  // No data, just address
  bool read_register(uint8_t module, uint8_t reg, uint8_t *data, size_t len);

  // GPIO helpers
  bool read_gpio_bulk(uint32_t *value);
  bool set_gpio_input_pullup(uint32_t pin_mask);
  bool set_gpio_input(uint32_t pin_mask);

  // NeoPixel helpers
  bool init_neopixel(uint8_t pin, uint16_t num_pixels, uint8_t bytes_per_pixel);
  bool write_neopixel_buffer(uint16_t offset, const uint8_t *data, size_t len);
  bool show_neopixels();

  // Configuration
  void set_software_reset(bool reset) { software_reset_ = reset; }

 protected:
  bool verify_hardware_id_();
  void configure_gpio_pins_();
  void notify_binary_sensors_(uint32_t gpio_state);

  std::vector<SeesawGPIOBinarySensor *> binary_sensors_;
  SeesawNeoPixelLight *neopixel_light_{nullptr};
  bool software_reset_{true};
  uint8_t hardware_id_{0};
};

}  // namespace seesaw
}  // namespace esphome
