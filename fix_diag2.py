import sys

with open('external_components/web_editor/web_ui.html', 'r', encoding='utf-8') as f:
    content = f.read()

# Fix escaped backticks
content = content.replace(r'`', '`')
# Fix escaped dollar signs
content = content.replace(r'${', '${')

with open('external_components/web_editor/web_ui.html', 'w', encoding='utf-8') as f:
    f.write(content)

print('SUCCESS: Fixed escaped backticks and dollar signs')