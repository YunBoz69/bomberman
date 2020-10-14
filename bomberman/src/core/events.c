#include "../../include/main.h"

int eventMenu(gameS *game)
{
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return 0;
        }
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
                return 0;
                break;

            case SDLK_UP:
                game->menu->menuSelected = 1;
                break;

            case SDLK_DOWN:
                game->menu->menuSelected = 2;
                break;

            case SDLK_RETURN:
                if (game->menu->menuSelected == 1) {
                    game->menu->menuSelected = 3;
                    return 3;
                }
                if (game->menu->menuSelected == 2) {
                    game->menu->menuSelected = 3;
                    return 4;
                }
                break;

            default:
                printf("Key pressed not recognised : %d\n", e.key.keysym.sym);
                break;
            }
        }
    }
    return 1;
}

int selectMapEvent(gameS *game)
{
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return 0;
        }
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
                return 0;
                break;

            case SDLK_RIGHT:
                if (game->menu->menuSelected < 7) {
                    game->menu->menuSelected++;
                }
                break;

            case SDLK_LEFT:
                if (game->menu->menuSelected > 5) {
                    game->menu->menuSelected--;
                }
                break;

            case SDLK_RETURN:
                if (game->menu->menuSelected == 5) {
                    game->mapFeatures->mapId = 0;
                    return 0;
                }
                if (game->menu->menuSelected == 6) {
                    game->mapFeatures->mapId = 1;
                    return 0;
                }
                if (game->menu->menuSelected == 7) {
                    game->mapFeatures->mapId = 2;
                    return 0;
                }
                break;

            default:
                printf("Key pressed not recognised : %d\n", e.key.keysym.sym);
                break;
            }
        }
    }
    return 1;
}

int inputPortEvent(char *port, gameS *game, int size)
{
    SDL_Event e;
    if (SDL_WaitEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return 0;
        }
        else if (e.type == SDL_TEXTINPUT) {
            //port = e.text.text;
            if (((e.text.text[0] <= 57 && e.text.text[0] >= 48) || e.text.text[0] == 46) && strlen(port) < size) {
                strcat(port, e.text.text);
                printf("%s\n", port);
            }
        }
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
            case SDLK_BACKSPACE:
                if (strlen(port) > 1) {
                    port[strlen(port) - 1] = '\0';
                    printf("%s\n", port);
                }
                break;

            case SDLK_RETURN:
                return 0;
                break;
            }
        }
    }
    return 1;
}

SDL_Texture *textChange(gameS *game, char *message, int size)
{
    TTF_Font *font = TTF_OpenFont("./fonts/font.ttf", size);
    SDL_Surface *surface;
    SDL_Texture *texture;
    if (!font) {
        printf("Error : SDL Font creation : %s \n", TTF_GetError());
    }
    SDL_Color color = {30, 30, 30};

    surface = TTF_RenderText_Blended(font, message, color);
    if (!surface) {
        printf("Error : SDL Surface creation menu: %s \n", SDL_GetError());
        gameDestroy(game);
    } else {
        texture = SDL_CreateTextureFromSurface(game->renderer, surface);
        if (!texture) {
            printf("Error : SDL Texture creation menu: %s \n", SDL_GetError());
            gameDestroy(game);
        }
    }
    return texture;
}