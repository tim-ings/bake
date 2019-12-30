default: build

build:
	gcc -std=c99 -Werror -Wall -pedantic -lm src/*.c -o bake
