default: build

build:
	gcc -std=c99 -lm *.c -o bake
