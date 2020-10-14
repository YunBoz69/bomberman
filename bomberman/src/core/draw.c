#include "../../include/main.h"

#define direction game->players[k]->direction
#define bombeX game->players[k]->pBombs->caseBombPos[j].x
#define bombeY game->players[k]->pBombs->caseBombPos[j].y
#define radiusBombe game->players[k]->pStats->radBombs

void menuSdl(gameS *game, Uint32 *seconds)
{
    Uint32 otherSeconds = *seconds;
    SDL_RenderClear(game->renderer);
    if (game->menu->menuSelected <= 4) {
        SDL_RenderCopy(game->renderer, game->menu->backgroundTexture, NULL, &game->windowSize);
    } else {
        SDL_RenderCopy(game->renderer, game->menu->backgroundTexture_selectMap, NULL, &game->windowSize);
    }

    if (game->menu->menuSelected == 1) {
        game->menu->firstButtonSpritePosition = animationMenu(game->menu->firstButtonSpritePosition, seconds, 875, 125);
        game->menu->secondButtonSpritePosition.y = 0;
        SDL_RenderCopy(game->renderer, game->menu->firstButtonTexture, &game->menu->firstButtonSpritePosition, &game->menu->firstButtonPosition);
        SDL_RenderCopy(game->renderer, game->menu->secondButtonTexture, &game->menu->secondButtonSpritePosition, &game->menu->secondButtonPosition);
    }
    else if (game->menu->menuSelected == 2) {
        game->menu->secondButtonSpritePosition = animationMenu(game->menu->secondButtonSpritePosition, seconds, 875, 125);
        game->menu->firstButtonSpritePosition.y = 0;
        SDL_RenderCopy(game->renderer, game->menu->firstButtonTexture, &game->menu->firstButtonSpritePosition, &game->menu->firstButtonPosition);
        SDL_RenderCopy(game->renderer, game->menu->secondButtonTexture, &game->menu->secondButtonSpritePosition, &game->menu->secondButtonPosition);
    }
    else if (game->menu->menuSelected == 3) {
        SDL_RenderCopy(game->renderer, game->menu->selectPortTexture, NULL, &game->menu->selectPortPosition);
        SDL_RenderCopy(game->renderer, game->menu->selectInputTexture, NULL, &game->menu->selectInputPosition);
    }
    else if (game->menu->menuSelected == 4) {
        SDL_RenderCopy(game->renderer, game->menu->selectIpTexture, NULL, &game->menu->selectIpPosition);
        SDL_RenderCopy(game->renderer, game->menu->selectInputTexture, NULL, &game->menu->selectInputPosition);
    }
    else if (game->menu->menuSelected > 4) {
        if (game->menu->menuSelected == 5) {
            game->menu->mapSelectionSpritePosition[0] = animationMenuError(game->menu->mapSelectionSpritePosition[0], seconds, 2000, 732);
            game->menu->mapSelectionSpritePosition[1].y = 0;
            game->menu->mapSelectionSpritePosition[2].y = 0;
        }
        else if (game->menu->menuSelected == 6) {
            game->menu->mapSelectionSpritePosition[0].y = 0;
            game->menu->mapSelectionSpritePosition[1] = animationMenuError(game->menu->mapSelectionSpritePosition[1], seconds, 2000, 732);
            game->menu->mapSelectionSpritePosition[2].y = 0;
        }
        else if (game->menu->menuSelected == 7) {
            game->menu->mapSelectionSpritePosition[0].y = 0;
            game->menu->mapSelectionSpritePosition[1].y = 0;
            game->menu->mapSelectionSpritePosition[2] = animationMenuError(game->menu->mapSelectionSpritePosition[2], seconds, 2000, 732);
        }
        SDL_RenderCopy(game->renderer, game->menu->mapSelectionTexture[0], &game->menu->mapSelectionSpritePosition[0], &game->menu->mapSelectionPosition[0]);
        SDL_RenderCopy(game->renderer, game->menu->mapSelectionTexture[1], &game->menu->mapSelectionSpritePosition[1], &game->menu->mapSelectionPosition[1]);
        SDL_RenderCopy(game->renderer, game->menu->mapSelectionTexture[2], &game->menu->mapSelectionSpritePosition[2], &game->menu->mapSelectionPosition[2]);
    }
    if (game->menu->inputSelected > 0) {
        SDL_RenderCopy(game->renderer, game->menu->inputTexture, NULL, &game->menu->inputPosition);
        game->menu->errorSpritePosition[0].y = 0;
        game->menu->errorSpritePosition[1].y = 0; // just to reset the errors, may consume resources
    }
    else if (game->menu->inputSelected == -1) {
        game->menu->errorSpritePosition[0] = animationMenuError(game->menu->errorSpritePosition[0], &otherSeconds, 1250, 125);
        SDL_RenderCopy(game->renderer, game->menu->errorTexture[0], &game->menu->errorSpritePosition[0], &game->menu->errorPosition[0]);
    }
    else if (game->menu->inputSelected == -2) {
        game->menu->errorSpritePosition[1] = animationMenuError(game->menu->errorSpritePosition[1], &otherSeconds, 1250, 125);
        SDL_RenderCopy(game->renderer, game->menu->errorTexture[1], &game->menu->errorSpritePosition[1], &game->menu->errorPosition[1]);
    }

    SDL_RenderPresent(game->renderer);
}

