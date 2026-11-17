import gzip
import re

with open('external_components/web_editor/web_ui.h', 'r') as f:
    content = f.read()

match = re.search(r'static const uint8_t WEB_UI_HTML_GZ\[\] = \{(.*?)\};', content, re.DOTALL)
if match:
    hex_str = match.group(1)
    bytes_list = [int(x.strip(), 16) for x in hex_str.split(',') if x.strip()]
    gz_data = bytes(bytes_list)
    print(f'Gzip size: {len(gz_data)} bytes')
    try:
        html = gzip.decompress(gz_data)
        print(f'Decompressed size: {len(html)} bytes')
        html_str = html.decode('utf-8')
        print(f'Has #app div: {"id=\"app\"" in html_str}')
        print(f'Has Svelte mount: {"document.getElementById(\"app\")" in html_str}')
        print(f'First 200 chars: {html_str[:200]}')
    except Exception as e:
        print(f'Decompress error: {e}')