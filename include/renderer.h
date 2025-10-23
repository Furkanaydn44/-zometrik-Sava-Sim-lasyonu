#ifndef __RENDERER_H
#define __RENDERER_H
#ifdef __cplusplus
extern "C"{
#endif

#define WINDOW_WIDTH     1200
#define WINDOW_HEIGHT    720

void initRenderer(char *windowCaption);
SDL_Renderer *getRenderer();
SDL_Window *getWindow();
void closeRenderer();

#ifdef __cplusplus
}
#endif

#endif // __RENDERER_H

