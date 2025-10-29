#include <simplelog.h>
#include <ffwr_render.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

int
ffwr_hello() {
	int ret = 0;
	ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) ;
	spllog(1, "---");
	return ret;
}

int
ffwr_init(FFWR_InitFlags flags) {
	int ret  = 0;
    spllog(1, "SDL compiled version: %d.%d.%d\n",
        SDL_MAJOR_VERSION,
        SDL_MINOR_VERSION,
        SDL_PATCHLEVEL);	
	do {
		ret = SDL_Init(flags);
		if(ret) {
			spllog(4, "SDL_Init Error: %s\n", SDL_GetError());
			break;
		}		
	} while(0);

	return ret;
}