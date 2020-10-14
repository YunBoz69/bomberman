#ifndef GAME_H
#define GAME_H

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

//To define the stats menu that will contents the informations about the player
//To define the textures that will serves later to render the surface
typedef struct
{
    SDL_Rect statPos;
    SDL_Rect playerPos;
    SDL_Rect alivePlayerPos;
    SDL_Rect nbBombsPos;
    SDL_Rect nbPlantedBombsPos;
    SDL_Rect radBombsPos;
    SDL_Rect PlayerSpeedRectPost;

    SDL_Texture *statTexture;
    SDL_Texture *deadPlayerTexture;
    SDL_Texture *texturePlayerId;
    SDL_Texture *alivePlayerTexture;
    SDL_Texture *nbBombsTexture;
    SDL_Texture *nbPlantedBombsTexture;
    SDL_Texture *radBombsTexture;
    SDL_Texture *playerSpeedTexture;
} statDisplay;

//To define the bonus items
typedef struct
{
    SDL_Rect bonusRectPos;
    SDL_Point bonusCasePosition;
    SDL_Rect bonusSpRectPosition;
    int bonusType;
} bonusS;

//To define the bonus items texture's
typedef struct
{
    SDL_Texture *bonusTexArray[5];
    bonusS **bonusesDT;
} allBonusS;

//To define the maps textures
typedef struct
{
    SDL_Texture *mapTexArray2D[5][5];
    int mapId;
} mapInfoS;

//To define the cases (obstacles) and it containts
typedef struct
{
    SDL_Rect casePos;
    int caseType;

    int bombCase;
    int playerCase;
    int bonusCase;
} caseS;

//To define the ip and port for connection
typedef struct
{
    char *port;
    char *ipAdress;

    int portInt;
    int ipAdresseInt;
} infoServer;

//To define the actual the actual datas of the player
typedef struct
{
    int nbLife;
    int playerSpeed;
    int nbBombs;
    int nbPlantedBombs;
    int radBombs;
} pStats;

//To define the bomb and the explosion textures
typedef struct
{
    SDL_Texture *bombTex;
    SDL_Texture *expTexture[9];
} bombS;

//To define the bomb datas(position, state...)
typedef struct
{
    SDL_Rect bombRectPos[4];
    SDL_Rect bombSpRectPos[4];
    SDL_Point caseBombPos[4];
    int activated[4];
    int exploded[4];
    int caseBombPost[4][2];
    Uint32 expTime[4];
} playerBombS;

//To define the datas about the player
typedef struct
{
    SDL_Texture *playerTexture[6];
    SDL_Rect playerRextPos;
    SDL_Rect playerSpPos;
    SDL_Point playerSquarePosition;

    playerBombS *pBombs;
    pStats *pStats;
    int direction;
} playerS;

//To define informations about sockets
typedef struct
{
    int clients[4];
    int synchNbSockets;
    int maxSockets;
} gameNetwork;

//To define the font, the blitting, the texture...of the main menu display, the network selection and the map one
typedef struct
{
    TTF_Font *font;
    TTF_Font *font_12;
    SDL_Surface *inputFont;
    SDL_Surface *backgroundSurface;
    SDL_Texture *inputTexture;
    SDL_Texture *backgroundTexture;
    SDL_Texture *backgroundTexture_selectMap;
    SDL_Texture *backgroundTexture_serverRunning;

    SDL_Rect inputPosition;
    int menuSelected;
    int inputSelected;

    SDL_Rect errorPosition[2];
    SDL_Rect errorSpritePosition[2];
    SDL_Texture *errorTexture[2];

    SDL_Rect firstButtonPosition;
    SDL_Rect firstButtonSpritePosition;
    SDL_Texture *firstButtonTexture;

    SDL_Rect secondButtonPosition;
    SDL_Rect secondButtonSpritePosition;
    SDL_Texture *secondButtonTexture;

    SDL_Rect selectPortPosition;
    SDL_Texture *selectPortTexture;

    SDL_Rect selectIpPosition;
    SDL_Texture *selectIpTexture;

    SDL_Rect selectInputPosition;
    SDL_Texture *selectInputTexture;

    SDL_Rect mapSelectionPosition[3];
    SDL_Rect mapSelectionSpritePosition[3];
    SDL_Texture *mapSelectionTexture[3];
} menuSDL;

