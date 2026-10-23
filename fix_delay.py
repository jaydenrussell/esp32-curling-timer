p='external_components/web_editor/web_editor.cpp'
s=open(p).read()
old='  ESP_LOGI(TAG, "auth ready (session cookie %s, port %u)", SESSION_COOKIE, this->port_);\n  if (!start_web_httpd(this)) {'
new='  ESP_LOGI(TAG, "auth ready (session cookie %s, port %u)", SESSION_COOKIE, this->port_);\n  delay(500);\n  if (!start_web_httpd(this)) {'
assert old in s, 'pattern not found'
s=s.replace(old,new)
open(p,'w').write(s)
print('done')
