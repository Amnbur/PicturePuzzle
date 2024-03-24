#include "D:\programs\code\SDL2-2.28.5\x86_64-w64-mingw32\include\SDL2\SDL.h"

#undef main
int main(int, char**)
{
    SDL_Window* win = SDL_CreateWindow("yoooomeng", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Surface* surface = SDL_LoadBMP("yoo.bmp");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    bool quit = false;
    SDL_Event ev;
    SDL_Rect rect = { 0,0,800,600 };
    int sx = 0, sy = 0;
    while (!quit)
    {
        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                sx = ev.button.x+rect.x ;
                sy = ev.button.y+rect.y;
                break;
            case SDL_MOUSEMOTION:
                if (ev.motion.state & SDL_BUTTON_LMASK)
                {
                    rect.x = ev.motion.x - sx ;
                    rect.y = ev.motion.y - sy;
                }
                break;
            case SDL_MOUSEWHEEL:
                if (ev.wheel.y > 0)
                {
                    rect.h *= 1.1;
                    rect.w *= 1.1;
                }
                if (ev.wheel.y < 0)
                {
                    rect.w /= 1.1;
                    rect.h /= 1.1;
                }
                break;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    return 0;
}