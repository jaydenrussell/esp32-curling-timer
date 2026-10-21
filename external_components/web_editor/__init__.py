import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import curling_engine, esp32
from esphome.const import CONF_ID, CONF_PASSWORD, CONF_PORT

DEPENDENCIES = ["curling_engine", "esp32", "json"]

CONF_ENGINE_ID = "engine_id"

web_editor_ns = cg.esphome_ns.namespace("web_editor")
WebEditorComponent = web_editor_ns.class_("WebEditorComponent", cg.Component)

CONFIG_SCHEMA = cv.COMPONENT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(WebEditorComponent),
        cv.GenerateID(CONF_ENGINE_ID): cv.use_id(
            curling_engine.CurlingEngineComponent
        ),
        cv.Required(CONF_PASSWORD): cv.string,
        cv.Optional(CONF_PORT, default=8080): cv.port,
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    engine = await cg.get_variable(config[CONF_ENGINE_ID])
    cg.add(var.set_engine(engine))
    cg.add(var.set_password(config[CONF_PASSWORD]))
    cg.add(var.set_port(config[CONF_PORT]))
    # not in DEFAULT_EXCLUDED_IDF_COMPONENTS' allowlist — re-enable it
    esp32.include_builtin_idf_component("esp_http_server")
