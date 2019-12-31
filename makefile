CC = clang
CARGS = -Werror -Wall -pedantic
DARGS = -g -fsanitize=address
IN = src/*.c
LIBS = -lcurl
OUT = bake

default: build

debug:
	$(CC) $(CARGS) $(DARGS) $(IN) $(LIBS) -o $(OUT)

build:
	$(CC) $(CARGS) $(IN) $(LIBS) -o $(OUT)
