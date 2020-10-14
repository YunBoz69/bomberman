#include "../../include/main.h"

#define playerX game->players[num]->playerSquarePosition.x
#define playerY game->players[num]->playerSquarePosition.y

// RETOURNE 5 POUR LA BOMBE ET 1.2.3.4 POUR LES FIRECTIONS 
int client_eventmsgSend(gameS *game, Uint32 *sec, int playerNumber, char *msg)
{
    SDL_Event event;
    
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            return 0;
        else if (event.type == SDL_KEYDOWN) 
        {
            switch (event.key.keysym.sym) 
            {
                case SDLK_ESCAPE:
                    strcpy(msg, "0\n");
                    puts("quit");
                    return 0;
                    break;

                case SDLK_SPACE:
                    strcpy(msg, "5");
                    puts("bombe");
                    break;

                case SDLK_UP:
                    strcpy(msg, "1");
                    game->players[playerNumber]->direction = 0;
                    puts("up");
                    break;

                case SDLK_DOWN:
                    strcpy(msg, "2");
                    game->players[playerNumber]->direction = 1;
                    puts("down");
                    break;

                case SDLK_RIGHT:
                    strcpy(msg, "3");
                    game->players[playerNumber]->direction = 2;
                    puts("right");
                    break;

                case SDLK_LEFT:
                    strcpy(msg, "4");
                    game->players[playerNumber]->direction = 3;
                    puts("left");
                    break;

                default:
                    printf("EVENT CLIENT**** Touche non reconnu -> %d\n", event.key.keysym.sym);
                    break;
            }
        }
    }
    return 6;
}

int client_PlayerMove(gameS *game, Uint32 *sec, int dir, int special_comm, int num)
{
    if (dir == 1)
    {
        game->players[num]->playerSquarePosition.y--;
        game->players[num]->direction = 0;
        return 1;
    }
    else if (dir == 2)
    {
        game->players[num]->playerSquarePosition.y++;
        game->players[num]->direction = 1;
        return 2;
    }
    else if (dir == 3)
    {
        game->players[num]->playerSquarePosition.x++;
        game->players[num]->direction = 2;
        return 3;
    }
    else if (dir == 4)
    {
        game->players[num]->playerSquarePosition.x--;
        game->players[num]->direction = 3;
        return 4;
    }
    else if (dir == 5)
    {
        game->players[num]->pStats->nbBombs--;
        game->players[num]->pBombs->activated[special_comm] = 1;

        game->players[num]->pBombs->caseBombPos[special_comm].x = game->players[num]->playerSquarePosition.x;
        game->players[num]->pBombs->caseBombPos[special_comm].y = game->players[num]->playerSquarePosition.y;
        game->players[num]->pBombs->bombRectPos[special_comm].x = 48 + ((game->players[num]->playerSquarePosition.x - 1) * 48);
        game->players[num]->pBombs->bombRectPos[special_comm].y = 48 + ((game->players[num]->playerSquarePosition.y - 1) * 48);

        game->players[num]->pBombs->caseBombPost[special_comm][1] = game->players[num]->playerSquarePosition.x;
        game->players[num]->pBombs->caseBombPost[special_comm][0] = game->players[num]->playerSquarePosition.y;
        return 5;
    }
    return 6;
}

