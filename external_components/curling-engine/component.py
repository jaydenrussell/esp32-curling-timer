import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["esp32"]

CONF_STORE_PATH = "store_path"

curling_ns = cg.esphome_ns.namespace("curling_engine")
CurlingEngine = curling_ns.class_("CurlingEngine", cg.Component)
CurlingEngineComponent = curling_ns.class_(
    "CurlingEngineComponent", cg.Component
)

CONFIG_SCHEMA = cv.COMPONENT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(CurlingEngineComponent),
        cv.Optional(CONF_STORE_PATH, default="/flash/data"): cv.string,
        # TODO(v1): switch reference for relay control, time id for sntp,
        # and display id for the LVGL UI state handoff.
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_store_path(config[CONF_STORE_PATH]))
