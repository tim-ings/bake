default: build

build:
	gcc -std=c99 -lm src/*.c -o bake
