from pathlib import Path

here = Path(r"C:\Open Code\esp32-curling-timer\external_components\web_editor")
html = (here / "web_ui.html").read_text(encoding="utf-8")
html = html.replace("{{version}}", "v2026.11.201")
content = (
    "// AUTO-GENERATED from web_ui.html by web_editor __init__.py at codegen"
    " time — edit web_ui.html, not this file.\n"
    "#pragma once\n\n"
    f'static const char WEB_UI_HTML[] = R"esphome_web_ui('
    f"{html})esphome_web_ui\\\";\n"
)
header = here / "web_ui.h"
header.write_text(content, encoding="utf-8", newline="\n")
print("Generated", header)