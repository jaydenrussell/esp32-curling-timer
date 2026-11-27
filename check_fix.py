with open('external_components/web_editor/web_ui.html', 'r', encoding='utf-8') as f:
    content = f.read()

count_backtick = content.count(r'\`')
count_dollar = content.count(r'\${')
print(f'Backslash-backtick count: {count_backtick}')
print(f'Backslash-dollar count: {count_dollar}')