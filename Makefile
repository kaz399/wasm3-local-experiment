SRCS:=$(wildcard ./wasm3/source/*.c)
SRCS+=$(wildcard ./local-api/*.c)
SRCS+=minimal-main.c

OBJS:=$(patsubst %.c,%.o,$(SRCS))

CC:=clang
CLANG_TARGET:=$(shell clang -print-target-triple)

CFLAGS:=-I./wasm3/source/
CFLAGS+=-I./local-api/
CFLAGS+=--std=c11
CFLAGS+=-DLINK_KUSO_CODE
CFLAGS+=-DDEBUG
CFLAGS+=-Dd_m3LogParse=1
CFLAGS+=-Dd_m3LogModule=1
CFLAGS+=-Dd_m3LogCompile=0
CFLAGS+=-Dd_m3LogWasmStack=0
CFLAGS+=-Dd_m3LogEmit=0
CFLAGS+=-Dd_m3LogCodePages=0
CFLAGS+=-Dd_m3LogRuntime=0
CFLAGS+=-Dd_m3LogNativeStack=0

LDFLAGS:=-fuse-ld=lld

ifneq ($(findstring msvc,$(CLANG_TARGET)),msvc)
LDFLAGS+=-lm
endif

TARGET:=wasm3_with_kuso_code


.PHONY: clean


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS)

testwasm:
	$(MAKE) -C ./wasm-code/

testwasm-%:
	$(MAKE) -C ./wasm-code/ $*

test: $(TARGET) testwasm
	./$(TARGET) $(TESTOPT) ./wasm-code/test_kuso_code.wasm

clean: testwasm-clean
	rm -f $(OBJS)
	rm -f $(TARGET)

compiledb: clean
	compiledb $(MAKE) $(TARGET) testwasm

