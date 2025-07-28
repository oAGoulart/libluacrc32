#include <stdint.h>
#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define NUM_METHODS 6

extern const uint32_t crc32lt[4][256];

static const char* options_[NUM_METHODS + 1] = {
  "ISO-HDLC", "ISCSI", "BZIP2", "JAMCRC", "MPEG-2", "CD-ROM-EDC", NULL
};

static const uint8_t index_[NUM_METHODS] = {
  0, 1, 2, 0, 2, 3
};

static uint8_t
has_sse42_(void)
{
  static uint8_t sse42 = 0xFF;
  __asm__("cmpb $0xFF, %0\n\t"
          "jnz 2f\n\t"
          "movl $1, %%eax\n\t"
          "cpuid\n\t"
          "andl $0x100000, %%ecx\n\t"
          "jz 1f\n\t"
          "movb $1, %0\n\t"
          "jmp 2f\n"
          "1:\n\t"
          "xorb %0, %0\n"
          "2:\n\t"
          : "=d" (sse42)
          : "0" (sse42)
          : "eax", "ebx", "ecx");
  return sse42;
}

#ifdef __amd64__
#define CRCDST_TYPE uint64_t
#else
#define CRCDST_TYPE uint32_t
#endif
static __inline__ uint32_t
crc32sse42_(const uint8_t* data, const size_t l)
{
  uint8_t remain = l % sizeof(CRCDST_TYPE);
  CRCDST_TYPE crc = UINT32_MAX;
  size_t i = 0;
  for(; i < l; i += sizeof(CRCDST_TYPE))
  {
    CRCDST_TYPE* p = (CRCDST_TYPE*)&data[i];
    __asm__("crc32 %1, %0\n\t" : "+r" (crc) : "r" (*p));
  }
  for(i = 0; i < remain; i++)
  {
    uint8_t* p = (uint8_t*)&data[i];
    __asm__("crc32b %1, %0\n\t" : "+r" (crc) : "r" (*p));
  }
  return ~(uint32_t)crc;
}

static int
l_calculate_(lua_State* L)
{
  size_t l;
  const uint8_t* data = (uint8_t*)luaL_checklstring(L, 1, &l);
  uint8_t method = (uint8_t)luaL_checkoption(L, 2, "ISO-HDLC", options_);
  if (method == 1 && has_sse42_())
  {
    lua_pushinteger(L, crc32sse42_(data, l));
    return 1;
  }

	const uint32_t* lookup = crc32lt[index_[method]];
	uint32_t crc = 0;
  uint8_t invert = 0;
  __asm__("cmpb $4, %%al\n\t"
          "ja 2f\n\t"
          "movl $0xFFFFFFFF, %1\n\t"
          "cmpb $3, %%al\n\t"
          "jb 1f\n\t"
          "jmp 2f\n"
          "1:\n\t"
          "movb $1, %2\n\t"
          "2:\n"
          : "+a" (method), "=r" (crc), "=r" (invert)
          : "1" (crc), "2" (invert));

  const uint8_t reflect = method & 1 || !method;
  size_t i = 0;
  for (; i < l; i++)
  {
    if (reflect)
    {
      crc = lookup[(uint8_t)crc ^ data[i]] ^ (crc >> 8);
    }
    else
    {
      crc = lookup[(uint8_t)(crc >> 24) ^ data[i]] ^ (crc << 8);
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
