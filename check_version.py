import gzip, re

with open('external_components/web_editor/web_ui.h', 'rb') as f:
    content = f.read()

text = content.decode('utf-8', errors='ignore')
hex_bytes = re.findall(r'0x([0-9a-fA-F]{2})', text)
byte_data = bytes(int(b, 16) for b in hex_bytes)
decompressed = gzip.decompress(byte_data)
html = decompressed.decode('utf-8')

match = re.search(r'curling-timer-version.*?content="([^"]+)"', html)
if match:
    print('Version in HTML:', match.group(1))
else:
    print('Version meta tag not found')