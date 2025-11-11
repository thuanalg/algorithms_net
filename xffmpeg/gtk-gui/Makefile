target=gtk-window
libs+= -lsimplelog
all:
	gcc -g -o ${target} main.c `pkg-config --cflags --libs gtk+-3.0` ${libs}
clean:
	rm ${target}
