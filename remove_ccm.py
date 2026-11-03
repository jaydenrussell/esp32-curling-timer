p = 'external_components/curling_engine/config_edit.cpp'
s = open(p, encoding='utf-8').read()
old = '         path == "config.club_website_url" ||\n         path == "config.curling_club_manager_url";'
new = '         path == "config.club_website_url";'
assert old in s, 'config_edit not found'
s = s.replace(old, new)
open(p, 'w', encoding='utf-8').write(s)

p = 'external_components/web_editor/api.cpp'
s = open(p, encoding='utf-8').read()
old = '      cfg["config.club_website_url"] = getval("config.club_website_url");\n      cfg["config.curling_club_manager_url"] = getval("config.curling_club_manager_url");'
new = '      cfg["config.club_website_url"] = getval("config.club_website_url");'
assert old in s, 'api getval not found'
s = s.replace(old, new)
old = '               path == "config.club_website_url" || path == "config.league_page_url" ||\n               path == "config.curling_club_manager_url") {'
new = '               path == "config.club_website_url" || path == "config.league_page_url") {'
assert old in s, 'api validation not found'
s = s.replace(old, new)
open(p, 'w', encoding='utf-8').write(s)

p = 'external_components/web_editor/web_ui.html'
s = open(p, encoding='utf-8').read()
old = "      ['config.curling_club_manager_url', 'text', 'Curling Club Manager URL'],\n      ['config.club_website_url', 'text', 'Club website URL'],"
new = "      ['config.club_website_url', 'text', 'Club website URL (Curling Club Manager)'],"
assert old in s, 'html not found'
s = s.replace(old, new)
open(p, 'w', encoding='utf-8').write(s)

p = 'external_components/web_editor/web_ui.h'
s = open(p, encoding='utf-8').read()
old = "      ['config.curling_club_manager_url', 'text', 'Curling Club Manager URL'],\n      ['config.club_website_url', 'text', 'Club website URL'],"
new = "      ['config.club_website_url', 'text', 'Club website URL (Curling Club Manager)'],"
assert old in s, 'h not found'
s = s.replace(old, new)
open(p, 'w', encoding='utf-8').write(s)

print('done')
