SRCS:=test_kuso_code.c
OBJS:=$(patsubst %.c,%.o,$(SRCS))

CFLAGS:=
CFLAGS+=--target=wasm32
CFLAGS+=-I../wasm3/source/
CFLAGS+=-I../local-api/

LDFLAGS:=
LDFLAGS+=--no-entry
LDFLAGS+=--export-all
LDFLAGS+=--allow-undefined

TARGET:=test_kuso_code.wasm

CC=clang
LD=wasm-ld

.PHONY: clean

$(TARGET): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
