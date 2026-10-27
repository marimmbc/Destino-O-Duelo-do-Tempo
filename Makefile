destino: src/*.c
	gcc src/*.c -o destino $(shell pkg-config --cflags --libs raylib) -lm