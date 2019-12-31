default: build

build:
	gcc -std=c99 -Werror -Wall -pedantic -lcurl src/*.c -o bake
