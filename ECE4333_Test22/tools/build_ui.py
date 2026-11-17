from pathlib import Path
import gzip
p=Path(__file__).resolve().parent.parent/'ESP32_Assignment4'
s=(p/'TrafficPage.h').read_text(encoding='utf-8').split('R"HTML(',1)[1].rsplit(')HTML"',1)[0]
b=gzip.compress(s.encode(),mtime=0)
(p/'TrafficPageGzip.h').write_text('#pragma once\n#include <stdint.h>\n// Generated from TrafficPage.h.\nstatic const uint8_t TRAFFIC_PAGE_GZ[] PROGMEM = {\n'+''.join(' '+','.join(f'0x{x:02x}' for x in b[i:i+16])+',\n' for i in range(0,len(b),16))+'};\n')
print('Generated and verified',len(b),'gzip bytes')
assert gzip.decompress(b).decode()==s
