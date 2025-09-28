#include "FFWrapper.h"
#include <simplelog.h>
#if 0
int
main(int argc, char *argv[])
{
	FFWR_DEVICE *devs = 0;
	int count = 0;
	int ret = 0;
	char cfgpath[1024];
	SPL_INPUT_ARG input = {0};
	snprintf(cfgpath, 1024, "z.cfg");
	snprintf(input.folder, SPL_PATH_FOLDER, "%s", cfgpath);
	ret = spl_init_log_ext(&input);
	do {
		ffwr_devices_by_name(&devs, &count, "dshow");
		/*Open Video context.*/
		/*Open Audio context*/
		ret = ffwr_open_devices(devs, count, "dshow");
		if (ret) {
			break;
		}
		ret = ffwr_open_output(devs, count);
		if (ret) {
			break;
		}
		ret = ffwr_devices_operate(devs, count);
		if (ret) {
			break;
		}
		ffwr_close_devices(devs, count);
	} while (0);
	spl_finish_log();
	return ret;
}

#else 

int
main(int argc, char *argv[])
{
	FFWR_FMT_DEVICES inp;
	int count = 0;
	int ret = 0;
	int rs = 0;
	char cfgpath[1024];
	SPL_INPUT_ARG input = {0};
	snprintf(cfgpath, 1024, "z.cfg");
	snprintf(input.folder, SPL_PATH_FOLDER, "%s", cfgpath);
	ret = spl_init_log_ext(&input);
	memset(&inp, 0, sizeof(inp));

	do {
		

		snprintf(inp.type, 64, "%s", "dshow");
		snprintf(inp.name, 512, "%s", 
			"video=Integrated Webcam:audio="
			"Microphone (2- Realtek(R) Audio)");

		ffwr_open_in_fmt(&inp);

	} while (0);
	spl_finish_log();
	return ret;
}
#endif