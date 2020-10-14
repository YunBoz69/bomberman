#include "../../include/main.h"

/* INIT THE GAME ///////////////////////////////////////////////////*/
gameS *gameInit()
{

    // init and malloc the game variables
    gameS *game = NULL;
    game = malloc(sizeof(gameS));
    game->bombe = malloc(sizeof(bombS));
    game->menu = malloc(sizeof(menuSDL));
    game->infos = malloc(sizeof(infoServer));
    game->serverSockets = malloc(sizeof(gameNetwork));
    game->mapFeatures = malloc(sizeof(mapInfoS));

    // malloc the players
    for (int i = 0; i < 4; i++) {
        game->players[i] = malloc(sizeof(playerS));
        game->players[i]->pBombs = malloc(sizeof(playerBombS)) + 1;
        game->players[i]->pStats = malloc(sizeof(pStats));
    }

    // nombre de blocks dans la map [n]x[n]
    game->nbX = 15;
    game->nbY = 11;

    game->serverSockets->maxSockets = 0;

    if (!game) {
        printf("Error : Game structure Initialisation");
        gameDestroy(game);
        return NULL;
    }

    // init the sdl and sdl_ttf
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error : SDL Initialisation : %s \n", SDL_GetError());
        gameDestroy(game);
        return NULL;
    }
    printf("SDL initialised correctly\n");
    if (TTF_Init() != 0) {
        printf("Error : SDL_TTF Initialisation : %s \n", TTF_GetError());
        gameDestroy(game);
        return NULL;
    }
    printf("SDL_TTF initialised correctly\n");

    game->gameScreenSize.x = 720;
    game->gameScreenSize.y = 528;
    game->statScreenSize.x = 940;
    game->statScreenSize.y = 528;
    game->windowSize.w = 940;
    game->windowSize.h = 528;
    game->windowSize.x = 0;
    game->windowSize.y = 0;
    game->window = NULL;
    game->renderer = NULL;

    game->bombe->bombTex = NULL;

    game->menu->menuSelected = 1;
    game->menu->inputFont = NULL;
    game->menu->font = TTF_OpenFont("./fonts/font.ttf", 20);
    game->menu->font_12 = TTF_OpenFont("./fonts/font.ttf", 12);

    game->menu->inputSelected = 0;
    game->menu->inputPosition.x = game->gameScreenSize.x / 32;
    game->menu->inputPosition.y = game->gameScreenSize.x / 12;
    game->menu->inputPosition.w = game->gameScreenSize.x / 16;
    game->menu->inputPosition.h = game->gameScreenSize.y / 16;

    // port, ip an other network stuf
    game->infos->port = malloc(sizeof(char) * 8);
    game->infos->ipAdress = malloc(sizeof(char) * 24);
    memset(game->infos->port, '\0', 8);
    memset(game->infos->ipAdress, '\0', 24);
    game->infos->port[0] = ':';
    game->infos->ipAdress[0] = ':';

    // create the window and renderer
    game->window = SDL_CreateWindow("Bomberman", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    game->statScreenSize.x, game->statScreenSize.y, SDL_WINDOW_SHOWN);

    if (game->window) {
        game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
        if (!game->renderer) {
            printf("Error : SDL Renderer creation : %s \n", SDL_GetError());
            gameDestroy(game);
            return NULL;
        }
        printf("SDL Window and Renderer initialised correctly\n");
    } else {
        printf("Error : SDL Window creation : %s \n", SDL_GetError());
        gameDestroy(game);
        return NULL;
    }

    // init the 4 players  // init and malloc the map
    game = playersInit(game);
    game = mapInitClient(game->nbX, game->nbY, game);
    game = socketsInit(game);
    game = initBonuses(game->nbX, game->nbY, game);
    game = buttonsInit(game);
    game = initHud(game);
    //printf("%d", game->map[0][0]->casePos.y);

    SDL_Surface *surfaceBombe = IMG_Load("./images/bombe.png");
    if (!surfaceBombe) {
        printf("Error : SDL Surface creation (image : \"bombe.png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->bombe->bombTex = SDL_CreateTextureFromSurface(game->renderer, surfaceBombe);
        if (!game->bombe->bombTex) {
            printf("Error : SDL Texture creation bombe: %s \n", IMG_GetError());
            SDL_FreeSurface(surfaceBombe);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceBombe);
        printf("bombe texture initialised correctly\n");
    }

    SDL_Surface *surfaceFlame_center = IMG_Load("./images/flames/flames_center.png");
    SDL_Surface *surfaceFlame_up = IMG_Load("./images/flames/flames_up.png");
    SDL_Surface *surfaceFlame_down = IMG_Load("./images/flames/flames_down.png");
    SDL_Surface *surfaceFlame_right = IMG_Load("./images/flames/flames_right.png");
    SDL_Surface *surfaceFlame_left = IMG_Load("./images/flames/flames_left.png");
    SDL_Surface *surfaceFlame_up_center = IMG_Load("./images/flames/flames_up_center.png");
    SDL_Surface *surfaceFlame_down_center = IMG_Load("./images/flames/flames_down_center.png");
    SDL_Surface *surfaceFlame_right_center = IMG_Load("./images/flames/flames_right_center.png");
    SDL_Surface *surfaceFlame_left_center = IMG_Load("./images/flames/flames_left_center.png");
    if (!surfaceFlame_up || !surfaceFlame_down || !surfaceFlame_right || !surfaceFlame_left) {
        printf("Error : SDL Surface creation (image : \"flames_'up/down/left/right'.png\"): %s \n", IMG_GetError());
        if (surfaceFlame_center) {
            SDL_FreeSurface(surfaceFlame_center);
        }
        if (surfaceFlame_up) {
            SDL_FreeSurface(surfaceFlame_up);
        }
        if (surfaceFlame_down) {
            SDL_FreeSurface(surfaceFlame_down);
        }
        if (surfaceFlame_right) {
            SDL_FreeSurface(surfaceFlame_right);
        }
        if (surfaceFlame_left) {
            SDL_FreeSurface(surfaceFlame_left);
        }
        if (surfaceFlame_up) {
            SDL_FreeSurface(surfaceFlame_up_center);
        }
        if (surfaceFlame_down) {
            SDL_FreeSurface(surfaceFlame_down_center);
        }
        if (surfaceFlame_right) {
            SDL_FreeSurface(surfaceFlame_right_center);
        }
        if (surfaceFlame_left) {
            SDL_FreeSurface(surfaceFlame_left_center);
        }
        gameDestroy(game);
        return NULL;
    } else {
        game->bombe->expTexture[0] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_center);
        game->bombe->expTexture[1] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_up);
        game->bombe->expTexture[2] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_down);
        game->bombe->expTexture[3] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_right);
        game->bombe->expTexture[4] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_left);
        game->bombe->expTexture[5] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_up_center);
        game->bombe->expTexture[6] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_down_center);
        game->bombe->expTexture[7] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_right_center);
        game->bombe->expTexture[8] = SDL_CreateTextureFromSurface(game->renderer, surfaceFlame_left_center);
        for (int i = 0; i < 9; i++) {
            if (!game->bombe->expTexture[i]) {
                printf("Error : SDL Texture creation flame: %s \n", IMG_GetError());
                SDL_FreeSurface(surfaceFlame_center);
                SDL_FreeSurface(surfaceFlame_up);
                SDL_FreeSurface(surfaceFlame_down);
                SDL_FreeSurface(surfaceFlame_right);
                SDL_FreeSurface(surfaceFlame_left);
                SDL_FreeSurface(surfaceFlame_up_center);
                SDL_FreeSurface(surfaceFlame_down_center);
                SDL_FreeSurface(surfaceFlame_right_center);
                SDL_FreeSurface(surfaceFlame_left_center);
                gameDestroy(game);
                return NULL;
            }
        }
        SDL_FreeSurface(surfaceFlame_center);
        SDL_FreeSurface(surfaceFlame_up);
        SDL_FreeSurface(surfaceFlame_down);
        SDL_FreeSurface(surfaceFlame_right);
        SDL_FreeSurface(surfaceFlame_left);
        SDL_FreeSurface(surfaceFlame_up_center);
        SDL_FreeSurface(surfaceFlame_down_center);
        SDL_FreeSurface(surfaceFlame_right_center);
        SDL_FreeSurface(surfaceFlame_left_center);
        printf("flame texture initialised correctly\n");
    }

    SDL_Surface *surfaceBackground = IMG_Load("./images/background.png");
    if (!surfaceBackground) {
        printf("Error : SDL Surface creation (image : \"background.png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->backgroundTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceBackground);
        if (!game->menu->backgroundTexture) {
            printf("Error : SDL Texture creation input: %s \n", IMG_GetError());
            SDL_FreeSurface(surfaceBackground);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceBackground);
        printf("input texture initialised correctly\n");
    }

    SDL_Surface *surfaceBackground_selectMap = IMG_Load("./images/background_selectmap.png");
    if (!surfaceBackground_selectMap) {
        printf("Error : SDL Surface creation (image : \"background.png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->backgroundTexture_selectMap = SDL_CreateTextureFromSurface(game->renderer, surfaceBackground_selectMap);
        if (!game->menu->backgroundTexture_selectMap) {
            printf("Error : SDL Texture creation input: %s \n", IMG_GetError());
            SDL_FreeSurface(surfaceBackground_selectMap);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceBackground_selectMap);
        printf("input texture initialised correctly\n");
    }

    SDL_Surface *surfaceBackground_serverRunning = IMG_Load("./images/background_serverRunning.png");
    if (!surfaceBackground_serverRunning) {
        printf("Error : SDL Surface creation (image : \"background_serverRunning.png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->backgroundTexture_serverRunning = SDL_CreateTextureFromSurface(game->renderer, surfaceBackground_serverRunning);
        if (!game->menu->backgroundTexture_serverRunning) {
            printf("Error : SDL Texture creation input: %s \n", IMG_GetError());
            SDL_FreeSurface(surfaceBackground_serverRunning);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceBackground_serverRunning);
        printf("input texture initialised correctly\n");
    }
    printf("menu texture initialised correctly\n\n");
    return game;
}

/* INIT THE MAP ///////////////////////////////////////////////////*/
gameS *mapInit(int sizeX, int sizeY, gameS *game)
{
    game->map = (caseS ***)malloc(sizeY * sizeof(caseS **));
    for (int i = 0; i < sizeY; i++) {
        game->map[i] = (caseS **)malloc(sizeX * sizeof(caseS *));
    }
    SDL_Surface *surfaceCase_map1_1 = IMG_Load("./images/map/0/grass1.png");
    SDL_Surface *surfaceCase_map1_2 = IMG_Load("./images/map/0/grass2.png");
    SDL_Surface *surfaceCase_map1_3 = IMG_Load("./images/map/0/expWall.png");
    SDL_Surface *surfaceCase_map1_4 = IMG_Load("./images/map/0/solid.png");
    SDL_Surface *surfaceCase_map1_5 = IMG_Load("./images/map/0/solid_exterior.png");
    if (!surfaceCase_map1_1 || !surfaceCase_map1_2 || !surfaceCase_map1_3 || !surfaceCase_map1_4 || !surfaceCase_map1_5) {
        printf("Error : SDL Surface creation (image : \"grass1.png\" or \"grass2.png\" or \"solid.png\" or \"solid_exterior.png\" or \"expWall.png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    }
    game->mapFeatures->mapId = 0;
    game->mapFeatures->mapTexArray2D[0][1] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_1);
    game->mapFeatures->mapTexArray2D[0][2] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_2);
    game->mapFeatures->mapTexArray2D[0][3] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_3);
    game->mapFeatures->mapTexArray2D[0][4] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_4);
    game->mapFeatures->mapTexArray2D[0][5] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_5);
    for (int j = 0; j < sizeY; j++) {
        for (int k = 0; k < sizeX; k++) {
            game->map[j][k] = malloc(sizeof(caseS));
            int random = rand();
            if (k == 0 || j == 0 || k == sizeX - 1 || j == sizeY - 1) { // case pourtoure innaccessibles
                game->map[j][k]->caseType = 5;
            }
            else if (j % 2 == 1 && k % 2 == 1) { // case simples_2
                if (random % 4 == 0 || (k == 1 && j == 1) || (k == sizeX - 2 && j == sizeY - 2) || (k == 1 && j == sizeY - 2) || (k == sizeX - 2 && j == 1)) {
                    game->map[j][k]->caseType = 2;
                } else {
                    game->map[j][k]->caseType = 3;
                }
            }
            else if (j % 2 == 1 || k % 2 == 1) { // case simples
                if (random % 4 == 0 || (k == 2 && j == 1) || (k == 1 && j == 2) || (k == sizeX - 3 && j == sizeY - 2) || (k == sizeX - 2 && j == sizeY - 3) || (k == 2 && j == sizeY - 2) || (k == 1 && j == sizeY - 3) || (k == sizeX - 3 && j == 1) || (k == sizeX - 2 && j == 2)) {
                    game->map[j][k]->caseType = 1;
                } else {
                    game->map[j][k]->caseType = 3;
                }
            }
            else
            { // case innaccessibles
                game->map[j][k]->caseType = 4;
            }
            game->map[j][k]->playerCase = 0;
            game->map[j][k]->bombCase = 0;
            game->map[j][k]->bonusCase = -1;
            // taille de la case
            game->map[j][k]->casePos.w = game->gameScreenSize.x / sizeX;
            game->map[j][k]->casePos.h = game->gameScreenSize.y / sizeY;

            // position (pixels) de la case
            if (j == 0) {
                game->map[j][k]->casePos.y = 0;
            }
            else
            {
                game->map[j][k]->casePos.y = game->map[j - 1][k]->casePos.y + game->map[j][k]->casePos.h;
            }
            if (k == 0) {
                game->map[j][k]->casePos.x = 0;
            }
            else
            {
                game->map[j][k]->casePos.x = game->map[j][k - 1]->casePos.x + game->map[j][k]->casePos.w;
            }
        }
    }
    SDL_FreeSurface(surfaceCase_map1_1);
    SDL_FreeSurface(surfaceCase_map1_2);
    SDL_FreeSurface(surfaceCase_map1_3);
    SDL_FreeSurface(surfaceCase_map1_4);
    SDL_FreeSurface(surfaceCase_map1_5);
    printf("Map textures initialised correctly\n");
    return game;
}

