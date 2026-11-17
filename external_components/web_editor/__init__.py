from pathlib import Path
import gzip
import subprocess

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import curling_engine, esp32, text_sensor
from esphome.const import (
    CONF_ENTITY_CATEGORY,
    CONF_ICON,
    CONF_ID,
    CONF_NAME,
    CONF_PASSWORD,
    CONF_PORT,
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from esphome.const import __version__ as ESPHOME_VERSION
from esphome.core import ID

# "network" is required: its setup() calls esp_netif_init(), which brings
# up lwIP and creates the TCP/IP core lock. Starting the HTTP server
# before that aborts the chip - the first socket() call takes a NULL
# core-lock mutex (xSemaphoreTake -> assert).
DEPENDENCIES = ["curling_engine", "esp32", "json", "text_sensor", "network"]

CONF_ENGINE_ID = "engine_id"

# The diagnostic entity belongs to the component, not to the YAML: the
# YAML is pasted into Home Assistant and may be newer than the external
# component clone the Device Builder has cached, so the YAML must not
# call component APIs (that would break the build with a stale clone).
STATUS_SENSOR_ID = "web_editor_status"

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


def component_version() -> str:
    """Build stamp of the component.

    When fetched as a git external_component, prefers `git describe
    --tags --always` so the version reflects the actual compiled commit
    (and updates automatically on `ref: main` pulls). Falls back to the
    VERSION file (bumped on releases) when git isn't available.
    """
    here = Path(__file__).parent
    try:
        desc = subprocess.check_output(
            ["git", "describe", "--tags", "--always"],
            cwd=here,
            stderr=subprocess.DEVNULL,
            timeout=5,
        ).decode().strip()
        if desc:
            return desc
    except (subprocess.SubprocessError, OSError, FileNotFoundError):
        pass
    return (here / "VERSION").read_text(encoding="utf-8").strip()


def emit_web_ui_header() -> None:
    """Read web_ui.html and (re)generate web_ui.h as a gzip-compressed byte array.

    Runs at codegen time so the {{version}} placeholder always shows the
    component build version, and only rewrites the header when the emitted
    content actually changed.
    """
    here = Path(__file__).parent
    html = (here / "web_ui.html").read_text(encoding="utf-8")
    if "{{version}}" not in html:
        raise cv.Invalid("web_ui.html must keep its {{version}} placeholder")
    html = html.replace("{{version}}", component_version())
    gz = gzip.compress(html.encode("utf-8"), compresslevel=9)
    bytes_per_line = 12
    byte_lines = []
    for i in range(0, len(gz), bytes_per_line):
        chunk = gz[i : i + bytes_per_line]
        byte_lines.append("  " + ", ".join(f"0x{b:02x}" for b in chunk))
    content = (
        "// AUTO-GENERATED from web_ui.html by web_editor __init__.py at codegen"
        " time — edit web_ui.html (frontend source/artifact), not this file.\n"
        "#pragma once\n\n"
        "#include <stddef.h>\n"
        "#include <stdint.h>\n\n"
        "static const uint8_t WEB_UI_HTML_GZ[] = {\n"
        + ",\n".join(byte_lines)
        + "\n};\n"
        "static const size_t WEB_UI_HTML_GZ_LEN = sizeof(WEB_UI_HTML_GZ);\n"
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
    # not in DEFAULT_EXCLUDED_IDF_COMPONENTS' allowlist — re-enable them
    esp32.include_builtin_idf_component("esp_http_server")
    esp32.include_builtin_idf_component("esp_http_client")
    esp32.include_builtin_idf_component("esp_crt_bundle")
    # Diagnostic entity: "listening on port 8080" or the reason the web
    # editor is not reachable. Created here so the YAML stays free of
    # component internals.
    status = await text_sensor.new_text_sensor(
        text_sensor.text_sensor_schema(
            text_sensor.TextSensor,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:web",
        )(
            {
                CONF_ID: ID(STATUS_SENSOR_ID, type=text_sensor.TextSensor),
                CONF_NAME: "Web Editor",
            }
        )
    )
    cg.add(var.set_status_sensor(status))
    # Build stamp of the component, shown in the diagnostic entity. It
    # tells a fresh clone apart from a stale one (the Device Builder
    # caches the git checkout), so bump the VERSION file with every
    # change to the component.
    cg.add_define("WEB_EDITOR_VERSION", f'"{component_version()}"')
    emit_web_ui_header()
