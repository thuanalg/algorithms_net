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
	/*Open Video context.*/
	/*Open Audio context*/
	ffwr_open_devices(devs, count, "dshow");
	
	do {

	} while (0);

	ffwr_close_devices(devs, count);
	spl_finish_log();
	return ret;
}
