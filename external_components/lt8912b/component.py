import esphome.config_validation as cv
from esphome.components import display
from esphome import pins
from esphome.const import (
    CONF_ID,
    CONF_UPDATE_INTERVAL,
    CONF_I2C_ID,
    CONF_SDA_PIN,
    CONF_SCL_PIN,
    CONF_RESET_PIN,
    CONF_WIDTH,
    CONF_HEIGHT,
)

CONF_MIPI_DSI_ID = "mipi_dsi_id"
CONF_REFRESH_RATE = "refresh_rate"

AUTO_LOAD = ["display", "mipi_dsi"]
DEPENDENCIES = ["esp32", "i2c"]

lt8912b_ns = cg.esphome_ns.namespace("lt8912b")
LT8912BDisplay = lt8912b_ns.class_("LT8912BDisplay", display.Display)

MODES = {
    "720P60": (1280, 720, 60),
    "1080P30": (1920, 1080, 30),
}

CONFIG_SCHEMA = display.FULL_DISPLAY_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(LT8912BDisplay),
        cv.Optional(CONF_I2C_ID): cv.use_variable_id(i2c.I2CBus),
        cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_WIDTH): cv.positive_int,
        cv.Required(CONF_HEIGHT): cv.positive_int,
        cv.Optional(CONF_REFRESH_RATE, default=60): cv.positive_int,
        cv.Optional(CONF_UPDATE_INTERVAL, default="16ms"): cv.positive_time_period_milliseconds,
    }
).extend(cv.polling_component_schema("16ms"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)
    cg.add(var.set_dimensions(config[CONF_WIDTH], config[CONF_HEIGHT]))
    cg.add(var.set_refresh_rate(config[CONF_REFRESH_RATE]))
    if CONF_RESET_PIN in config:
        rst = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(rst))
    if CONF_I2C_ID in config:
        bus = await cg.get_variable(config[CONF_I2C_ID])
        cg.add(var.set_i2c_bus(bus))
