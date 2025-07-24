# libluacrc32

Lua library to generate 32-bits cyclic redundancy checks. Currently, only one method can be built at a time.

**Methods available:**
- [x] ISO-HDLC
- [x] ISCSI
- [ ] JAMCRC
- [ ] MPEG-2
- [ ] BZIP2
- [ ] CD-ROM-EDC

## Usage

Compile with gcc or use `build.sh` script. If on Windows, use `CFLAGS` and `LFLAGS` to set include and library paths, respectively.

```sh
env CFLAGS=-IC:/Users/a_gou/scoop/apps/lua/5.4.7-2/include \
    LFLAGS=-LC:/Users/a_gou/scoop/apps/lua/5.4.7-2/bin ./build.sh
```

**NOTE:** If you want to use MSVC, you are on your own, sorry.

Then, require and call with a string to calculate its CRC. First and second arguments of `calculate` are `strings`, the latter is which method to use.

```lua
local crc32 = require("libluacrc32")

print(crc32.calculate("wrappem", "ISO-HDLC"))
```

**NOTE:** If the Lua code above fails with `module 'libluacrc32' not found` on Windows, add `package.cpath=".\\?.dll"` one line before `require` or change your `LUA_CPATH` environment variable to include `.\\?.dll`.

## Further reading
1. [Catalogue of parametrised CRC algorithms with 17 or more bits](https://reveng.sourceforge.io/crc-catalogue/17plus.htm).
