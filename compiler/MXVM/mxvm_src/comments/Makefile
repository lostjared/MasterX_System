CC=cc
MXVMC=mxvmc
LIB_PATH=/usr/local/lib
OBJ_PATH=.
IO_MODULE_PATH=/usr/local/lib/modules/io
IO_LIB=-lmxvm_io_static

SRC_MXVM=ReadFile.mxvm Comments.mxvm
OBJ_S=$(SRC_MXVM:.mxvm=.s)
OBJ_O=$(SRC_MXVM:.mxvm=.o)

TARGET=strip-py-comments

.PHONY: all clean
all:
	../../build/src/vm/mxvmc Comments.mxvm --path  $(LIB_PATH) --object-path $(OBJ_PATH) --action translate
	$(CC) $(OBJ_S) -o $(TARGET)  -L$(IO_MODULE_PATH)  $(IO_LIB)
clean:
	rm -f $(OBJ_S) $(OBJ_O) $(TARGET)
