if [ $# -ne 1 ]; then
  crc=1
else
  crc=$1
fi

targ='libluacrc32'

gcc -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 \
    -fPIC -DCRC_METHOD=$crc --std=c11 -g0 -O3 -c $targ.c

if [ $(echo '$OS') = 'Windows_NT' ]; then
  gcc $targ.o -o $targ.dll -lm -shared
else
  gcc $targ.o -o $targ.so -lm -shared
fi
