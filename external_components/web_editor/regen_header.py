#!/usr/bin/env python3
from pathlib import Path

here = Path('.')
version = (here / 'VERSION').read_text(encoding='utf-8').strip()
html = (here / 'web_ui.html').read_text(encoding='utf-8')
html = html.replace('{{version}}', 'v' + version)

content = (
    '// AUTO-GENERATED from web_ui.html by web_editor __init__.py at codegen time \u2014 edit web_ui.html, not this file.\n'
    '#pragma once\n\n'
    'static const char WEB_UI_HTML[] = R"esphome_web_ui('
    + html + ')esphome_web_ui";\n'
)

header = here / 'web_ui.h'
header.write_text(content, encoding='utf-8', newline='\n')
print('Header regenerated successfully')
print('Header length:', len(content))