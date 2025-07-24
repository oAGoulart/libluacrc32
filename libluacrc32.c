#include <stdint.h>
#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define NUM_METHODS 6

extern const uint32_t crc32lt[2][256];

static const char* options_[NUM_METHODS + 1] = {
  "ISO-HDLC", "ISCSI", "BZIP2", "JAMCRC", "MPEG-2", "CD-ROM-EDC", NULL
};

enum options_e {
  ISO_HDLC, ISCSI, BZIP2, JAMCRC, MPEG_2, CD_ROM_EDC
};

static int
l_calculate_(lua_State* L)
{
  size_t l;
  const char* data = luaL_checklstring(L, 1, &l);
  uint8_t method = (uint8_t)luaL_checkoption(L, 2, "ISO-HDLC", options_);

	const uint32_t* lookup = crc32lt[method];
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
	uint32_t crc;
  uint8_t invert;
  __asm__("cmpb $4, %%al\n\t"
          "ja 2f\n\t"
          "movl $0xFFFFFFFF, %1\n\t"
          "cmpb $3, %%al\n\t"
          "jb 1f\n\t"
          "jmp 3f\n"
          "1:\n\t"
          "movb $1, %2\n\t"
          "jmp 4f\n"
          "2:\n\t"
          "xorl %1, %1\n"
          "3:\n\t"
          "xorb %2, %2\n"
          "4:"
          : "+a" (method), "=r" (crc), "=r" (invert)
          : "1" (crc), "2" (invert)
          : "cc");
#pragma GCC diagnostic pop

  size_t i = 0;
	for (; i < l; i++)
  {
    switch (method)
    {
    case ISO_HDLC:
    case ISCSI:
    case JAMCRC:
    case CD_ROM_EDC:
      crc = lookup[(uint8_t)crc ^ data[i]] ^ (crc >> 8);
      break;
    default:
      crc = lookup[(uint8_t)(crc >> 24) ^ data[i]] ^ (crc << 8);
      break;
    }
  }
  if (invert)
  {
    crc ^= 0xFFFFFFFF;
  }
  lua_pushinteger(L, crc);
  return 1;
}

static const struct luaL_Reg crc32[] = {
  { "calculate", l_calculate_ }, 
  { NULL, NULL }
};

int
luaopen_libluacrc32 (lua_State* L)
{
  luaL_newlib(L, crc32);
  return 1;
}
