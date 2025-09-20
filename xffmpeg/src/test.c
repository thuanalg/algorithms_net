#include "FFWrapper.h"
#include <simplelog.h>
int
main()
{
	FFWR_CODEC *lst = 0;
	FFWR_DEVICE *devs = 0;
	FFWR_DEMUXER_FMT *fmts = 0;
	FFWR_MUXER_FMT *muxers = 0;
	int count = 0;
	int ret = 0;
	char cfgpath[1024];
	SPL_INPUT_ARG input = {0};
	snprintf(cfgpath, 1024, "D:/x/algorithms_net/xffmpeg/ffmpeg/x64/z.cfg");
	snprintf(input.folder, SPL_PATH_FOLDER, "%s", cfgpath);
	ret = spl_init_log_ext(&input);
#if 0
	ffwr_all_codecs(&lst, &count);
	ffwr_clear_all_codecs(&lst, count);
	ffwr_all_devices(&devs, &count);
	ffwr_clear_all_devices(&devs, count);
//#endif
	ffwr_all_demuxers(&fmts, &count);
	ffwr_clear_all_demuxers(&fmts, count);
//#endif
	ffwr_all_muxers(&muxers, &count);
	ffwr_clear_all_muxers(&muxers, count);
#endif
	ffwr_devices_by_name(&devs, &count, "dshow");
	spl_finish_log();
	return 0;
}

