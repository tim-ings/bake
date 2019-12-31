default: build

build:
	gcc -g -fsanitize=address -std=c99 -Werror -Wall -pedantic src/*.c -lcurl -o bake
