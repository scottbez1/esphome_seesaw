# ESPHome Seesaw Component

A barebones vibe-coded ESPHome external component for Adafruit Seesaw devices over I2C.

This implementation provides binary sensor and addressable light support for Seesaw-based products such as the NeoKey 1x4.

## Supported Features

- **Binary Sensors**: GPIO input pins with optional pull-up resistors
- **Addressable Lights**: NeoPixel control with RGB and RGBW color orders
- **Hardware**: SAMD09, ATtiny806/807/816/817/1616/1617 Seesaw firmware variants

## Not Supported

The Adafruit Seesaw platform includes many features that are **not implemented** in this component:

- Analog inputs (ADC)
- Analog outputs (DAC)
- PWM / Timer outputs
- GPIO digital outputs
- Rotary encoders
- Capacitive touch sensing
- UART / SERCOM communication
- EEPROM access
- Temperature sensor
- Hardware interrupts

## Installation

Add the external component to your ESPHome configuration:

```yaml
external_components:
  - source: github://scottbez1/esphome_seesaw
```

## Example Configuration

The following example demonstrates a NeoKey 1x4 with four button binary sensors and four addressable LEDs:

```yaml
i2c:
  # Define a named I2C bus if you have multiple buses
  # - id: bus_a
  #   sda: GPIO21
  #   scl: GPIO22

seesaw:
  - id: neokey
    # To use a non-default I2C bus, specify:
    # i2c_id: bus_a
    #
    # Default address is 0x30. For a different address:
    # address: 0x31

binary_sensor:
  - platform: seesaw
    seesaw_id: neokey
    name: "Key A"
    pin: 4

  - platform: seesaw
    seesaw_id: neokey
    name: "Key B"
    pin: 5

  - platform: seesaw
    seesaw_id: neokey
    name: "Key C"
    pin: 6

  - platform: seesaw
    seesaw_id: neokey
    name: "Key D"
    pin: 7

light:
  - platform: seesaw
    seesaw_id: neokey
    name: "NeoKey LEDs"
    num_leds: 4
    # NeoKey 1x4 uses pin 3 for NeoPixels (default)
    # pin: 3
    # color_order: GRB
```

## Configuration Options

### Seesaw Hub

| Option | Required | Default | Description |
|--------|----------|---------|-------------|
| `id` | Yes | - | Identifier for this Seesaw device |
| `address` | No | `0x30` | I2C address |
| `i2c_id` | No | - | ID of I2C bus to use |
| `update_interval` | No | `20ms` | Polling interval for binary sensors |

### Binary Sensor

| Option | Required | Default | Description |
|--------|----------|---------|-------------|
| `seesaw_id` | Yes | - | ID of parent Seesaw device |
| `pin` | Yes | - | GPIO pin number (0-31) |
| `pin_mode` | No | `INPUT_PULLUP` | `INPUT` or `INPUT_PULLUP` |
| `inverted` | No | `true` | Invert the pin state |

### Light

| Option | Required | Default | Description |
|--------|----------|---------|-------------|
| `seesaw_id` | Yes | - | ID of parent Seesaw device |
| `num_leds` | Yes | - | Number of LEDs |
| `pin` | No | `3` | NeoPixel data pin |
| `color_order` | No | `GRB` | Pixel color order |

Supported color orders: `RGB`, `RBG`, `GRB`, `GBR`, `BRG`, `BGR`, `RGBW`, `RBGW`, `GRBW`, `GBRW`, `BRGW`, `BGRW`