//To define the struct of the game in overall with its contents
typedef struct
{
    SDL_Point gameScreenSize;
    SDL_Point statScreenSize;
    SDL_Rect windowSize;
    SDL_Window *window;
    SDL_Renderer *renderer;
    statDisplay *statsScreen[4];

    playerS *players[4];
    bombS *bombe;
    menuSDL *menu;
    infoServer *infos;

    int nbX;
    int nbY;
    caseS ***map;
    mapInfoS *mapFeatures;
    gameNetwork *serverSockets;
    allBonusS *bonus;

    SDL_Texture *statsBackTexture[3];
    SDL_Rect statsBackPos;
} gameS;

//functions of the game and its contains
gameS *gameInit();

void gameDestroy(gameS *game);
void gameSdl(gameS *game, int sizeX, int sizeY, int playersThatAreHere[4]);
void menuSdl(gameS *game, Uint32 *seconds);
int eventMenu(gameS *game);
int inputPortEvent(char *port, gameS *game, int size);

int playerMove(gameS *game, SDL_Keycode direction, Uint32 *seconds, int num);
void generateBomb(gameS *game, int num);

void *handle_connection(void *socket);
void *_server(void *gameVoid);
int _client(char *ip, int port, gameS *game);
void switchText(gameS *game, char *message, int size, SDL_Rect rect, SDL_Surface *surface, SDL_Texture *texture);

SDL_Texture *textChange(gameS *game, char *message, int size);
SDL_Texture *changeTextSize(gameS *game, char *message);

// functions of the map
gameS *mapInit(int sizeX, int sizeY, gameS *game);
gameS *playersInit(gameS *game);
gameS *socketsInit(gameS *game);

// events_client
int client_eventmsgSend(gameS *game, Uint32 *sec, int playerNumber, char *msg);
int client_PlayerMove(gameS *game, Uint32 *sec, int dir, int special_comm, int num);
int client_eventmsgReceive(gameS *game, int sock, Uint32 *seconds, char *msg, int playersThatAreHere[4], int num_move, int *player_num_move, int *dir, int *speacialCommand);
gameS *mapInitClient(int sizeX, int sizeY, gameS *game);

// events_server
int server_event(gameS *game, Uint32 *sec, int direction, int no_player);
int gameMovePlayer_server(gameS *game, Uint32 *sec, int direction, int no_player);
int evnt_server_inside();
void bombe_explosion(gameS *game, int bombeNum, int no_player);
void bombe_continue_explosion(gameS *game, int bombeNum, int no_player);
void bombe_explosion_duree(gameS *game, int no_player);
gameS *mapInitServer(int sizeX, int sizeY, gameS *game);

// events_game
int animations(gameS *game, int animationsArray[][4], Uint32 secondsAnimations[]);
int animationHandling(gameS *game, int animationsArrayElement[3], Uint32 secondsAnimations[], int playerNumber, int elementNumber, int speacialCommand);
void initSurfaces(SDL_Surface *surfacesPlayers[4][6]);
void mapToMessage(gameS *game, char *message);
gameS *initBonuses(int sizeX, int sizeY, gameS *game);
gameS *buttonsInit(gameS *game);
int server_bonus_taking(gameS *game, int no_player, char *msg);
int bonusTaking_Client(gameS *game, int num, char *message);
int bonusAnimations(gameS *game, Uint32 *seconds);
int concat_Stat(gameS *game, char *msg);
SDL_Texture *changeTextSize_12(gameS *game, char *message);
int playerStatsToMessage(gameS *game, char *message);
int selectMapEvent(gameS *game);
int bonusToMessage(gameS *game, char *message);
void menuServer_Sdl(gameS *game);
int bonus_creation(gameS *game, int positionX, int positionY, char *msg);
void mapToMessage(gameS *game, char *message);
gameS *initHud(gameS *game);
SDL_Rect animationMenu(SDL_Rect spritePosition, Uint32 *seconds, int maxSpritePosition, int incrementSpritePosition);
SDL_Rect animationMenuError(SDL_Rect spritePosition, Uint32 *seconds, int maxSpritePosition, int incrementSpritePosition);


#endif