int client_eventmsgReceive(gameS *game, int sock, Uint32 *seconds, char *msg, int playersThatAreHere[4], int num_move, int *player_num_move, int *dir, int *speacialCommand)
{

    if (msg[0] == '!') {  //les comm
        *dir = msg[1] - 48;
        *player_num_move = msg[2] - 48;
        client_PlayerMove(game, seconds,*dir, 0, *player_num_move);
        if (msg[3] != '\0' && msg[3] != '(') {
            bonusTaking_Client(game, *player_num_move, msg);
        }
        else if (msg[3] != '\0') {
            msg += 3;
            num_move = client_eventmsgReceive(game, sock, seconds, msg, playersThatAreHere, num_move, player_num_move, dir, speacialCommand);
        }
        num_move++;
        return num_move;
    }
    else if (msg[0] == '$') { // pour la bombe
        *dir = 5;
        *player_num_move = msg[2] - 48;
        *speacialCommand = msg[3] - 48;
        client_PlayerMove(game, seconds,*dir, *speacialCommand, *player_num_move);
        if (msg[4] != '\0') {
            msg += 4;
            num_move = client_eventmsgReceive(game, sock, seconds, msg, playersThatAreHere, num_move, player_num_move, dir, speacialCommand);
        }
        num_move++;
        return num_move;
    }
    else if (msg[0] == '%') { // pour num de joueur
        printf("num player -> %s\n", msg);
        for (int j = 0; j < 4; j++)
            playersThatAreHere[j] = msg[j + 1] - 48;
        if (msg[5] != '\0') {
            msg += 5;
            num_move = client_eventmsgReceive(game, sock, seconds, msg, playersThatAreHere, num_move, player_num_move, dir, speacialCommand);
        }
        puts("num player MAJ");
        return num_move;
    }
    else if (msg[0] == '\'') { /// pour connaitre positon joueur, et surtout pour connaitre la positiob du nouveau joueur connecté
        printf("joueur pos message : %s\n", msg);
        int j = 1;
        for (int k = 0; k < 4; k++) {
            if (playersThatAreHere[k] > 0) {
                if (game->players[k]->playerSquarePosition.x != msg[j] - 48) {
                    game->players[k]->playerSquarePosition.x = msg[j] - 48;
                    game->players[k]->playerRextPos.x = 48 + ((game->players[k]->playerSquarePosition.x - 1) * 48);
                    num_move--;
                }
                if (game->players[k]->playerSquarePosition.y != msg[j + 1] - 48) {
                    game->players[k]->playerSquarePosition.y = msg[j + 1] - 48;
                    game->players[k]->playerRextPos.y = 30 + ((game->players[k]->playerSquarePosition.y - 1) * 48);
                    num_move--;
                }
                if (game->players[k]->pStats->nbLife != msg[j + 2] - 48) {
                    game->players[k]->pStats->nbLife = msg[j + 2] - 48;
                }
                if (game->players[k]->pStats->playerSpeed != (msg[j + 3] - 48) * 10) {
                    game->players[k]->pStats->playerSpeed = (msg[j + 3] - 48) * 10;
                }
                if (game->players[k]->pStats->nbBombs != msg[j + 4] - 48) {
                    game->players[k]->pStats->nbBombs = msg[j + 4] - 48;
                }
                if (game->players[k]->pStats->nbPlantedBombs != msg[j + 5] - 48) {
                    game->players[k]->pStats->nbPlantedBombs = msg[j + 5] - 48;
                }
            }
            j += 6;
        }
        if (msg[25] != '\0') {
            msg += 25;
            num_move = client_eventmsgReceive(game, sock, seconds, msg, playersThatAreHere, num_move, player_num_move, dir, speacialCommand);
        }
        puts("EVENT CLIENT joueur pos maj");
        return num_move;
    }
    else if (msg[0] == '\"') { // pour l'explosion initiale
        for (int k = 0; k < 4; k++) {
            game->players[k]->pStats->nbLife = msg[k + 1] - 48;
            if (game->players[k]->pStats->nbLife == 0)
                printf("EVENT CLIENT joueur %d est mort\n", k);
        }
        *dir = 6;
        *player_num_move = msg[5] - 48;
        *speacialCommand = msg[6] - 48;
        
        game->players[*player_num_move]->pBombs->activated[*speacialCommand] = 0;
        game->players[*player_num_move]->pBombs->exploded[*speacialCommand] = 1;
        num_move++;
        int i = 7;
        while (msg[i] != '$' && msg[i] != '\0') 
        {
            if ((msg[i] - 48) % 2 == 1 && (msg[i + 1] - 48) % 2 == 1)
                game->map[msg[i] - 48][msg[i + 1] - 48]->caseType = 2;
            else
                game->map[msg[i] - 48][msg[i + 1] - 48]->caseType = 1;
            i += 2;
        }
        if (msg[i] == '$')
            i++;
        int k;
        while (msg[i] != '\0' && msg[i] != '%' && msg[i] != '\'' && msg[i] != '(' && msg[i] != '!' && msg[i] != '$' && msg[i] != '\"' && msg[i] != '#' && msg[i] != ')' && msg[i] != '*') {
            k = msg[i] - 48;
            game->bonus->bonusesDT[k]->bonusType = msg[i + 1] - 48;
            printf("EVENT CLIENT --- type de bonus -> %d\n", game->bonus->bonusesDT[k]->bonusType);
            game->bonus->bonusesDT[k]->bonusCasePosition.x = msg[i + 2] - 48;
            game->bonus->bonusesDT[k]->bonusCasePosition.y = msg[i + 3] - 48;
            printf("EVENT CLIENNTTT bonusX : %d | bonusY : %d\n", game->bonus->bonusesDT[k]->bonusCasePosition.x, game->bonus->bonusesDT[k]->bonusCasePosition.y);
            game->bonus->bonusesDT[k]->bonusRectPos.x = game->map[game->bonus->bonusesDT[k]->bonusCasePosition.y][game->bonus->bonusesDT[k]->bonusCasePosition.x]->casePos.x;
            game->bonus->bonusesDT[k]->bonusRectPos.y = game->map[game->bonus->bonusesDT[k]->bonusCasePosition.y][game->bonus->bonusesDT[k]->bonusCasePosition.x]->casePos.y;
            game->bonus->bonusesDT[k]->bonusSpRectPosition.x = 0;
            game->bonus->bonusesDT[k]->bonusSpRectPosition.y = 0;
            game->bonus->bonusesDT[k]->bonusSpRectPosition.h = 64;
            game->bonus->bonusesDT[k]->bonusSpRectPosition.w = 64;
            i += 4;
        }
        if (msg[i] != '\0') {
            msg += i;
            num_move = client_eventmsgReceive(game, sock, seconds, msg, playersThatAreHere, num_move, player_num_move, dir, speacialCommand);
        }
        return num_move;
    }
    else if (msg[0] == '#') { // poiur l'explosion
        for (int k = 0; k < 4; k++) 
        {
            game->players[k]->pStats->nbLife = msg[k + 1] - 48;
            if (game->players[k]->pStats->nbLife == 0)
                 printf("JOUEUR %d EST MORT\n", k);
        }
        if (msg[5] != '\0') {
            msg += 5;
            num_move = client_eventmsgReceive(game, sock, seconds, msg, playersThatAreHere, num_move, player_num_move, dir, speacialCommand);
        }
        return num_move;
    }
    else if (msg[0] == '(')  // pour la map
    { 
        printf("EVENT CLIENNTTTT msg map : %s\n", msg);
        int l = 1;
        while (msg[l] != '\0' && msg[l] != '%' && msg[l] != '\'' && msg[l] != '(' && msg[l] != '!' && msg[l] != '$' && msg[l] != '\"' && msg[l] != '#' && msg[l] != ')' && msg[l] != '*') {
            if (msg[l] - 48 == 3) 
                   game->map[msg[l + 1] - 48][msg[l + 2] - 48]->caseType = 3;
            l += 3;
        }
        if (msg[l] != '\0') {
            msg += l;
            num_move = client_eventmsgReceive(game, sock, seconds, msg, playersThatAreHere, num_move, player_num_move, dir, speacialCommand);
        }
    }
    else if (msg[0] == ')') {
        printf("EVENT CLIENNTTTT MSG BONUS TAB : %s\n", msg);
        int i = 1;
        while (msg[i] != '\0' && msg[i] != '%' && msg[i] != '\'' && msg[i] != '(' && msg[i] != '!' && msg[i] != '$' && msg[i] != '\"' && msg[i] != '#' && msg[i] != ')' && msg[i] != '*') {
            game->bonus->bonusesDT[msg[i] - 48]->bonusType = msg[i + 1] - 48;
            game->bonus->bonusesDT[msg[i] - 48]->bonusCasePosition.x = msg[i + 2] - 48;
            game->bonus->bonusesDT[msg[i] - 48]->bonusCasePosition.y = msg[i + 3] - 48;
            game->bonus->bonusesDT[msg[i] - 48]->bonusSpRectPosition.x = 0;
            game->bonus->bonusesDT[msg[i] - 48]->bonusSpRectPosition.y = 0;
            game->bonus->bonusesDT[msg[i] - 48]->bonusSpRectPosition.h = 64;
            game->bonus->bonusesDT[msg[i] - 48]->bonusSpRectPosition.w = 64;
            game->bonus->bonusesDT[msg[i] - 48]->bonusRectPos.x = game->map[game->bonus->bonusesDT[msg[i] - 48]->bonusCasePosition.y][game->bonus->bonusesDT[msg[i] - 48]->bonusCasePosition.x]->casePos.x;
            game->bonus->bonusesDT[msg[i] - 48]->bonusRectPos.y = game->map[game->bonus->bonusesDT[msg[i] - 48]->bonusCasePosition.y][game->bonus->bonusesDT[msg[i] - 48]->bonusCasePosition.x]->casePos.y;
            i += 4;
        }
        if (msg[i] != '\0') {
            msg += i;
            num_move = client_eventmsgReceive(game, sock, seconds, msg, playersThatAreHere, num_move, player_num_move, dir, speacialCommand);
        }
    }
    else if (msg[0] == '*') {
        puts("MAJ JOUEUR STAT");
        int j = 1;
        char *stat = malloc(sizeof(char) * 4);
        memset(stat, '\0', 4);
        for (size_t i = 0; i < 4; i++) {
            game->players[i]->pStats->nbLife = msg[j + 1] - 48;
            game->players[i]->pStats->playerSpeed = (msg[j + 2] - 48) * 50;
            game->players[i]->pStats->nbBombs = msg[j + 3] - 48;
            game->players[i]->pStats->nbPlantedBombs = msg[j + 4] - 48;
            game->players[i]->pStats->radBombs = msg[j + 5] - 48;

            memset(stat, '\0', 4);
            sprintf(stat, "%d", game->players[i]->pStats->nbBombs);
            game->statsScreen[i]->nbBombsTexture = changeTextSize_12(game, stat);
            SDL_QueryTexture(game->statsScreen[i]->nbBombsTexture, NULL, NULL, &game->statsScreen[i]->nbBombsPos.w, &game->statsScreen[i]->nbBombsPos.h);
            memset(stat, '\0', 4);

            sprintf(stat, "%d", game->players[i]->pStats->playerSpeed);
            game->statsScreen[i]->playerSpeedTexture = changeTextSize_12(game, stat);
            SDL_QueryTexture(game->statsScreen[i]->playerSpeedTexture, NULL, NULL, &game->statsScreen[i]->PlayerSpeedRectPost.w, &game->statsScreen[i]->PlayerSpeedRectPost.h);
            memset(stat, '\0', 4);

            sprintf(stat, "%d", game->players[i]->pStats->radBombs);
            game->statsScreen[i]->radBombsTexture = changeTextSize_12(game, stat);
            SDL_QueryTexture(game->statsScreen[i]->radBombsTexture, NULL, NULL, &game->statsScreen[i]->radBombsPos.w, &game->statsScreen[i]->radBombsPos.h);
            memset(stat, '\0', 4);

            sprintf(stat, "%d", game->players[i]->pStats->nbLife);
            game->statsScreen[i]->alivePlayerTexture = changeTextSize_12(game, stat);
            SDL_QueryTexture(game->statsScreen[i]->alivePlayerTexture, NULL, NULL, &game->statsScreen[i]->alivePlayerPos.w, &game->statsScreen[i]->alivePlayerPos.h);
            memset(stat, '\0', 4);

            sprintf(stat, "%d", game->players[i]->pStats->nbPlantedBombs);
            game->statsScreen[i]->nbPlantedBombsTexture = changeTextSize_12(game, stat);
            SDL_QueryTexture(game->statsScreen[i]->nbPlantedBombsTexture, NULL, NULL, &game->statsScreen[i]->nbPlantedBombsPos.w, &game->statsScreen[i]->nbPlantedBombsPos.h);

            j += 6;
        }
        if (msg[j] != '\0') {
            msg += j;
            num_move = client_eventmsgReceive(game, sock, seconds, msg, playersThatAreHere, num_move, player_num_move, dir, speacialCommand);
        }
    }
    return num_move;
}

