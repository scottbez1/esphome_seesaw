#pragma once

// Adafruit Seesaw Register Definitions
// Reference: https://github.com/adafruit/Adafruit_Seesaw

namespace esphome {
namespace seesaw {

// Module Base Addresses
constexpr uint8_t SEESAW_STATUS_BASE = 0x00;
constexpr uint8_t SEESAW_GPIO_BASE = 0x01;
constexpr uint8_t SEESAW_SERCOM0_BASE = 0x02;
constexpr uint8_t SEESAW_TIMER_BASE = 0x08;
constexpr uint8_t SEESAW_ADC_BASE = 0x09;
constexpr uint8_t SEESAW_DAC_BASE = 0x0A;
constexpr uint8_t SEESAW_INTERRUPT_BASE = 0x0B;
constexpr uint8_t SEESAW_DAP_BASE = 0x0C;
constexpr uint8_t SEESAW_EEPROM_BASE = 0x0D;
constexpr uint8_t SEESAW_NEOPIXEL_BASE = 0x0E;
constexpr uint8_t SEESAW_TOUCH_BASE = 0x0F;
constexpr uint8_t SEESAW_ENCODER_BASE = 0x11;

// Status Module Registers (SEESAW_STATUS_BASE)
constexpr uint8_t SEESAW_STATUS_HW_ID = 0x01;
constexpr uint8_t SEESAW_STATUS_VERSION = 0x02;
constexpr uint8_t SEESAW_STATUS_OPTIONS = 0x03;
constexpr uint8_t SEESAW_STATUS_TEMP = 0x04;
constexpr uint8_t SEESAW_STATUS_SWRST = 0x7F;

// GPIO Module Registers (SEESAW_GPIO_BASE)
constexpr uint8_t SEESAW_GPIO_DIRSET_BULK = 0x02;
constexpr uint8_t SEESAW_GPIO_DIRCLR_BULK = 0x03;
constexpr uint8_t SEESAW_GPIO_BULK = 0x04;
constexpr uint8_t SEESAW_GPIO_BULK_SET = 0x05;
constexpr uint8_t SEESAW_GPIO_BULK_CLR = 0x06;
constexpr uint8_t SEESAW_GPIO_BULK_TOGGLE = 0x07;
constexpr uint8_t SEESAW_GPIO_INTENSET = 0x08;
constexpr uint8_t SEESAW_GPIO_INTENCLR = 0x09;
constexpr uint8_t SEESAW_GPIO_INTFLAG = 0x0A;
constexpr uint8_t SEESAW_GPIO_PULLENSET = 0x0B;
constexpr uint8_t SEESAW_GPIO_PULLENCLR = 0x0C;

// NeoPixel Module Registers (SEESAW_NEOPIXEL_BASE)
constexpr uint8_t SEESAW_NEOPIXEL_STATUS = 0x00;
constexpr uint8_t SEESAW_NEOPIXEL_PIN = 0x01;
constexpr uint8_t SEESAW_NEOPIXEL_SPEED = 0x02;
constexpr uint8_t SEESAW_NEOPIXEL_BUF_LENGTH = 0x03;
constexpr uint8_t SEESAW_NEOPIXEL_BUF = 0x04;
constexpr uint8_t SEESAW_NEOPIXEL_SHOW = 0x05;

// NeoPixel Speed Values
constexpr uint8_t SEESAW_NEOPIXEL_SPEED_400KHZ = 0x00;
constexpr uint8_t SEESAW_NEOPIXEL_SPEED_800KHZ = 0x01;

// Hardware IDs
constexpr uint8_t SEESAW_HW_ID_CODE_SAMD09 = 0x55;
constexpr uint8_t SEESAW_HW_ID_CODE_TINY806 = 0x84;
constexpr uint8_t SEESAW_HW_ID_CODE_TINY807 = 0x85;
constexpr uint8_t SEESAW_HW_ID_CODE_TINY816 = 0x86;
constexpr uint8_t SEESAW_HW_ID_CODE_TINY817 = 0x87;
constexpr uint8_t SEESAW_HW_ID_CODE_TINY1616 = 0x88;
constexpr uint8_t SEESAW_HW_ID_CODE_TINY1617 = 0x89;

// Default I2C Addresses
constexpr uint8_t SEESAW_ADDRESS_NEOKEY_1X4 = 0x30;
constexpr uint8_t SEESAW_ADDRESS_NEOSLIDER = 0x30;
constexpr uint8_t SEESAW_ADDRESS_ROTARY = 0x36;

// NeoKey 1x4 specific
constexpr uint8_t NEOKEY_1X4_NEOPIXEL_PIN = 3;
constexpr uint8_t NEOKEY_1X4_BUTTON_PIN_0 = 4;
constexpr uint8_t NEOKEY_1X4_BUTTON_PIN_1 = 5;
constexpr uint8_t NEOKEY_1X4_BUTTON_PIN_2 = 6;
constexpr uint8_t NEOKEY_1X4_BUTTON_PIN_3 = 7;
constexpr uint32_t NEOKEY_1X4_BUTTON_MASK = 0xF0;  // Pins 4-7

// Seesaw I2C protocol timing
constexpr uint16_t SEESAW_DELAY_US = 250;  // Delay between write and read

}  // namespace seesaw
}  // namespace esphome
