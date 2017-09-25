CFLAGS = -O2 -Wall -Wextra -Wpedantic -std=c++11
LDFLAGS = -lSDL2 -lm -lpthread
CC = g++

CFILES = 	src/main.cpp \
			src/render.cpp \
			src/color.cpp \
			src/supersampling.cpp \
			src/thread_pool.cpp

.PHONY: mandelbrot clean

mandelbrot: $(CFILES)
	$(CC) $(CFLAGS) $(CFILES) -o mandelbrot $(LDFLAGS)

clean:
	-rm hello.bmp
	-rm mandelbrot
