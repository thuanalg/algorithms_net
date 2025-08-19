#export PKG_CONFIG_LIBDIR=/home/thuannt/x/FFmpeg/zffmpeg/pkgconfig
INC_LIB_PATH=-I/usr/local/include -L/home/thuannt/x/FFmpeg/zffmpeg 
INC_LIB_PATH+= -lavformat -lavcodec -lavdevice -lavfilter -lavutil 
INC_LIB_PATH+= -lswscale -lswresample -lpthread

all:
	gcc -g -o main main.c ${INC_LIB_PATH}
clean:
	rm -rf main

#export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:"/home/thuannt/x/FFmpeg/zffmpeg"