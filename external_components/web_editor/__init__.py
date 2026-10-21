from pathlib import Path

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import curling_engine, esp32
from esphome.const import CONF_ID, CONF_PASSWORD, CONF_PORT
from esphome.const import __version__ as ESPHOME_VERSION

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


WEB_UI_DELIMITER = "esphome_web_ui"


def emit_web_ui_header() -> None:
    """Read web_ui.html and (re)generate web_ui.h as a raw string literal.

    Runs at codegen time so the {{version}} placeholder always shows the
    ESPHome version of this build, and only rewrites the header when the
    HTML actually changed (a plain compile-flag define would blow the
    Windows command line limit for a page this size).
    """
    here = Path(__file__).parent
    html = (here / "web_ui.html").read_text(encoding="utf-8")
    if "{{version}}" not in html:
        raise cv.Invalid("web_ui.html must keep its {{version}} placeholder")
    html = html.replace("{{version}}", f"v{ESPHOME_VERSION}")
    content = (
        "// AUTO-GENERATED from web_ui.html by web_editor __init__.py at codegen"
        " time — edit web_ui.html, not this file.\n"
        "#pragma once\n\n"
        f'static const char WEB_UI_HTML[] = R"{WEB_UI_DELIMITER}('
        f"{html}){WEB_UI_DELIMITER}\";\n"
    )
    header = here / "web_ui.h"
    if header.exists() and header.read_text(encoding="utf-8") == content:
        return
    header.write_text(content, encoding="utf-8", newline="\n")


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    engine = await cg.get_variable(config[CONF_ENGINE_ID])
    cg.add(var.set_engine(engine))
    cg.add(var.set_password(config[CONF_PASSWORD]))
    cg.add(var.set_port(config[CONF_PORT]))
    # not in DEFAULT_EXCLUDED_IDF_COMPONENTS' allowlist — re-enable it
    esp32.include_builtin_idf_component("esp_http_server")
    emit_web_ui_header()
