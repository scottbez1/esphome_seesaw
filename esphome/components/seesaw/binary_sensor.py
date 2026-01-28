import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_PIN, CONF_INVERTED

from . import seesaw_ns, SeesawDevice, CONF_SEESAW_ID

DEPENDENCIES = ["seesaw"]

CONF_PIN_MODE = "pin_mode"

SeesawGPIOBinarySensor = seesaw_ns.class_(
    "SeesawGPIOBinarySensor", binary_sensor.BinarySensor, cg.Component
)

SeesawGPIOPinMode = seesaw_ns.enum("SeesawGPIOPinMode")
PIN_MODES = {
    "INPUT": SeesawGPIOPinMode.SEESAW_PIN_MODE_INPUT,
    "INPUT_PULLUP": SeesawGPIOPinMode.SEESAW_PIN_MODE_INPUT_PULLUP,
}

CONFIG_SCHEMA = (
    binary_sensor.binary_sensor_schema(SeesawGPIOBinarySensor)
    .extend({
        cv.GenerateID(CONF_SEESAW_ID): cv.use_id(SeesawDevice),
        cv.Required(CONF_PIN): cv.int_range(min=0, max=31),
        cv.Optional(CONF_PIN_MODE, default="INPUT_PULLUP"): cv.enum(PIN_MODES, upper=True),
        cv.Optional(CONF_INVERTED, default=True): cv.boolean,
    })
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await binary_sensor.new_binary_sensor(config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_SEESAW_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_pin(config[CONF_PIN]))
    cg.add(var.set_pin_mode(config[CONF_PIN_MODE]))
    cg.add(var.set_inverted(config[CONF_INVERTED]))