int bonusTaking_Client(gameS *game, int num, char *message)
{
    char *stat = malloc(sizeof(char) * 4);
    memset(stat, '\0', 4);
    
    game->bonus->bonusesDT[message[3] - 48]->bonusType = -1;
    if (message[4] - 48 == 0) 
    {
        game->players[num]->pStats->nbBombs = message[5] - 48;
        sprintf(stat, "%d", game->players[num]->pStats->nbBombs);
        game->statsScreen[num]->nbBombsTexture = changeTextSize_12(game, stat);
        SDL_QueryTexture(game->statsScreen[num]->nbBombsTexture, NULL, NULL, &game->statsScreen[num]->nbBombsPos.w, &game->statsScreen[num]->nbBombsPos.h);
    }
    else if (message[4] - 48 == 1) 
    {
        game->players[num]->pStats->playerSpeed = (message[5] - 48) * 50;
        sprintf(stat, "%d", game->players[num]->pStats->playerSpeed);
        game->statsScreen[num]->playerSpeedTexture = changeTextSize_12(game, stat);
        SDL_QueryTexture(game->statsScreen[num]->playerSpeedTexture, NULL, NULL, &game->statsScreen[num]->PlayerSpeedRectPost.w, &game->statsScreen[num]->PlayerSpeedRectPost.h);
    }
    else if (message[4] - 48 == 2) 
    {
        game->players[num]->pStats->radBombs = message[5] - 48;
        ;
        sprintf(stat, "%d", game->players[num]->pStats->radBombs);
        game->statsScreen[num]->radBombsTexture = changeTextSize_12(game, stat);
        SDL_QueryTexture(game->statsScreen[num]->radBombsTexture, NULL, NULL, &game->statsScreen[num]->radBombsPos.w, &game->statsScreen[num]->radBombsPos.h);
    }
    else if (message[4] - 48 == 3) 
    {
        game->players[num]->pStats->nbLife = message[5] - 48;
        sprintf(stat, "%d", game->players[num]->pStats->nbLife);
        game->statsScreen[num]->alivePlayerTexture = changeTextSize_12(game, stat);
        SDL_QueryTexture(game->statsScreen[num]->alivePlayerTexture, NULL, NULL, &game->statsScreen[num]->alivePlayerPos.w, &game->statsScreen[num]->alivePlayerPos.h);
    }
    else if (message[4] - 48 == 4) 
    {
        game->players[num]->pStats->nbPlantedBombs = message[5] - 48;
        sprintf(stat, "%d", game->players[num]->pStats->nbPlantedBombs);
        game->statsScreen[num]->nbPlantedBombsTexture = changeTextSize_12(game, stat);
        SDL_QueryTexture(game->statsScreen[num]->nbPlantedBombsTexture, NULL, NULL, &game->statsScreen[num]->nbPlantedBombsPos.w, &game->statsScreen[num]->nbPlantedBombsPos.h);
    }
}