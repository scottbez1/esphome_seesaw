import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_ID, CONF_PIN, CONF_NUM_LEDS, CONF_OUTPUT_ID

from . import seesaw_ns, SeesawDevice, CONF_SEESAW_ID

DEPENDENCIES = ["seesaw"]

CONF_COLOR_ORDER = "color_order"

SeesawNeoPixelLight = seesaw_ns.class_(
    "SeesawNeoPixelLight", light.AddressableLight
)

SeesawColorOrder = seesaw_ns.enum("SeesawColorOrder")
COLOR_ORDERS = {
    "RGB": SeesawColorOrder.SEESAW_COLOR_ORDER_RGB,
    "RBG": SeesawColorOrder.SEESAW_COLOR_ORDER_RBG,
    "GRB": SeesawColorOrder.SEESAW_COLOR_ORDER_GRB,
    "GBR": SeesawColorOrder.SEESAW_COLOR_ORDER_GBR,
    "BRG": SeesawColorOrder.SEESAW_COLOR_ORDER_BRG,
    "BGR": SeesawColorOrder.SEESAW_COLOR_ORDER_BGR,
    "RGBW": SeesawColorOrder.SEESAW_COLOR_ORDER_RGBW,
    "RBGW": SeesawColorOrder.SEESAW_COLOR_ORDER_RBGW,
    "GRBW": SeesawColorOrder.SEESAW_COLOR_ORDER_GRBW,
    "GBRW": SeesawColorOrder.SEESAW_COLOR_ORDER_GBRW,
    "BRGW": SeesawColorOrder.SEESAW_COLOR_ORDER_BRGW,
    "BGRW": SeesawColorOrder.SEESAW_COLOR_ORDER_BGRW,
}

CONFIG_SCHEMA = light.ADDRESSABLE_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(SeesawNeoPixelLight),
    cv.GenerateID(CONF_SEESAW_ID): cv.use_id(SeesawDevice),
    cv.Required(CONF_NUM_LEDS): cv.positive_not_null_int,
    cv.Optional(CONF_PIN, default=3): cv.int_range(min=0, max=31),
    cv.Optional(CONF_COLOR_ORDER, default="GRB"): cv.enum(COLOR_ORDERS, upper=True),
})


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await light.register_light(var, config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_SEESAW_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_num_leds(config[CONF_NUM_LEDS]))
    cg.add(var.set_pin(config[CONF_PIN]))
    cg.add(var.set_color_order(config[CONF_COLOR_ORDER]))