// map init client
gameS *mapInitClient(int sizeX, int sizeY, gameS *game)
{
    game->map = (caseS ***)malloc(sizeY * sizeof(caseS **));
    for (int i = 0; i < sizeY; i++) {
        game->map[i] = (caseS **)malloc(sizeX * sizeof(caseS *));
    }
    SDL_Surface *surfaceCase_map1_1 = IMG_Load("./images/map/0/grass1.png");
    SDL_Surface *surfaceCase_map1_2 = IMG_Load("./images/map/0/grass2.png");
    SDL_Surface *surfaceCase_map1_3 = IMG_Load("./images/map/0/expWall.png");
    SDL_Surface *surfaceCase_map1_4 = IMG_Load("./images/map/0/solid.png");
    SDL_Surface *surfaceCase_map1_5 = IMG_Load("./images/map/0/solid_exterior.png");
    if (!surfaceCase_map1_1 || !surfaceCase_map1_2 || !surfaceCase_map1_3 || !surfaceCase_map1_4 || !surfaceCase_map1_5) {
        printf("Error : SDL Surface creation (image : \"grass1.png\" or \"grass2.png\" or \"solid.png\" or \"solid_exterior.png\" or \"expWall.png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    }
    game->mapFeatures->mapId = 0;
    game->mapFeatures->mapTexArray2D[0][1] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_1);
    game->mapFeatures->mapTexArray2D[0][2] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_2);
    game->mapFeatures->mapTexArray2D[0][3] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_3);
    game->mapFeatures->mapTexArray2D[0][4] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_4);
    game->mapFeatures->mapTexArray2D[0][5] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map1_5);

    SDL_Surface *surfaceCase_map2_1 = IMG_Load("./images/map/1/grass1.png");
    SDL_Surface *surfaceCase_map2_2 = IMG_Load("./images/map/1/grass2.png");
    SDL_Surface *surfaceCase_map2_3 = IMG_Load("./images/map/1/expWall.png");
    SDL_Surface *surfaceCase_map2_4 = IMG_Load("./images/map/1/solid.png");
    SDL_Surface *surfaceCase_map2_5 = IMG_Load("./images/map/1/solid_exterior.png");
    if (!surfaceCase_map2_1 || !surfaceCase_map2_2 || !surfaceCase_map2_3 || !surfaceCase_map2_4 || !surfaceCase_map2_5) {
        printf("Error : SDL Surface creation (image : \"grass1.png\" or \"grass2.png\" or \"solid.png\" or \"solid_exterior.png\" or \"expWall.png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    }
    game->mapFeatures->mapTexArray2D[1][1] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map2_1);
    game->mapFeatures->mapTexArray2D[1][2] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map2_2);
    game->mapFeatures->mapTexArray2D[1][3] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map2_3);
    game->mapFeatures->mapTexArray2D[1][4] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map2_4);
    game->mapFeatures->mapTexArray2D[1][5] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map2_5);

    SDL_Surface *surfaceCase_map3_1 = IMG_Load("./images/map/2/grass1.png");
    SDL_Surface *surfaceCase_map3_2 = IMG_Load("./images/map/2/grass2.png");
    SDL_Surface *surfaceCase_map3_3 = IMG_Load("./images/map/2/expWall.png");
    SDL_Surface *surfaceCase_map3_4 = IMG_Load("./images/map/2/solid.png");
    SDL_Surface *surfaceCase_map3_5 = IMG_Load("./images/map/2/solid_exterior.png");
    if (!surfaceCase_map3_1 || !surfaceCase_map3_2 || !surfaceCase_map3_3 || !surfaceCase_map3_4 || !surfaceCase_map3_5) {
        printf("Error : SDL Surface creation (image : \"grass1.png\" or \"grass2.png\" or \"solid.png\" or \"solid_exterior.png\" or \"expWall.png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    }
    game->mapFeatures->mapTexArray2D[2][1] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map3_1);
    game->mapFeatures->mapTexArray2D[2][2] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map3_2);
    game->mapFeatures->mapTexArray2D[2][3] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map3_3);
    game->mapFeatures->mapTexArray2D[2][4] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map3_4);
    game->mapFeatures->mapTexArray2D[2][5] = SDL_CreateTextureFromSurface(game->renderer, surfaceCase_map3_5);
    for (int j = 0; j < sizeY; j++) {
        for (int k = 0; k < sizeX; k++) {
            game->map[j][k] = malloc(sizeof(caseS));
            if (k == 0 || j == 0 || k == sizeX - 1 || j == sizeY - 1) { // case pourtoure innaccessibles
                game->map[j][k]->caseType = 5;
            }
            else if (j % 2 == 1 && k % 2 == 1) { // case simples_2
                game->map[j][k]->caseType = 2;
            }
            else if (j % 2 == 1 || k % 2 == 1) { // case simples
                game->map[j][k]->caseType = 1;
            }
            else
            { // case innaccessibles
                game->map[j][k]->caseType = 4;
            }
            game->map[j][k]->playerCase = 0;
            game->map[j][k]->bombCase = 0;
            game->map[j][k]->bonusCase = -1;
            // taille de la case
            game->map[j][k]->casePos.w = game->gameScreenSize.x / sizeX;
            game->map[j][k]->casePos.h = game->gameScreenSize.y / sizeY;

            // position de la case
            if (j == 0) {
                game->map[j][k]->casePos.y = 0;
            }
            else
            {
                game->map[j][k]->casePos.y = game->map[j - 1][k]->casePos.y + game->map[j][k]->casePos.h;
            }
            if (k == 0) {
                game->map[j][k]->casePos.x = 0;
            }
            else
            {
                game->map[j][k]->casePos.x = game->map[j][k - 1]->casePos.x + game->map[j][k]->casePos.w;
            }
        }
    }
    SDL_FreeSurface(surfaceCase_map1_1);
    SDL_FreeSurface(surfaceCase_map1_2);
    SDL_FreeSurface(surfaceCase_map1_3);
    SDL_FreeSurface(surfaceCase_map1_4);
    SDL_FreeSurface(surfaceCase_map1_5);
    printf("Map textures initialised correctly\n");
    return game;
}

