import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID

MULTI_CONF = True
DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["binary_sensor", "light"]

CONF_SEESAW_ID = "seesaw_id"
CONF_SOFTWARE_RESET = "software_reset"

seesaw_ns = cg.esphome_ns.namespace("seesaw")
SeesawDevice = seesaw_ns.class_("SeesawDevice", cg.PollingComponent, i2c.I2CDevice)

CONFIG_SCHEMA = (
    cv.Schema({
        cv.GenerateID(): cv.declare_id(SeesawDevice),
        cv.Optional(CONF_SOFTWARE_RESET, default=True): cv.boolean,
    })
    .extend(cv.polling_component_schema("20ms"))
    .extend(i2c.i2c_device_schema(0x30))  # Default NeoKey 1x4 address
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_software_reset(config[CONF_SOFTWARE_RESET]))
