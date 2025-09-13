#export PKG_CONFIG_LIBDIR=/home/thuannt/x/FFmpeg/zffmpeg/pkgconfig:${PKG_CONFIG_LIBDIR}
INC_LIB_PATH=-I/usr/local/include -L/mingw64/bin/ -L/usr/local/bin/
INC_LIB_PATH+= -lavcodec -lavformat -lswscale -lavfilter -lavdevice -lavutil
INC_LIB_PATH+= 

all:
	gcc -g -o main main.c ${INC_LIB_PATH}
clean:
	rm -rf main

#export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:"/home/thuannt/x/FFmpeg/zffmpeg"
#git config --global user.name "Thuận Nguyễn-Thái"

