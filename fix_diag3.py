with open('external_components/web_editor/web_ui.html', 'r', encoding='utf-8') as f:
    content = f.read()

# Count escaped characters before fix
count_backtick = content.count(r'\`')
count_dollar = content.count(r'\${')
print(f'Backslash-backtick count: {count_backtick}')
print(f'Backslash-dollar count: {count_dollar}')

# Fix escaped backticks and dollar signs
content = content.replace(r'\`', '`')
content = content.replace(r'\${', '${')

with open('external_components/web_editor/web_ui.html', 'w', encoding='utf-8') as f:
    f.write(content)

# Verify
content2 = f.read() if False else None
# Re-read to verify
with open('external_components/web_editor/web_ui.html', 'r', encoding='utf-8') as f:
    content2 = f.read()
count_backtick2 = content2.count(r'\`')
count_dollar2 = content2.count(r'\${')
print(f'Backslash-backtick count after: {count_backtick2}')
print(f'Backslash-dollar count after: {count_dollar2}')
print('DONE')