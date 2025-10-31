#include <simplelog.h>
#include <ffwr_render.h>
#include <stdio.h>
#include <stdlib.h>
FFWR_INPUT_ST mfcinfo;
int main(int argc, char *argv[]) {
	snprintf(mfcinfo.name, sizeof(mfcinfo.name), "%s", 
		"tcp://127.0.0.1:12345");
	ffwr_create_demux(&mfcinfo);	
	while(1) {
		spl_sleep(1);
	}
	return 0;
}