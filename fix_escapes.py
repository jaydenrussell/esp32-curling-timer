with open('external_components/web_editor/web_ui.html', 'r', encoding='utf-8') as f:
    content = f.read()

count_bt = content.count(r'\`')
count_dollar = content.count(r'\${')
print('Backslash-backtick count:', count_bt)
print('Backslash-dollar count:', count_dollar)

content = content.replace(r'\`', '`')
content = content.replace(r'\${', '${')

with open('external_components/web_editor/web_ui.html', 'w', encoding='utf-8') as f:
    f.write(content)

print('Done')