import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import time
from esphome.const import CONF_ID, CONF_TIME_ID

DEPENDENCIES = ["esp32"]

CONF_STORE_PATH = "store_path"

curling_ns = cg.esphome_ns.namespace("curling_engine")
CurlingEngineComponent = curling_ns.class_("CurlingEngineComponent", cg.Component)

CONFIG_SCHEMA = cv.COMPONENT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(CurlingEngineComponent),
        # Directory on SPIFFS that holds settings.yml + rolling backups.
        cv.Optional(CONF_STORE_PATH, default="/flash"): cv.string,
        cv.GenerateID(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_store_path(config[CONF_STORE_PATH]))
    time_ = await cg.get_variable(config[CONF_TIME_ID])
    cg.add(var.set_time(time_))
