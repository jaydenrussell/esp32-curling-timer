import esphome.codegen as cg
from esphome import pins
from esphome.components import display, esp32
from esphome.components.const import BYTE_ORDER_LITTLE, CONF_DRAW_ROUNDING
from esphome.components.display import CONF_SHOW_TEST_CARD
from esphome.components.esp32 import VARIANT_ESP32P4, only_on_variant
import esphome.config_validation as cv
from esphome.const import (
    CONF_AUTO_CLEAR_ENABLED,
    CONF_ENABLE_PIN,
    CONF_ID,
    CONF_LAMBDA,
    CONF_PAGES,
    CONF_RESET_PIN,
    CONF_ROTATION,
)
from esphome.final_validate import full_config
from esphome.types import ConfigType

DEPENDENCIES = ["esp32", "esp_ldo", "psram"]
AUTO_LOAD = ["display"]
CODEOWNERS = ["@jaydenrussell"]
DOMAIN = "lt8912b"

lt8912b_ns = cg.esphome_ns.namespace("lt8912b")
Lt8912b = lt8912b_ns.class_("Lt8912b", display.Display, cg.Component)

CONF_SDA_PIN = "sda_pin"
CONF_SCL_PIN = "scl_pin"
CONF_I2C_FREQUENCY = "i2c_frequency"
CONF_LANE_BIT_RATE = "lane_bit_rate"

WIDTH = 1280
HEIGHT = 720


def requires_buffer(config) -> bool:
    return any(config.get(key) for key in (CONF_LAMBDA, CONF_PAGES, CONF_SHOW_TEST_CARD))


def _add_metadata(config: ConfigType) -> ConfigType:
    display.add_metadata(
        config[CONF_ID],
        WIDTH,
        HEIGHT,
        has_hardware_rotation=False,
        byte_order=BYTE_ORDER_LITTLE,
        has_writer=requires_buffer(config) or config.get(CONF_AUTO_CLEAR_ENABLED) is True,
        rotation=config.get(CONF_ROTATION, 0),
        draw_rounding=config.get(CONF_DRAW_ROUNDING, 2),
    )
    return config


CONFIG_SCHEMA = cv.All(
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Lt8912b),
            cv.Optional(CONF_RESET_PIN, default="GPIO4"): pins.gpio_output_pin_schema,
            cv.Optional(CONF_ENABLE_PIN, default="GPIO33"): pins.gpio_output_pin_schema,
            cv.Optional(CONF_SDA_PIN, default="GPIO0"): pins.internal_gpio_input_pin_schema,
            cv.Optional(CONF_SCL_PIN, default="GPIO1"): pins.internal_gpio_input_pin_schema,
            cv.Optional(CONF_I2C_FREQUENCY, default="100kHz"): cv.frequency,
            cv.Optional(CONF_LANE_BIT_RATE, default="960Mbps"): cv.All(
                cv.bps, cv.Range(min=100e6, max=3200e6)
            ),
            cv.Optional(CONF_DRAW_ROUNDING, default=2): cv.positive_int,
        }
    ),
    cv.only_on_esp32,
    only_on_variant(supported=[VARIANT_ESP32P4]),
    _add_metadata,
)


def _final_validate(config: ConfigType) -> None:
    global_config = full_config.get()

    from esphome.components.lvgl import DOMAIN as LVGL_DOMAIN

    # Force show_test_card off - LVGL drives the display
    config[CONF_SHOW_TEST_CARD] = False


FINAL_VALIDATE_SCHEMA = _final_validate


async def to_code(config: ConfigType) -> None:
    esp32.add_idf_component(name="espressif/esp_lcd_lt8912b", ref="0.2.1")

    var = cg.new_Pvariable(config[CONF_ID], WIDTH, HEIGHT)
    for key, setter in (
        (CONF_RESET_PIN, "set_reset_pin"),
        (CONF_ENABLE_PIN, "set_enable_pin"),
        (CONF_SDA_PIN, "set_sda_pin"),
        (CONF_SCL_PIN, "set_scl_pin"),
    ):
        pin = await cg.gpio_pin_expression(config[key])
        cg.add(getattr(var, setter)(pin))
    cg.add(var.set_i2c_frequency(int(config[CONF_I2C_FREQUENCY])))
    cg.add(var.set_lane_bit_rate(config[CONF_LANE_BIT_RATE] / 1.0e6))

    await display.register_display(var, config)
    if lamb := config.get(CONF_LAMBDA):
        lambda_ = await cg.process_lambda(lamb, [(display.DisplayRef, "it")], return_type=cg.void)
        cg.add(var.set_writer(lambda_))
