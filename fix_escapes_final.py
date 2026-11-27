with open('external_components/web_editor/web_ui.html', 'r', encoding='utf-8') as f:
    content = f.read()

count_bt = content.count(r'\`')
count_dollar = content.count(r'\${')
print(f'Backslash-backtick count: {count_bt}')
print(f'Backslash-dollar count: {count_dollar}')

content = content.replace(r'\`', '`')
content = content.replace(r'\${', '${')

with open('external_components/web_editor/web_ui.html', 'w', encoding='utf-8') as f:
    f.write(content)

# Verify
with open('external_components/web_editor/web_ui.html', 'r', encoding='utf-8') as f:
    content2 = f.read()
count_bt2 = content2.count(r'\`')
count_dollar2 = content2.count(r'\${')
print(f'Backslash-backtick count after: {count_bt2}')
print(f'Backslash-dollar count after: {count_dollar2}')
print('DONE')