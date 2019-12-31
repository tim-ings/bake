default: build

build:
	gcc -std=c99 -Werror -Wall -pedantic src/*.c -lcurl -o bake
