targ='libluacrc32'

gcc -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 \
    -fPIC $CFLAGS --std=c11 -O3 -c lookup.c $targ.c

if [ "$(printenv 'OS')" = 'Windows_NT' ]; then
  gcc -shared -lm -o $targ.dll $targ.o lookup.o $LFLAGS -llua55
else
  gcc -shared -lm -o $targ.so $targ.o lookup.o $LFLAGS
fi
