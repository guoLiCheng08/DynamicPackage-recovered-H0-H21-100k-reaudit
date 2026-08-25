import struct
from pathlib import Path
b=Path('input/DynamicPackage.elf').read_bytes()
for a in (0x11ec8,0x11ed0):
 w=struct.unpack_from('<Q',b,a)[0]
 v=struct.unpack_from('<d',b,a)[0]
 print(hex(a),hex(w),repr(v))
