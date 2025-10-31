#include <simplelog.h>
#include <ffwr_render.h>
#include <stdio.h>
#include <stdlib.h>
FFWR_INPUT_ST mfcinfo;
int main(int argc, char *argv[]) {
	int ret = 0;
	char cfgpath[1024] = {0};
	SPL_INPUT_ARG input = {0};
	snprintf(cfgpath, 1024, "z.cfg");
	snprintf(input.folder, SPL_PATH_FOLDER, "%s", cfgpath);
	snprintf(input.id_name, 100, "tvk_wind");
	ret = spl_init_log_ext(&input);
	
	ffwr_init(FFWR_INIT_AUDIO | FFWR_INIT_VIDEO);
	
	snprintf(mfcinfo.name, sizeof(mfcinfo.name), "%s", 
		"tcp://127.0.0.1:12345");
	ffwr_create_demux(&mfcinfo);	
	while(1) {
		spl_sleep(1);
	}
	spl_finish_log();
	return 0;
}