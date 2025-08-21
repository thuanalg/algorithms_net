all:
	gcc -g main.c -o main  -lglut -lGLU -lGL -lwayland-client -lwayland-egl -lEGL -lGLESv2 `pkg-config --cflags --libs gtk+-3.0` -lepoxy
clean:	
	rm -r main