gameS *mapInitServer(int sizeX, int sizeY, gameS *game)
{
    for (int j = 0; j < sizeY; j++) {
        for (int k = 0; k < sizeX; k++) {
            int random = rand();
            if (k == 0 || j == 0 || k == sizeX - 1 || j == sizeY - 1) { // case pourtoure innaccessibles
                game->map[j][k]->caseType = 5;
            }
            else if (j % 2 == 1 && k % 2 == 1) { // case simples_2
                if (random % 4 == 0 || (k == 1 && j == 1) || (k == sizeX - 2 && j == sizeY - 2) || (k == 1 && j == sizeY - 2) || (k == sizeX - 2 && j == 1)) {
                    game->map[j][k]->caseType = 2;
                } else {
                    game->map[j][k]->caseType = 3;
                }
            }
            else if (j % 2 == 1 || k % 2 == 1) { // case simples
                if (random % 4 == 0 || (k == 2 && j == 1) || (k == 1 && j == 2) || (k == sizeX - 3 && j == sizeY - 2) || (k == sizeX - 2 && j == sizeY - 3) || (k == 2 && j == sizeY - 2) || (k == 1 && j == sizeY - 3) || (k == sizeX - 3 && j == 1) || (k == sizeX - 2 && j == 2)) {
                    game->map[j][k]->caseType = 1;
                } else {
                    game->map[j][k]->caseType = 3;
                }
            }
            else
            { // case innaccessibles
                game->map[j][k]->caseType = 4;
            }
        }
    }
    printf("Map textures initialised correctly\n");
    return game;
}

