#kdir build && cd build
#cmake ../ -D CMAKE_BUILD_TYPE=Debug -D WITH_OPENGL=ON -D OPENCV_GENERATE_PKGCONFIG=ON -DWITH_FFMPEG=OFF\
# -D CMAKE_INSTALL_PREFIX=/usr/local \
# -D OPENCV_GENERATE_PKGCONFIG=ON \
# -D WITH_OPENGL=ON \
# -D OPENCV_ENABLE_NONFREE=ON \
# ..
CPPFLAGS+=-g -std=c++20
target=HumanDetector
INC_LIB_PATH=-I/usr/local/include/opencv4 
INC_LIB_PATH+= -L/usr/local/lib -lopencv_video
INC_LIB_PATH+= -lopencv_gapi -lopencv_highgui -lopencv_ml -lopencv_objdetect 
INC_LIB_PATH+= -lopencv_photo -lopencv_stitching -lopencv_video 
INC_LIB_PATH+= -lopencv_calib3d -lopencv_features2d -lopencv_dnn 
INC_LIB_PATH+= -lopencv_flann -lopencv_videoio -lopencv_imgcodecs -lopencv_imgproc -lopencv_core

all:
	g++ ${CPPFLAGS} -o ${target}  main.cpp ${INC_LIB_PATH}
clean:
	rm -f ${target}
#export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:${PKG_CONFIG_PATH}
#-I/usr/local/include/opencv4 -L/usr/local/lib 
#-lopencv_gapi -lopencv_highgui -lopencv_ml -lopencv_objdetect 
#-lopencv_photo -lopencv_stitching -lopencv_video 
#-lopencv_calib3d -lopencv_features2d -lopencv_dnn 
#-lopencv_flann -lopencv_videoio -lopencv_imgcodecs -lopencv_imgproc -lopencv_core
#export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH