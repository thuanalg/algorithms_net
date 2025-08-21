all:
	#gcc -g main.c -o main  -lglut -lGLU -lGL -lwayland-client -lwayland-egl -lEGL -lGLESv2 `pkg-config --cflags --libs gtk+-3.0` -lepoxy
	gcc -g main.c -o main `pkg-config --cflags --libs gtk4` -lvulkan
clean:
	rm -r main