/* INIT THE 4 PLAYERS ///////////////////////////////////////////////////*/
gameS *playersInit(gameS *game)
{
    // player 1
    // game->players[0]->playerTexture = NULL;
    game->players[0]->playerRextPos.x = 48;
    game->players[0]->playerRextPos.y = 30;
    game->players[0]->playerRextPos.w = game->gameScreenSize.x / 15;
    game->players[0]->playerRextPos.h = game->gameScreenSize.y / 10;

    game->players[0]->playerSpPos.x = 0;
    game->players[0]->playerSpPos.y = 0;
    game->players[0]->playerSpPos.w = 67;
    game->players[0]->playerSpPos.h = 108;

    game->players[0]->playerSquarePosition.x = 1;
    game->players[0]->playerSquarePosition.y = 1;

    // player 2
    // game->players[1]->playerTexture = NULL;
    game->players[1]->playerRextPos.x = 48 + ((game->nbX - 3) * 48);
    game->players[1]->playerRextPos.y = 30 + ((game->nbY - 3) * 48);
    game->players[1]->playerRextPos.w = game->gameScreenSize.x / 15;
    game->players[1]->playerRextPos.h = game->gameScreenSize.y / 10;

    game->players[1]->playerSpPos.x = 0;
    game->players[1]->playerSpPos.y = 0;
    game->players[1]->playerSpPos.w = 67;
    game->players[1]->playerSpPos.h = 108;

    game->players[1]->playerSquarePosition.x = game->nbX - 2;
    game->players[1]->playerSquarePosition.y = game->nbY - 2;

    // player 3
    // game->players[2]->playerTexture = NULL;
    game->players[2]->playerRextPos.x = 48 + ((game->nbX - 3) * 48);
    game->players[2]->playerRextPos.y = 30;
    game->players[2]->playerRextPos.w = game->gameScreenSize.x / 15;
    game->players[2]->playerRextPos.h = game->gameScreenSize.y / 10;

    game->players[2]->playerSpPos.x = 0;
    game->players[2]->playerSpPos.y = 0;
    game->players[2]->playerSpPos.w = 67;
    game->players[2]->playerSpPos.h = 108;

    game->players[2]->playerSquarePosition.x = game->nbX - 2;
    game->players[2]->playerSquarePosition.y = 1;

    // player 4
    // game->players[3]->playerTexture = NULL;
    game->players[3]->playerRextPos.x = 48;
    game->players[3]->playerRextPos.y = 30 + ((game->nbY - 3) * 48);
    game->players[3]->playerRextPos.w = game->gameScreenSize.x / 15;
    game->players[3]->playerRextPos.h = game->gameScreenSize.y / 10;

    game->players[3]->playerSpPos.x = 0;
    game->players[3]->playerSpPos.y = 0;
    game->players[3]->playerSpPos.w = 67;
    game->players[3]->playerSpPos.h = 108;

    game->players[3]->playerSquarePosition.x = 1;
    game->players[3]->playerSquarePosition.y = game->nbY - 2;

    for (int i = 0; i < 4; i++) {
        game->players[i]->pStats->radBombs = 1;
        game->players[i]->pStats->nbBombs = 20;
        game->players[i]->pStats->nbPlantedBombs = 2;
        game->players[i]->pStats->playerSpeed = 300;
        game->players[i]->pStats->nbLife = 1;
        game->players[i]->direction = 1;
        for (int j = 0; j < 4; j++) {
            game->players[i]->pBombs->bombRectPos[j].x = 0;
            game->players[i]->pBombs->bombRectPos[j].y = 0;
            game->players[i]->pBombs->bombRectPos[j].w = 48;
            game->players[i]->pBombs->bombRectPos[j].h = 48;
            game->players[i]->pBombs->bombSpRectPos[j].x = 0;
            game->players[i]->pBombs->bombSpRectPos[j].y = 0;
            game->players[i]->pBombs->bombSpRectPos[j].w = 84;
            game->players[i]->pBombs->bombSpRectPos[j].h = 84;
            game->players[i]->pBombs->activated[j] = 0;
            game->players[i]->pBombs->exploded[j] = 0;
            game->players[i]->pBombs->caseBombPos[j].x = 0;
            game->players[i]->pBombs->caseBombPos[j].y = 0;
        }
    }
    // init the players surfaces
    SDL_Surface *surfacesPlayers[4][6];
    initSurfaces(surfacesPlayers);

    // init the players texture
    for (int k = 0; k < 4; k++) {
        for (int l = 0; l < 4; l++) {
            game->players[k]->playerTexture[l] = SDL_CreateTextureFromSurface(game->renderer, surfacesPlayers[k][l]);
        }
    }

    return game;
}

gameS *socketsInit(gameS *game)
{
    for (int i = 0; i < 4; i++) {
        game->serverSockets->clients[i] = -1;
    }
    puts("serverSockets initialised correctly on -1");
    return game;
}

void initSurfaces(SDL_Surface *surfacesPlayers[4][6])
{
    surfacesPlayers[0][0] = IMG_Load("./images/characters/characters_dos_1.png");
    surfacesPlayers[0][1] = IMG_Load("./images/characters/characters_face_1.png");
    surfacesPlayers[0][2] = IMG_Load("./images/characters/characters_coteD_1.png");
    surfacesPlayers[0][3] = IMG_Load("./images/characters/characters_coteG_1.png");
    surfacesPlayers[0][4] = IMG_Load("./images/characters/characters.png");
    surfacesPlayers[0][5] = IMG_Load("./images/characters/characters.png");

    surfacesPlayers[1][0] = IMG_Load("./images/characters/characters_dos_2.png");
    surfacesPlayers[1][1] = IMG_Load("./images/characters/characters_face_2.png");
    surfacesPlayers[1][2] = IMG_Load("./images/characters/characters_coteD_2.png");
    surfacesPlayers[1][3] = IMG_Load("./images/characters/characters_coteG_2.png");
    surfacesPlayers[1][4] = IMG_Load("./images/characters/characters.png");
    surfacesPlayers[1][5] = IMG_Load("./images/characters/characters.png");

    surfacesPlayers[2][0] = IMG_Load("./images/characters/characters_dos_3.png");
    surfacesPlayers[2][1] = IMG_Load("./images/characters/characters_face_3.png");
    surfacesPlayers[2][2] = IMG_Load("./images/characters/characters_coteD_3.png");
    surfacesPlayers[2][3] = IMG_Load("./images/characters/characters_coteG_3.png");
    surfacesPlayers[2][4] = IMG_Load("./images/characters/characters.png");
    surfacesPlayers[2][5] = IMG_Load("./images/characters/characters.png");

    surfacesPlayers[3][0] = IMG_Load("./images/characters/characters_dos_4.png");
    surfacesPlayers[3][1] = IMG_Load("./images/characters/characters_face_4.png");
    surfacesPlayers[3][2] = IMG_Load("./images/characters/characters_coteD_4.png");
    surfacesPlayers[3][3] = IMG_Load("./images/characters/characters_coteG_4.png");
    surfacesPlayers[3][4] = IMG_Load("./images/characters/characters.png");
    surfacesPlayers[3][5] = IMG_Load("./images/characters/characters.png");
}

