#include "FFWrapper.h"
#include <simplelog.h>
int
main(int argc, char *argv[])
{
	FFWR_DEVICE *devs = 0;
	int count = 0;
	int ret = 0;
	char cfgpath[1024];
	SPL_INPUT_ARG input = {0};
	snprintf(cfgpath, 1024, "D:/x/algorithms_net/xffmpeg/ffmpeg/x64/z.cfg");
	snprintf(input.folder, SPL_PATH_FOLDER, "%s", cfgpath);
	ret = spl_init_log_ext(&input);

	ffwr_devices_by_name(&devs, &count, "dshow");
	ffwr_open_devices(devs, count, "dshow");
	
#if 0
	/*Open Video context.*/
	/*Open Audio context*/
	AVFormatContext *video_ctx = NULL;
	AVFormatContext *audio_ctx = NULL;
	AVInputFormat *iformat = av_find_input_format("dshow");

	// Open video device
	if (avformat_open_input(&video_ctx, video_device, iformat, NULL) < 0) {
		fprintf(stderr, "Cannot open video device\n");
		return -1;
	}

	// Open audio device
	if (avformat_open_input(&audio_ctx, audio_device, iformat, NULL) < 0) {
		fprintf(stderr, "Cannot open audio device\n");
		return -1;
	}
#endif

	spl_finish_log();
	return ret;
}
