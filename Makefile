main: src/main.c
	gcc -Wall -Wextra -pedantic-errors  -std=c11  -g -o main -I ./lib/log.c/src src/main.c 
