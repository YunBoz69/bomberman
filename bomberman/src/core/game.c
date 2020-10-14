#include "../../include/main.h"

//fonts of displayed text
void switchText(gameS *game, char *message, int size, SDL_Rect rect, SDL_Surface *surface, SDL_Texture *texture)
{
    //Define color, text and font and setting it with TTF_RenderText_Blended
    SDL_Color color = {30, 30, 30};
    game->menu->font = TTF_OpenFont("./fonts/font.ttf", size);
    surface = TTF_RenderText_Blended(game->menu->font, message, color);
    if (!surface) {
        printf("Error : SDL Surface modification menu: %s \n", SDL_GetError());
        gameDestroy(game);
    } else {
          //Creating a texture from the defined surface
        texture = SDL_CreateTextureFromSurface(game->renderer, surface);
        if (!texture) {
            printf("Error : SDL Texture modification menu: %s \n", SDL_GetError());
            gameDestroy(game);
        } else {
              //Query all the defind texture
            SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
        }
    }
}

//Function to end the game (total exit from it)
void gameDestroy(gameS *game)
{
    //main condition
    if (game) {

      /*
      Overall review of freeing :
      when we click exit button from any menu other than the menu = 3 (home)
      we free memory of any component of the game then gets redirected to home menu where the game can be destroyed
      */

      //if we are inside a menu
      //Free the inputFont if exists
      //Free the inputTexture if exists
      //Then free the menu
        if (game->menu) {
            if (game->menu->inputFont) {
                SDL_FreeSurface(game->menu->inputFont);
            }
            if (game->menu->inputTexture) {
                SDL_DestroyTexture(game->menu->inputTexture);
            }
            free(game->menu);
        }

        //if there's a bomb
        //Free the bomb Texture if exists
        //Free the flameTexture if exists
        //Then free the bomb
        if (game->bombe) {
            if (game->bombe->bombTex) {
                SDL_DestroyTexture(game->bombe->bombTex);
            }
            if (game->bombe->expTexture) {
                for (int i = 0; i < 5; i++) {
                    if (game->bombe->expTexture[i])
                    {
                        SDL_DestroyTexture(game->bombe->expTexture[i]);
                    }
                }
            }
            free(game->bombe);
        }
        //if there's a player
        //Free the player Texture if exists
        //Free the playerBombes if exists
        //Then free the player
        if (game->players) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 6; j++) {
                    if (game->players[i]->playerTexture[j])
                    {
                        SDL_DestroyTexture(game->players[i]->playerTexture[j]);
                    }
                    if (game->players[i]->pBombs)
                    {
                        free(game->players[i]->pBombs);
                    }
                }
                free(game->players[i]);
            }
        }
        //if there's game informations
        //Free the port if exists
        //Free the ipAdress if exists
        //Then free the game informations
        if (game->infos) {
            if (game->infos->port) {
                free(game->infos->port);
            }
            if (game->infos->ipAdress) {
                free(game->infos->ipAdress);
            }
            free(game->infos);
        }
        //Free the game sockets if exists
        if (game->serverSockets) {
            free(game->serverSockets);
        }
        //free the map if exists
        if (game->map) {
            for (int i = 0; i < game->nbY; i++) {
                for (int j = 0; j < game->nbX; j++) {
                    free(game->map[i][j]);
                }
            }
        }
        //free the game renderer if exists
        if (game->renderer) {
            SDL_DestroyRenderer(game->renderer);
        }
        //free the game window if exists
        if (game->window) {
            SDL_DestroyWindow(game->window);
        }
        //To clean all initilised subsystems and then free the game
        SDL_Quit();
        free(game);
    }
}

//Same as first but used for fixed size texts like ip adresses and ports
SDL_Texture *changeTextSize(gameS *game, char *message)
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Color color = {30, 30, 30};
    surface = TTF_RenderText_Blended(game->menu->font, message, color);
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

//Same as the last one, used in init.c
SDL_Texture *changeTextSize_12(gameS *game, char *message)
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Color color = {30, 30, 30};
    surface = TTF_RenderText_Blended(game->menu->font_12, message, color);
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
