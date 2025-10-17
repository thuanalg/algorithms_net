#include <SDL.h>
#include <stdio.h>
#include <simplelog.h>

#ifndef UNIX_LINUX
#include <windows.h>
HWND gb_sdlWindow = 0;
#else
#endif 

#ifndef UNIX_LINUX
int WinMain
#else
int main
#endif
(int argc, char *argv[]) {	
	int ret = 0;
	SDL_SysWMinfo info = {0};
	SDL_Window *win = 0;
	int running = 1;
	SDL_Event e = {0};
	
	char cfgpath[1024] = {0};
	SPL_INPUT_ARG input = {0};
	snprintf(cfgpath, 1024, "z.cfg");
	snprintf(input.folder, SPL_PATH_FOLDER, "%s", cfgpath);
	snprintf(input.id_name, 100, "vk_window");
	ret = spl_init_log_ext(&input);
	if(ret) {
		exit(1);
	}
	
	ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret) {
        spllog(4, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    win = SDL_CreateWindow(
        "SDL2 Window",            // title
        SDL_WINDOWPOS_CENTERED,   // x
        SDL_WINDOWPOS_CENTERED,   // y
        800,                      // width
        600,                      // height
        SDL_WINDOW_SHOWN           // flags
    );
	SDL_GetWindowWMInfo(sdl_window, &info);
	gb_sdlWindow = info.info.win.window;
    if (!win) {
        spllog(4, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) {
        spllog(4, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // main loop
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
		spllog(1, "--");
        // fill background
        SDL_SetRenderDrawColor(ren, 0, 128, 255, 255); // blue
        SDL_RenderClear(ren);

        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
	spl_finish_log();
    return 0;
}
