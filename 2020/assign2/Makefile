# CS110 Assignment 2 Makefile
CC = gcc
PROG =  diskimageaccess
# DIFF = diff --color=always -u
DIFF = qoder -d

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    # macOS
    OPENSSL_PREFIX := $(shell brew --prefix openssl)
    CFLAGS += -I$(OPENSSL_PREFIX)/include
    LDFLAGS += -L$(OPENSSL_PREFIX)/lib
else ifeq ($(UNAME_S),Linux)
    # Linux 使用系统 OpenSSL
    CFLAGS +=
    LDFLAGS +=
endif

LIB_SRC  = diskimg.c inode.c unixfilesystem.c directory.c pathname.c  chksumfile.c file.c 
DEPS = -MMD -MF $(@:.o=.d)
WARNINGS = -fstack-protector -Wall -W -Wcast-qual -Wwrite-strings -Wextra

CFLAGS += -g $(WARNINGS) $(DEPS) -std=gnu99 -DDEBUG

LIB_OBJ = $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(LIB_SRC)))
LIB_DEP = $(patsubst %.o,%.d,$(LIB_OBJ))
LIB = v6fslib.a 

PROG_SRC = diskimageaccess.c 
PROG_OBJ = $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(PROG_SRC)))
PROG_DEP = $(patsubst %.o,%.d,$(PROG_OBJ))

TMP_PATH := /usr/bin:$(PATH)
export PATH = $(TMP_PATH)

LIBS += -lssl -lcrypto

all: $(PROG)


$(PROG): $(PROG_OBJ) $(LIB)
	$(CC) $(LDFLAGS) $(PROG_OBJ) $(LIB) $(LIBS) -o $@

$(LIB): $(LIB_OBJ)
	rm -f $@
	ar r $@ $^
	ranlib $@

TESTS = basic depthFile dirFnameSize

$(TESTS): $(PROG)
	./$(PROG) -ip slink/testdisks/$@DiskImage > /tmp/output 2>&1
	$(DIFF) /tmp/output slink/testdisks/$@DiskImage.gold

compile_commands.json:
	compiledb make -Bn

clean::
	rm -f $(PROG) $(PROG_OBJ) $(PROG_DEP)
	rm -f $(LIB) $(LIB_DEP) $(LIB_OBJ)

.PHONY: all clean compile_commands.json $(TESTS)
-include $(LIB_DEP) $(PROG_DEP)