gameS *initBonuses(int sizeX, int sizeY, gameS *game)
{
    game->bonus = malloc(sizeof(allBonusS));
    game->bonus->bonusesDT = malloc(sizeof(bonusS) * 16);
    for (int i = 0; i < 16; i++) {
        game->bonus->bonusesDT[i] = malloc(sizeof(bonusS));
        game->bonus->bonusesDT[i]->bonusType = -1;
        game->bonus->bonusesDT[i]->bonusRectPos.x = 0;
        game->bonus->bonusesDT[i]->bonusRectPos.y = 0;
        game->bonus->bonusesDT[i]->bonusRectPos.w = game->gameScreenSize.x / sizeX;
        game->bonus->bonusesDT[i]->bonusRectPos.h = game->gameScreenSize.y / sizeY;
    }

    SDL_Surface *surfaceBonus = IMG_Load("./images/bonus/bonus_1.png");
    SDL_Surface *surfaceBonus_2 = IMG_Load("./images/bonus/bonus_2.png");
    SDL_Surface *surfaceBonus_3 = IMG_Load("./images/bonus/bonus_3.png");
    SDL_Surface *surfaceBonus_4 = IMG_Load("./images/bonus/bonus_4.png");
    SDL_Surface *surfaceBonus_5 = IMG_Load("./images/bonus/bonus_5.png");
    if (!surfaceBonus || !surfaceBonus_2 || !surfaceBonus_3 || !surfaceBonus_4 || !surfaceBonus_5) {
        printf("Error : SDL Surface creation (image : \"bonus_1 / bonus_2 / bonus_3 / bonus_4 / bonus_5.png\"): %s \n", IMG_GetError());
        if (surfaceBonus) {
            SDL_FreeSurface(surfaceBonus);
        }
        if (surfaceBonus_2) {
            SDL_FreeSurface(surfaceBonus_2);
        }
        if (surfaceBonus_3) {
            SDL_FreeSurface(surfaceBonus_3);
        }
        if (surfaceBonus_4) {
            SDL_FreeSurface(surfaceBonus_4);
        }
        if (surfaceBonus_5) {
            SDL_FreeSurface(surfaceBonus_5);
        }
        gameDestroy(game);
        return NULL;
    } else {
        game->bonus->bonusTexArray[0] = SDL_CreateTextureFromSurface(game->renderer, surfaceBonus);
        game->bonus->bonusTexArray[1] = SDL_CreateTextureFromSurface(game->renderer, surfaceBonus_2);
        game->bonus->bonusTexArray[2] = SDL_CreateTextureFromSurface(game->renderer, surfaceBonus_3);
        game->bonus->bonusTexArray[3] = SDL_CreateTextureFromSurface(game->renderer, surfaceBonus_4);
        game->bonus->bonusTexArray[4] = SDL_CreateTextureFromSurface(game->renderer, surfaceBonus_5);
        if (!game->bonus->bonusTexArray[0] || !game->bonus->bonusTexArray[1] || !game->bonus->bonusTexArray[2] || !game->bonus->bonusTexArray[3] || !game->bonus->bonusTexArray[4]) {
            printf("Error : SDL Texture creation player: %s \n", IMG_GetError());
            SDL_FreeSurface(surfaceBonus);
            SDL_FreeSurface(surfaceBonus_2);
            SDL_FreeSurface(surfaceBonus_3);
            SDL_FreeSurface(surfaceBonus_4);
            SDL_FreeSurface(surfaceBonus_5);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceBonus);
        SDL_FreeSurface(surfaceBonus_2);
        SDL_FreeSurface(surfaceBonus_3);
        SDL_FreeSurface(surfaceBonus_4);
        SDL_FreeSurface(surfaceBonus_5);
        printf("Player texture initialised correctly\n");
    }
    return game;
}

