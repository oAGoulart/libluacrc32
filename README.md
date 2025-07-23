# libluacrc32

Lua library to generate 32-bits cyclic redundancy checks.

**Methods available:**
- [x] ISO-HDLC
- [x] ISCSI
- [ ] JAMCRC
- [ ] MPEG-2
- [ ] BZIP2
- [ ] CD-ROM-EDC

## Usage

Compile with gcc or use `build.sh` script.

```lua
local crc32 = require("libluacrc32")

print(crc32.calculate("wrappem"))
```

Further reading: [Catalogue of parametrised CRC algorithms with 17 or more bits](https://reveng.sourceforge.io/crc-catalogue/17plus.htm).
