#include "../../include/main.h"

#define moveX (game->gameScreenSize.x / game->nbX) / 8
#define moveY (game->gameScreenSize.y / game->nbY) / 8

int animations(gameS *game, int animationsArray[][4], Uint32 secondsAnimations[])
{
    for (int i = 0; i < 16; i++) {
        if (animationsArray[i][0] != -1) {
            animationHandling(game, animationsArray[i], secondsAnimations, animationsArray[i][0], i, animationsArray[i][3]);
        }
    }
}

int animationHandling(gameS *game, int animationsArrayElement[3], Uint32 secondsAnimations[], int playerNumber, int elementNumber, int speacialCommand)
{
    Uint32 newSeconds = SDL_GetTicks();
    if (animationsArrayElement[1] >= 0 && animationsArrayElement[1] <= 4) {
        if (newSeconds > secondsAnimations[elementNumber] + (game->players[playerNumber]->pStats->playerSpeed / 7) && animationsArrayElement[2] < 8) {
            game->players[playerNumber]->playerSpPos.x += 67;
            secondsAnimations[elementNumber] = newSeconds;
            animationsArrayElement[2]++;
            if (animationsArrayElement[1] == 1) {
                game->players[playerNumber]->playerRextPos.y -= moveY;
            }
            else if (animationsArrayElement[1] == 2) {
                game->players[playerNumber]->playerRextPos.y += moveY;
            }
            else if (animationsArrayElement[1] == 3) {
                game->players[playerNumber]->playerRextPos.x += moveX;
            }
            else if (animationsArrayElement[1] == 4) {
                game->players[playerNumber]->playerRextPos.x -= moveX;
            }
        }
        else if (animationsArrayElement[2] >= 8) {
            animationsArrayElement[0] = -1;
            game->players[playerNumber]->playerSpPos.x = 0;
        }
    }
    else if (animationsArrayElement[1] == 5) {
        if (newSeconds > secondsAnimations[elementNumber] + 50 && animationsArrayElement[2] < 64) {
            animationsArrayElement[2]++;
            secondsAnimations[elementNumber] = newSeconds;
            if (animationsArrayElement[2] == 8 || animationsArrayElement[2] == 16 || animationsArrayElement[2] == 24 || animationsArrayElement[2] == 32 || animationsArrayElement[2] == 40 || animationsArrayElement[2] == 48 || animationsArrayElement[2] == 56) {
                game->players[playerNumber]->pBombs->bombSpRectPos[speacialCommand].x = 0;
            }
            else
            {
                game->players[playerNumber]->pBombs->bombSpRectPos[speacialCommand].x += 84;
            }
        }
        else if (animationsArrayElement[2] >= 64) {
            animationsArrayElement[0] = -1;
            game->players[playerNumber]->pBombs->bombSpRectPos[speacialCommand].x = 0;
        }
    }
    else if (animationsArrayElement[1] == 6) {
        if (newSeconds > secondsAnimations[elementNumber] + 50 && animationsArrayElement[2] < 16) {
            animationsArrayElement[2]++;
            secondsAnimations[elementNumber] = newSeconds;
        }
        else if (animationsArrayElement[2] >= 16) {
            animationsArrayElement[0] = -1;
            game->players[playerNumber]->pBombs->exploded[speacialCommand] = 0;
        }
    }
}

SDL_Rect animationMenu(SDL_Rect spritePosition, Uint32 *seconds, int maxSpritePosition, int incrementSpritePosition)
{
    Uint32 newSeconds = SDL_GetTicks();
    if (newSeconds > *seconds + 75) {
        *seconds = newSeconds;
        if (spritePosition.y >= maxSpritePosition) {
            spritePosition.y = 0;
        } else {
            spritePosition.y += incrementSpritePosition;
        }
    }
    return spritePosition;
}

SDL_Rect animationMenuError(SDL_Rect spritePosition, Uint32 *seconds, int maxSpritePosition, int incrementSpritePosition)
{
    Uint32 newSeconds = SDL_GetTicks();
    if (newSeconds > *seconds + 75) {
        *seconds = newSeconds;
        if (spritePosition.y <= maxSpritePosition) {
            spritePosition.y += incrementSpritePosition;
        }
    }
    return spritePosition;
}

int bonusAnimations(gameS *game, Uint32 *seconds)
{
    Uint32 newSeconds = SDL_GetTicks();
    if (newSeconds > *seconds + 50) {
        *seconds = newSeconds;
        for (int i = 0; i < 16; i++) {
            if (game->bonus->bonusesDT[i]->bonusType != -1) {
                if (game->bonus->bonusesDT[i]->bonusSpRectPosition.y >= 320) {
                    game->bonus->bonusesDT[i]->bonusSpRectPosition.y = 0;
                } else {
                    game->bonus->bonusesDT[i]->bonusSpRectPosition.y += 64;
                }
            }
        }
    }
}