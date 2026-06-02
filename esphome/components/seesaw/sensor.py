import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID

from . import seesaw_ns, SeesawDevice, CONF_SEESAW_ID

DEPENDENCIES = ["seesaw"]

CONF_ENCODER = "encoder"

SeesawEncoderSensor = seesaw_ns.class_(
    "SeesawEncoderSensor", sensor.Sensor, cg.Component
)

CONFIG_SCHEMA = (
    sensor.sensor_schema(SeesawEncoderSensor)
    .extend({
        cv.GenerateID(CONF_SEESAW_ID): cv.use_id(SeesawDevice),
        cv.Optional(CONF_ENCODER, default=0): cv.int_range(min=0, max=7),
    })
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_SEESAW_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_encoder(config[CONF_ENCODER]))
