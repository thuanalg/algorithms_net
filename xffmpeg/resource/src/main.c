#include <simplelog.h>
#include <ffwr_render.h>
#include <stdio.h>
#include <stdlib.h>
FFWR_INPUT_ST mfcinfo;
int main(int argc, char *argv[]) {
	int ret = 0;
	char cfgpath[1024] = {0};
	SPL_INPUT_ARG input = {0};
	FFWR_DEMUX_OBJS test_demux = {0};
	FFWR_DEMUX_OBJS *obj = &test_demux;
	FFWR_INPUT_ST *pinput = 0;
	snprintf(cfgpath, 1024, "z.cfg");
	snprintf(input.folder, SPL_PATH_FOLDER, "%s", cfgpath);
	snprintf(input.id_name, 100, "x");
	ret = spl_init_log_ext(&input);
/*	
	ffwr_init(FFWR_INIT_AUDIO | FFWR_INIT_VIDEO);
	
	snprintf(mfcinfo.name, sizeof(mfcinfo.name), "%s", 
		"tcp://127.0.0.1:12345");
	ffwr_create_demux(&mfcinfo);
*/
	// FFWR_PIXELFORMAT_IYUV, FFWR_TEXTUREACCESS_STREAMING
	obj->render_objects.w = 640;
	obj->render_objects.h = 480;
	obj->render_objects.format = FFWR_PIXELFORMAT_IYUV;
	obj->render_objects.access = FFWR_TEXTUREACCESS_STREAMING;
	obj->render_objects.ren_flags =  (FFWR_INIT_AUDIO | FFWR_INIT_VIDEO);
	obj->buffer.vbuf_size = obj->buffer.abuf_size = 12000000;
	pinput = &(obj->input);
	
	snprintf(pinput->name, 
		sizeof(pinput->name), 
		"%s", "/home/thuannt/x/FFmpeg/zoutput.mp4");
	ret = ffwr_open_demux_objects(obj);
	while(1) {
		spl_sleep(1);
	}
	
	spl_finish_log();
	return 0;
}