p='external_components/web_editor/web_editor.cpp'
s=open(p).read()
old='  if (!start_web_httpd(this)) {'
new='  ESP_LOGW(TAG, "SKIPPING httpd_start for debug");\n  if (false) {'
assert old in s, 'pattern not found'
s=s.replace(old,new)
open(p,'w').write(s)
print('done')