void gameSdl(gameS *game, int sizeX, int sizeY, int playersThatAreHere[4])
{
    SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
    SDL_RenderClear(game->renderer);
    SDL_RenderCopy(game->renderer, game->statsBackTexture[game->mapFeatures->mapId], NULL, &game->statsBackPos);
    for (int i = 0; i < sizeY; i++) { // map
        for (int j = 0; j < sizeX; j++) {
            SDL_RenderCopy(game->renderer, game->mapFeatures->mapTexArray2D[game->mapFeatures->mapId][game->map[i][j]->caseType], NULL, &game->map[i][j]->casePos);
        }
    }
    for (int l = 0; l < 16; l++) { // bonus
        if (game->bonus->bonusesDT[l]->bonusType != -1) {
            SDL_RenderCopy(game->renderer, game->bonus->bonusTexArray[game->bonus->bonusesDT[l]->bonusType], &game->bonus->bonusesDT[l]->bonusSpRectPosition, &game->bonus->bonusesDT[l]->bonusRectPos);
        }
    }
    int goodXplus;
    int goodXminus;
    int goodYplus;
    int goodYminus;
    goodXplus = 1;
    goodXminus = 1;
    goodYplus = 1;
    goodYminus = 1;
    for (int k = 0; k < 4; k++) { // bombes and players
        if (playersThatAreHere[k] == 1) {
            for (int j = 0; j < game->players[k]->pStats->nbPlantedBombs; j++) {
                goodXplus = 1;
                goodXminus = 1;
                goodYplus = 1;
                goodYminus = 1;
                if (game->players[k]->pBombs->activated[j]) {
                    SDL_RenderCopy(game->renderer, game->bombe->bombTex, &game->players[k]->pBombs->bombSpRectPos[j], &game->players[k]->pBombs->bombRectPos[j]);
                }
                if (game->players[k]->pBombs->exploded[j]) {
                    SDL_RenderCopy(game->renderer, game->bombe->expTexture[0], NULL, &game->players[k]->pBombs->bombRectPos[j]);
                    for (int l = 1; l < radiusBombe; l++)
                    {
                        if (goodYplus)
                        {
                            if (game->map[bombeY + l][bombeX]->caseType < 4)
                            {
                                SDL_RenderCopy(game->renderer, game->bombe->expTexture[6], NULL, &game->map[bombeY + l][bombeX]->casePos);
                            }
                            else
                            {
                                goodYplus = 0;
                            }
                        }
                        if (goodYminus)
                        {
                            if (game->map[bombeY - l][bombeX]->caseType < 4)
                            {
                                SDL_RenderCopy(game->renderer, game->bombe->expTexture[5], NULL, &game->map[bombeY - l][bombeX]->casePos);
                            }
                            else
                            {
                                goodYminus = 0;
                            }
                        }
                        if (goodXplus)
                        {
                            if (game->map[bombeY][bombeX + l]->caseType < 4)
                            {
                                SDL_RenderCopy(game->renderer, game->bombe->expTexture[7], NULL, &game->map[bombeY][bombeX + l]->casePos);
                            }
                            else
                            {
                                goodXplus = 0;
                            }
                        }
                        if (goodXminus)
                        {
                            if (game->map[bombeY][bombeX - l]->caseType < 4)
                            {
                                SDL_RenderCopy(game->renderer, game->bombe->expTexture[8], NULL, &game->map[bombeY][bombeX - l]->casePos);
                            }
                            else
                            {
                                goodXminus = 0;
                            }
                        }
                    }
                    if (goodYplus)
                    {
                        if (game->map[bombeY + radiusBombe][bombeX]->caseType < 4)
                        {
                            SDL_RenderCopy(game->renderer, game->bombe->expTexture[2], NULL, &game->map[bombeY + radiusBombe][bombeX]->casePos);
                        }
                    }
                    if (goodYminus)
                    {
                        if (game->map[bombeY - radiusBombe][bombeX]->caseType < 4)
                        {
                            SDL_RenderCopy(game->renderer, game->bombe->expTexture[1], NULL, &game->map[bombeY - radiusBombe][bombeX]->casePos);
                        }
                    }
                    if (goodXplus)
                    {
                        if (game->map[bombeY][bombeX + radiusBombe]->caseType < 4)
                        {
                            SDL_RenderCopy(game->renderer, game->bombe->expTexture[3], NULL, &game->map[bombeY][bombeX + radiusBombe]->casePos);
                        }
                    }
                    if (goodXminus)
                    {
                        if (game->map[bombeY][bombeX - radiusBombe]->caseType < 4)
                        {
                            SDL_RenderCopy(game->renderer, game->bombe->expTexture[4], NULL, &game->map[bombeY][bombeX - radiusBombe]->casePos);
                        }
                    }
                }
            }
            if (game->players[k]->pStats->nbLife) {
                SDL_RenderCopy(game->renderer, game->players[k]->playerTexture[direction], &game->players[k]->playerSpPos, &game->players[k]->playerRextPos);
                SDL_RenderCopy(game->renderer, game->statsScreen[k]->statTexture, NULL, &game->statsScreen[k]->statPos);
                SDL_RenderCopy(game->renderer, game->statsScreen[k]->alivePlayerTexture, NULL, &game->statsScreen[k]->alivePlayerPos);
                SDL_RenderCopy(game->renderer, game->statsScreen[k]->nbBombsTexture, NULL, &game->statsScreen[k]->nbBombsPos);
                SDL_RenderCopy(game->renderer, game->statsScreen[k]->nbPlantedBombsTexture, NULL, &game->statsScreen[k]->nbPlantedBombsPos);
                SDL_RenderCopy(game->renderer, game->statsScreen[k]->radBombsTexture, NULL, &game->statsScreen[k]->radBombsPos);
                SDL_RenderCopy(game->renderer, game->statsScreen[k]->playerSpeedTexture, NULL, &game->statsScreen[k]->PlayerSpeedRectPost);
            }
            else
            {
                SDL_RenderCopy(game->renderer, game->statsScreen[k]->deadPlayerTexture, NULL, &game->statsScreen[k]->statPos);
            }
        } else {
            SDL_RenderCopy(game->renderer, game->statsScreen[k]->deadPlayerTexture, NULL, &game->statsScreen[k]->statPos);
        }
    }
    SDL_RenderPresent(game->renderer);
}

void menuServer_Sdl(gameS *game)
{
    SDL_RenderClear(game->renderer);
    SDL_RenderCopy(game->renderer, game->menu->backgroundTexture_serverRunning, NULL, &game->windowSize);
    SDL_RenderPresent(game->renderer);
}