gameS *buttonsInit(gameS *game)
{
    game->menu->firstButtonPosition.x = game->windowSize.w / 3;
    game->menu->firstButtonPosition.y = game->windowSize.h / 1.5;
    game->menu->firstButtonPosition.w = game->windowSize.w / 3;
    game->menu->firstButtonPosition.h = (game->windowSize.w / 3) / 5;
    game->menu->firstButtonSpritePosition.x = 0;
    game->menu->firstButtonSpritePosition.y = 0;
    game->menu->firstButtonSpritePosition.w = 600;
    game->menu->firstButtonSpritePosition.h = 125;
    SDL_Surface *surfaceButtonOne = IMG_Load("./images/join_game.png");
    if (!surfaceButtonOne) {
        printf("Error : SDL Surface creation (image : \"join_game.png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->firstButtonTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceButtonOne);
        if (!game->menu->firstButtonTexture) {
            printf("Error : SDL Texture creation input: %s \n", IMG_GetError());
            SDL_FreeSurface(surfaceButtonOne);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceButtonOne);
        printf("button one texture initialised correctly\n");
    }

    game->menu->secondButtonPosition.x = game->windowSize.w / 3;
    game->menu->secondButtonPosition.y = game->windowSize.h / 1.2;
    game->menu->secondButtonPosition.w = game->windowSize.w / 3;
    game->menu->secondButtonPosition.h = (game->windowSize.w / 3) / 5;
    game->menu->secondButtonSpritePosition.x = 0;
    game->menu->secondButtonSpritePosition.y = 0;
    game->menu->secondButtonSpritePosition.w = 600;
    game->menu->secondButtonSpritePosition.h = 125;
    SDL_Surface *surfaceButtonTwo = IMG_Load("./images/create_game.png");
    if (!surfaceButtonTwo) {
        printf("Error : SDL Surface creation (image : \"create_game.png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->secondButtonTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceButtonTwo);
        if (!game->menu->secondButtonTexture) {
            printf("Error : SDL Texture creation input: %s \n", IMG_GetError());
            SDL_FreeSurface(surfaceButtonTwo);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceButtonTwo);
        printf("button one texture initialised correctly\n");
    }

    game->menu->selectPortPosition.x = game->windowSize.w / 6;
    game->menu->selectPortPosition.y = game->windowSize.h / 1.3;
    game->menu->selectPortPosition.w = game->windowSize.w / 1.5;
    game->menu->selectPortPosition.h = (game->windowSize.w / 3) / 5;
    SDL_Surface *surfaceSelectPort = IMG_Load("./images/port_client.png");
    if (!surfaceSelectPort) {
        printf("Error : SDL Surface creation (image : \"port_client.png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->selectPortTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceSelectPort);
        if (!game->menu->selectPortTexture) {
            printf("Error : SDL Texture creation input: %s \n", IMG_GetError());
            SDL_FreeSurface(surfaceSelectPort);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceSelectPort);
        printf("button one texture initialised correctly\n");
    }

    game->menu->selectIpPosition.x = game->windowSize.w / 6;
    game->menu->selectIpPosition.y = game->windowSize.h / 1.3;
    game->menu->selectIpPosition.w = game->windowSize.w / 1.5;
    game->menu->selectIpPosition.h = (game->windowSize.w / 3) / 5;
    SDL_Surface *surfaceSelectIp = IMG_Load("./images/ip_client.png");
    if (!surfaceSelectIp) {
        printf("Error : SDL Surface creation (image : \"ip_client.png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->selectIpTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceSelectIp);
        if (!game->menu->selectIpTexture) {
            printf("Error : SDL Texture creation input: %s \n", IMG_GetError());
            SDL_FreeSurface(surfaceSelectIp);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceSelectIp);
        printf("button one texture initialised correctly\n");
    }

    game->menu->selectInputPosition.x = game->windowSize.w / 6;
    game->menu->selectInputPosition.y = game->windowSize.h / 1.3 + ((game->windowSize.w / 3) / 6);
    game->menu->selectInputPosition.w = game->windowSize.w / 4;
    game->menu->selectInputPosition.h = (game->windowSize.w / 3) / 5;
    SDL_Surface *surfaceSelectInput = IMG_Load("./images/input.png");
    if (!surfaceSelectInput) {
        printf("Error : SDL Surface creation (image : \"input.png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->selectInputTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceSelectInput);
        if (!game->menu->selectInputTexture) {
            printf("Error : SDL Texture creation input: %s \n", IMG_GetError());
            SDL_FreeSurface(surfaceSelectInput);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceSelectInput);
        printf("button one texture initialised correctly\n");
    }
    game->menu->inputPosition.x = game->windowSize.w / 5.8;
    game->menu->inputPosition.y = game->windowSize.h / 1.1;
    game->menu->inputPosition.w = game->gameScreenSize.x / 16;
    game->menu->inputPosition.h = game->gameScreenSize.y / 16;

    game->menu->errorPosition[0].x = 0;
    game->menu->errorPosition[0].y = game->windowSize.h / 16;
    game->menu->errorPosition[0].w = game->windowSize.w / 4;
    game->menu->errorPosition[0].h = (game->windowSize.w / 3) / 5;
    game->menu->errorSpritePosition[0].x = 0;
    game->menu->errorSpritePosition[0].y = 0;
    game->menu->errorSpritePosition[0].w = 600;
    game->menu->errorSpritePosition[0].h = 125;
    game->menu->errorPosition[1].x = 0;
    game->menu->errorPosition[1].y = game->windowSize.h / 16;
    game->menu->errorPosition[1].w = game->windowSize.w / 4;
    game->menu->errorPosition[1].h = (game->windowSize.w / 3) / 5;
    game->menu->errorSpritePosition[1].x = 0;
    game->menu->errorSpritePosition[1].y = 0;
    game->menu->errorSpritePosition[1].w = 600;
    game->menu->errorSpritePosition[1].h = 125;

    SDL_Surface *surfaceError_1 = IMG_Load("./images/wrong_port_or_ip.png");
    SDL_Surface *surfaceError_2 = IMG_Load("./images/could_not_connect.png");
    if (!surfaceError_1 || !surfaceError_2) {
        printf("Error : SDL Surface creation (image : \"wrong_port_or_ip / could_not_connect.png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->errorTexture[0] = SDL_CreateTextureFromSurface(game->renderer, surfaceError_1);
        game->menu->errorTexture[1] = SDL_CreateTextureFromSurface(game->renderer, surfaceError_2);
        if (!game->menu->errorTexture[0] || !game->menu->errorTexture[1]) {
            printf("Error : SDL Texture creation errors textures: %s \n", IMG_GetError());
            SDL_FreeSurface(surfaceError_1);
            SDL_FreeSurface(surfaceError_2);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceError_1);
        SDL_FreeSurface(surfaceError_2);
        printf("errors textures initilised correctly\n");
    }

    game->menu->mapSelectionPosition[0].x = 0;
    game->menu->mapSelectionPosition[0].y = game->gameScreenSize.y / 3;
    game->menu->mapSelectionPosition[0].w = game->windowSize.w / 3;
    ;
    game->menu->mapSelectionPosition[0].h = game->menu->mapSelectionPosition[0].w / 1.36;

    game->menu->mapSelectionPosition[1].x = game->windowSize.w / 3;
    game->menu->mapSelectionPosition[1].y = game->gameScreenSize.y / 3;
    game->menu->mapSelectionPosition[1].w = game->windowSize.w / 3;
    ;
    game->menu->mapSelectionPosition[1].h = game->menu->mapSelectionPosition[1].w / 1.36;

    game->menu->mapSelectionPosition[2].x = (game->windowSize.w * 2) / 3;
    game->menu->mapSelectionPosition[2].y = game->gameScreenSize.y / 3;
    game->menu->mapSelectionPosition[2].w = game->windowSize.w / 3;
    game->menu->mapSelectionPosition[2].h = game->menu->mapSelectionPosition[2].w / 1.36;

    game->menu->mapSelectionSpritePosition[0].x = 0;
    game->menu->mapSelectionSpritePosition[0].y = 0;
    game->menu->mapSelectionSpritePosition[0].w = 1000;
    ;
    game->menu->mapSelectionSpritePosition[0].h = 733;

    game->menu->mapSelectionSpritePosition[1].x = 0;
    game->menu->mapSelectionSpritePosition[1].y = 0;
    game->menu->mapSelectionSpritePosition[1].w = 1000;
    ;
    game->menu->mapSelectionSpritePosition[1].h = 733;

    game->menu->mapSelectionSpritePosition[2].x = 0;
    game->menu->mapSelectionSpritePosition[2].y = 0;
    game->menu->mapSelectionSpritePosition[2].w = 1000;
    game->menu->mapSelectionSpritePosition[2].h = 733;
    SDL_Surface *surfaceMapSelect_1 = IMG_Load("./images/map/map0.png");
    SDL_Surface *surfaceMapSelect_2 = IMG_Load("./images/map/map1.png");
    SDL_Surface *surfaceMapSelect_3 = IMG_Load("./images/map/map2.png");
    if (!surfaceMapSelect_1 || !surfaceMapSelect_2 || !surfaceMapSelect_3) {
        printf("Error : SDL Surface creation (image : \"map0\" \"map1\" \"map2\".png\"): %s \n", IMG_GetError());
        gameDestroy(game);
        return NULL;
    } else {
        game->menu->mapSelectionTexture[0] = SDL_CreateTextureFromSurface(game->renderer, surfaceMapSelect_1);
        game->menu->mapSelectionTexture[1] = SDL_CreateTextureFromSurface(game->renderer, surfaceMapSelect_2);
        game->menu->mapSelectionTexture[2] = SDL_CreateTextureFromSurface(game->renderer, surfaceMapSelect_3);
        if (!game->menu->mapSelectionTexture[0] || !game->menu->mapSelectionTexture[1] || !game->menu->mapSelectionTexture[2]) {
            printf("Error : SDL Texture creation input: %s \n", IMG_GetError());
            SDL_FreeSurface(surfaceMapSelect_1);
            SDL_FreeSurface(surfaceMapSelect_2);
            SDL_FreeSurface(surfaceMapSelect_3);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceMapSelect_1);
        SDL_FreeSurface(surfaceMapSelect_2);
        SDL_FreeSurface(surfaceMapSelect_3);
        printf("maps selection textures initialised correctly\n");
    }
    return game;
}

gameS *initHud(gameS *game)
{
    for (int i = 0; i < 4; i++) {
        game->statsScreen[i] = malloc(sizeof(statDisplay));
        if (i == 0) {
            game->statsScreen[i]->statPos.x = game->gameScreenSize.x;
            game->statsScreen[i]->statPos.y = 0;
            game->statsScreen[i]->statPos.h = game->windowSize.h / 4;
            game->statsScreen[i]->statPos.w = game->windowSize.w - game->gameScreenSize.x;

            // game->statsScreen[i]->playerPos.x = game->statsScreen[i]->statPos.x + game->statsScreen[i]->statPos.w / 1.5;
            // game->statsScreen[i]->playerPos.y = game->statsScreen[i]->statPos.h / 8;
            // game->statsScreen[i]->playerPos.h = 0;
            // game->statsScreen[i]->playerPos.w = 0;

            game->statsScreen[i]->alivePlayerPos.x = game->statsScreen[i]->statPos.x + game->statsScreen[i]->statPos.w / 3;
            game->statsScreen[i]->alivePlayerPos.y = (game->statsScreen[i]->statPos.h / 13.1);
            game->statsScreen[i]->alivePlayerPos.h = 0;
            game->statsScreen[i]->alivePlayerPos.w = 0;

            game->statsScreen[i]->nbBombsPos.x = game->statsScreen[i]->statPos.x + game->statsScreen[i]->statPos.w / 3;
            game->statsScreen[i]->nbBombsPos.y = (game->statsScreen[i]->statPos.h / 24) * 7.05;
            game->statsScreen[i]->nbBombsPos.h = 0;
            game->statsScreen[i]->nbBombsPos.w = 0;

            game->statsScreen[i]->nbPlantedBombsPos.x = game->statsScreen[i]->statPos.x + game->statsScreen[i]->statPos.w / 3;
            game->statsScreen[i]->nbPlantedBombsPos.y = (game->statsScreen[i]->statPos.h / 8) * 3.6;
            game->statsScreen[i]->nbPlantedBombsPos.h = 0;
            game->statsScreen[i]->nbPlantedBombsPos.w = 0;

            game->statsScreen[i]->radBombsPos.x = game->statsScreen[i]->statPos.x + ((game->statsScreen[i]->statPos.w / 6) * 5);
            game->statsScreen[i]->radBombsPos.y = (game->statsScreen[i]->statPos.h / 13.1);
            game->statsScreen[i]->radBombsPos.h = 0;
            game->statsScreen[i]->radBombsPos.w = 0;

            game->statsScreen[i]->PlayerSpeedRectPost.x = game->statsScreen[i]->statPos.x + game->statsScreen[i]->statPos.w / 3;
            game->statsScreen[i]->PlayerSpeedRectPost.y = (game->statsScreen[i]->statPos.h / 8) * 5.1;
            game->statsScreen[i]->PlayerSpeedRectPost.h = 0;
            game->statsScreen[i]->PlayerSpeedRectPost.w = 0;
        } else {
            game->statsScreen[i]->statPos.x = game->statsScreen[i - 1]->statPos.x;
            game->statsScreen[i]->statPos.y = game->statsScreen[i - 1]->statPos.y + game->statsScreen[i - 1]->statPos.h;
            game->statsScreen[i]->statPos.h = game->statsScreen[i - 1]->statPos.h;
            game->statsScreen[i]->statPos.w = game->statsScreen[i - 1]->statPos.w;

            // game->statsScreen[i]->playerPos.x = game->statsScreen[i-1]->playerPos.x;
            // game->statsScreen[i]->playerPos.y = game->statsScreen[i-1]->playerPos.y + game->statsScreen[i-1]->statPos.h;
            // game->statsScreen[i]->playerPos.h = game->statsScreen[i-1]->playerPos.h;
            // game->statsScreen[i]->playerPos.w = game->statsScreen[i-1]->playerPos.w;

            game->statsScreen[i]->alivePlayerPos.x = game->statsScreen[i - 1]->alivePlayerPos.x;
            game->statsScreen[i]->alivePlayerPos.y = game->statsScreen[i - 1]->alivePlayerPos.y + game->statsScreen[i - 1]->statPos.h;
            game->statsScreen[i]->alivePlayerPos.h = game->statsScreen[i - 1]->alivePlayerPos.h;
            game->statsScreen[i]->alivePlayerPos.w = game->statsScreen[i - 1]->alivePlayerPos.w;

            game->statsScreen[i]->nbBombsPos.x = game->statsScreen[i - 1]->nbBombsPos.x;
            game->statsScreen[i]->nbBombsPos.y = game->statsScreen[i - 1]->nbBombsPos.y + game->statsScreen[i - 1]->statPos.h;
            game->statsScreen[i]->nbBombsPos.h = game->statsScreen[i - 1]->nbBombsPos.h;
            game->statsScreen[i]->nbBombsPos.w = game->statsScreen[i - 1]->nbBombsPos.w;

            game->statsScreen[i]->radBombsPos.x = game->statsScreen[i - 1]->radBombsPos.x;
            game->statsScreen[i]->radBombsPos.y = game->statsScreen[i - 1]->radBombsPos.y + game->statsScreen[i - 1]->statPos.h;
            game->statsScreen[i]->radBombsPos.h = game->statsScreen[i - 1]->radBombsPos.h;
            game->statsScreen[i]->radBombsPos.w = game->statsScreen[i - 1]->radBombsPos.w;

            game->statsScreen[i]->nbPlantedBombsPos.x = game->statsScreen[i - 1]->nbPlantedBombsPos.x;
            game->statsScreen[i]->nbPlantedBombsPos.y = game->statsScreen[i - 1]->nbPlantedBombsPos.y + game->statsScreen[i - 1]->statPos.h;
            game->statsScreen[i]->nbPlantedBombsPos.h = game->statsScreen[i - 1]->nbPlantedBombsPos.h;
            game->statsScreen[i]->nbPlantedBombsPos.w = game->statsScreen[i - 1]->nbPlantedBombsPos.w;

            game->statsScreen[i]->PlayerSpeedRectPost.x = game->statsScreen[i - 1]->PlayerSpeedRectPost.x;
            game->statsScreen[i]->PlayerSpeedRectPost.y = game->statsScreen[i - 1]->PlayerSpeedRectPost.y + game->statsScreen[i - 1]->statPos.h;
            game->statsScreen[i]->PlayerSpeedRectPost.h = game->statsScreen[i - 1]->PlayerSpeedRectPost.h;
            game->statsScreen[i]->PlayerSpeedRectPost.w = game->statsScreen[i - 1]->PlayerSpeedRectPost.w;
        }
        game->statsBackPos.x = game->gameScreenSize.x;
        game->statsBackPos.y = 0;
        game->statsBackPos.h = game->gameScreenSize.y;
        game->statsBackPos.w = game->windowSize.w - game->gameScreenSize.x;
        // game->statsScreen[i]->texturePlayerId = textChange(game, "player n : 0\0", 12);
        char *stat = malloc(sizeof(char) * 4);
        memset(stat, '\0', 4);
        sprintf(stat, "%d", game->players[i]->pStats->nbLife);
        game->statsScreen[i]->alivePlayerTexture = changeTextSize_12(game, stat);

        memset(stat, '\0', 4);
        sprintf(stat, "%d", game->players[i]->pStats->nbBombs);
        game->statsScreen[i]->nbBombsTexture = changeTextSize_12(game, stat);

        memset(stat, '\0', 4);
        sprintf(stat, "%d", game->players[i]->pStats->nbPlantedBombs);
        game->statsScreen[i]->nbPlantedBombsTexture = changeTextSize_12(game, stat);

        memset(stat, '\0', 4);
        sprintf(stat, "%d", game->players[i]->pStats->radBombs);
        game->statsScreen[i]->radBombsTexture = changeTextSize_12(game, stat);

        memset(stat, '\0', 4);
        sprintf(stat, "%d", game->players[i]->pStats->playerSpeed);
        game->statsScreen[i]->playerSpeedTexture = changeTextSize_12(game, stat);
        // SDL_QueryTexture(game->statsScreen[i]->texturePlayerId, NULL, NULL, &game->statsScreen[i]->playerPos.w, &game->statsScreen[i]->playerPos.h);
        SDL_QueryTexture(game->statsScreen[i]->alivePlayerTexture, NULL, NULL, &game->statsScreen[i]->alivePlayerPos.w, &game->statsScreen[i]->alivePlayerPos.h);
        SDL_QueryTexture(game->statsScreen[i]->nbBombsTexture, NULL, NULL, &game->statsScreen[i]->nbBombsPos.w, &game->statsScreen[i]->nbBombsPos.h);
        SDL_QueryTexture(game->statsScreen[i]->nbPlantedBombsTexture, NULL, NULL, &game->statsScreen[i]->nbPlantedBombsPos.w, &game->statsScreen[i]->nbPlantedBombsPos.h);
        SDL_QueryTexture(game->statsScreen[i]->radBombsTexture, NULL, NULL, &game->statsScreen[i]->radBombsPos.w, &game->statsScreen[i]->radBombsPos.h);
        SDL_QueryTexture(game->statsScreen[i]->playerSpeedTexture, NULL, NULL, &game->statsScreen[i]->PlayerSpeedRectPost.w, &game->statsScreen[i]->PlayerSpeedRectPost.h);
    }
    SDL_Surface *surfaceHud = IMG_Load("./images/hud/hud_p1.png");
    SDL_Surface *surfaceHud_2 = IMG_Load("./images/hud/hud_p2.png");
    SDL_Surface *surfaceHud_3 = IMG_Load("./images/hud/hud_p3.png");
    SDL_Surface *surfaceHud_4 = IMG_Load("./images/hud/hud_p4.png");
    SDL_Surface *surfaceHud_dead = IMG_Load("./images/hud/hud_p1_dead.png");
    SDL_Surface *surfaceHud_2_dead = IMG_Load("./images/hud/hud_p2_dead.png");
    SDL_Surface *surfaceHud_3_dead = IMG_Load("./images/hud/hud_p3_dead.png");
    SDL_Surface *surfaceHud_4_dead = IMG_Load("./images/hud/hud_p4_dead.png");
    SDL_Surface *surfaceHud_background_1 = IMG_Load("./images/hud/background_hudMap0.png");
    SDL_Surface *surfaceHud_background_2 = IMG_Load("./images/hud/background_hudMap1.png");
    SDL_Surface *surfaceHud_background_3 = IMG_Load("./images/hud/background_hudMap2.png");
    if (!surfaceHud || !surfaceHud_2 || !surfaceHud_3 || !surfaceHud_4 || !surfaceHud_dead || !surfaceHud_2_dead || !surfaceHud_3_dead || !surfaceHud_4_dead || !surfaceHud_background_1 || !surfaceHud_background_2 || !surfaceHud_background_3) {
        printf("Error : SDL Surface creation (image : \"hud_p1 / hud_p2 / bonus_3 / hud_p3 / hud_p4.png\"): %s \n", IMG_GetError());
        if (surfaceHud) {
            SDL_FreeSurface(surfaceHud);
        }
        if (surfaceHud_2) {
            SDL_FreeSurface(surfaceHud_2);
        }
        if (surfaceHud_3) {
            SDL_FreeSurface(surfaceHud_3);
        }
        if (surfaceHud_4) {
            SDL_FreeSurface(surfaceHud_4);
        }
        if (surfaceHud_dead) {
            SDL_FreeSurface(surfaceHud_dead);
        }
        if (surfaceHud_2_dead) {
            SDL_FreeSurface(surfaceHud_2_dead);
        }
        if (surfaceHud_3_dead) {
            SDL_FreeSurface(surfaceHud_3_dead);
        }
        if (surfaceHud_4_dead) {
            SDL_FreeSurface(surfaceHud_4_dead);
        }
        if (surfaceHud_background_1) {
            SDL_FreeSurface(surfaceHud_background_1);
        }
        if (surfaceHud_background_2) {
            SDL_FreeSurface(surfaceHud_background_2);
        }
        if (surfaceHud_background_3) {
            SDL_FreeSurface(surfaceHud_background_3);
        }
        gameDestroy(game);
        return NULL;
    } else {
        game->statsScreen[0]->statTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceHud);
        game->statsScreen[1]->statTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_2);
        game->statsScreen[2]->statTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_3);
        game->statsScreen[3]->statTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_4);
        game->statsScreen[0]->deadPlayerTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_dead);
        game->statsScreen[1]->deadPlayerTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_2_dead);
        game->statsScreen[2]->deadPlayerTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_3_dead);
        game->statsScreen[3]->deadPlayerTexture = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_4_dead);
        game->statsBackTexture[0] = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_background_1);
        game->statsBackTexture[1] = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_background_2);
        game->statsBackTexture[2] = SDL_CreateTextureFromSurface(game->renderer, surfaceHud_background_3);
        if (!game->statsScreen[0]->statTexture || !game->statsScreen[1]->statTexture || !game->statsScreen[2]->statTexture || !game->statsScreen[3]->statTexture || !game->statsScreen[0]->deadPlayerTexture || !game->statsScreen[1]->deadPlayerTexture || !game->statsScreen[2]->deadPlayerTexture || !game->statsScreen[3]->deadPlayerTexture) {
            printf("Error : SDL Texture creation statsScreen: %s \n", IMG_GetError());
            SDL_FreeSurface(surfaceHud);
            SDL_FreeSurface(surfaceHud_2);
            SDL_FreeSurface(surfaceHud_3);
            SDL_FreeSurface(surfaceHud_4);
            SDL_FreeSurface(surfaceHud_dead);
            SDL_FreeSurface(surfaceHud_2_dead);
            SDL_FreeSurface(surfaceHud_3_dead);
            SDL_FreeSurface(surfaceHud_4_dead);
            gameDestroy(game);
            return NULL;
        }
        SDL_FreeSurface(surfaceHud);
        SDL_FreeSurface(surfaceHud_2);
        SDL_FreeSurface(surfaceHud_3);
        SDL_FreeSurface(surfaceHud_4);
        SDL_FreeSurface(surfaceHud_dead);
        SDL_FreeSurface(surfaceHud_2_dead);
        SDL_FreeSurface(surfaceHud_3_dead);
        SDL_FreeSurface(surfaceHud_4_dead);
        SDL_FreeSurface(surfaceHud_background_1);
        SDL_FreeSurface(surfaceHud_background_2);
        SDL_FreeSurface(surfaceHud_background_3);
        printf("statsScreen texture initialised correctly\n");
    }
    return game;
}
