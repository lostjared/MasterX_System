### Created by MXVM

LFLAGS="-lSDL2 -lSDL2_ttf"
all:
	LDFLAGS=$(LFLAGS) mxvmc  "BlocksMod.mxvm" --path "/usr/local/lib" --object-path "." --action compile  --target linux
run:
	mxvmc --path "/usr/local/lib" object_path "." "BlocksMod.mxvm"
