#include "../../include/main.h"

#define playerX game->players[no_player]->playerSquarePosition.x
#define playerY game->players[no_player]->playerSquarePosition.y
#define bombeX game->players[no_player]->pBombs->caseBombPos[bombeNum].x
#define bombeY game->players[no_player]->pBombs->caseBombPos[bombeNum].y

int server_event(gameS *game, Uint32 *sec, int direction, int no_player)
{
    int ret_commande = gameMovePlayer_server(game, sec, direction, no_player);
    
    char *commande = malloc(sizeof(char) * 512);
    
    memset(commande, '\0', 512);
    
    if (ret_commande != 99) 
    {
        if (ret_commande <= 4) 
        {
            commande[0] = '!'; // pour la comm
            commande[1] = ret_commande + '0';
            commande[2] = no_player + '0';
            
            server_bonus_taking(game, no_player, commande);
            concat_Stat(game, commande);
        }else if (ret_commande <= 8) 
        {
            commande[0] = '$'; // pour la bombe
            commande[1] = 5 + '0';
            commande[2] = no_player + '0';
            int no_bombe = ret_commande - 5;
            commande[3] = no_bombe + '0'; 
        }
        for (int x = 0; x < 4; x++) {
            if (game->serverSockets->clients[x] >= 0) {
                send(game->serverSockets->clients[x], commande, 512, MSG_NOSIGNAL);
            }
        }
        printf("EVENTSERVER*****SERVER donnée envoyé => %s\n", commande);
    }
    free(commande);
}

int gameMovePlayer_server(gameS *game, Uint32 *sec, int direction, int no_player)
{
    Uint32 newsec = SDL_GetTicks();
    
    if (direction >= 0 && direction <= 4) 
    {
        if (newsec >= *sec + game->players[no_player]->pStats->playerSpeed) 
        {
            *sec = newsec;

            int move_Y = game->gameScreenSize.y / game->nbY;
            int move_X = game->gameScreenSize.x / game->nbX;

            if (direction == 1) {
                if (game->players[no_player]->playerSquarePosition.y > 0 && game->map[playerY - 1][playerX]->caseType < 3 && game->map[playerY - 1][playerX]->playerCase == 0 && game->map[playerY - 1][playerX]->bombCase == 0) 
                {
                    game->map[playerY][playerX]->playerCase = 0;
                    game->map[playerY - 1][playerX]->playerCase = 1;
                    
                    game->players[no_player]->playerSquarePosition.y--;
                    game->players[no_player]->playerRextPos.y -= move_Y;
                    game->players[no_player]->direction = 0;
                    return 1;
                }
            }
            else if (direction == 2) {
                if (game->players[no_player]->playerSquarePosition.y < game->nbY - 1 && game->map[playerY + 1][playerX]->caseType < 3 && game->map[playerY + 1][playerX]->playerCase == 0 && game->map[playerY + 1][playerX]->bombCase == 0) {
                    game->map[playerY][playerX]->playerCase = 0;
                    game->map[playerY + 1][playerX]->playerCase = 1;
                    game->players[no_player]->playerSquarePosition.y++;
                    game->players[no_player]->playerRextPos.y += move_Y;
                    game->players[no_player]->direction = 1;
                    return 2;
                }
            }
            else if (direction == 3) {
                if (game->players[no_player]->playerSquarePosition.x < game->nbX - 1 && game->map[playerY][playerX + 1]->caseType < 3 && game->map[playerY][playerX + 1]->playerCase == 0 && game->map[playerY][playerX + 1]->bombCase == 0) {
                    game->map[playerY][playerX]->playerCase = 0;
                    game->map[playerY][playerX + 1]->playerCase = 1;
                    game->players[no_player]->playerSquarePosition.x++;
                    game->players[no_player]->playerRextPos.x += move_X;
                    game->players[no_player]->direction = 2;
                    return 3;
                }
            }
            else if (direction == 4) {
                if (game->players[no_player]->playerSquarePosition.x > 0 && game->map[playerY][playerX - 1]->caseType < 3 && game->map[playerY][playerX - 1]->playerCase == 0 && game->map[playerY][playerX - 1]->bombCase == 0) {
                    game->map[playerY][playerX]->playerCase = 0;
                    game->map[playerY][playerX - 1]->playerCase = 1;
                    game->players[no_player]->playerSquarePosition.x--;
                    game->players[no_player]->playerRextPos.x -= move_X;
                    game->players[no_player]->direction = 3;
                    return 4;
                }
            }
        }
    }
    else if (direction == 5) {
        if (game->map[playerY][playerX]->bombCase == 0) { //la je suis obligé de verifier s'il n'y a pas d'autre bomb
            if (newsec >= *sec + 200) {
                if (game->players[no_player]->pStats->nbBombs) {
                    for (int i = 0; i < game->players[no_player]->pStats->nbPlantedBombs; i++)
                    {
                        if (game->players[no_player]->pBombs->activated[i] == 0 && game->players[no_player]->pBombs->exploded[i] == 0)
                        {
                            game->players[no_player]->pBombs->activated[i] = 1;
                            game->map[playerY][playerX]->bombCase = 1;
                            game->players[no_player]->pBombs->caseBombPost[i][0] = playerY;
                            game->players[no_player]->pBombs->caseBombPost[i][1] = playerX;
                            game->players[no_player]->pStats->nbBombs--;
                            game->players[no_player]->pBombs->caseBombPos[i].x = game->players[no_player]->playerSquarePosition.x;
                            game->players[no_player]->pBombs->caseBombPos[i].y = game->players[no_player]->playerSquarePosition.y;

                            game->players[no_player]->pBombs->expTime[i] = SDL_GetTicks();
                            return 5 + i;
                        }
                    }
                }
            }
        }
    }
    return 99;
}

int evnt_server_inside()
{
    SDL_Event event;

    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            return 0;
        else if (event.type == SDL_KEYDOWN) 
        {
            switch (event.key.keysym.sym) 
            {
                case SDLK_ESCAPE: //touche echap
                    return 0;
                    break;

                default: //autre touche
                    printf("EVENTSERVER----Touche tapé non reconnu ->> %d\n", event.key.keysym.sym);
                    return 2;
                    break;
            }
        }
    }
    return 1;
}


void bombe_explosion(gameS *game, int bombeNum, int no_player)
{
	game->players[no_player]->pBombs->activated[bombeNum] = 0;
	game->players[no_player]->pBombs->exploded[bombeNum] = 1;

    char *commande = malloc(sizeof(char) * 512);
    char *bonus_msg = malloc(sizeof(char) * 256);
    
    memset(commande, '\0', 512);
    memset(bonus_msg, '\0', 256);

    commande[0] = '\"'; // signe pour l'explosion
    
    for (int l = 1; l <= 6; l++)       
        commande[l] = '0';

    int rad_bmb = game->players[no_player]->pStats->radBombs;
    
    int X_plus;
    int X_minus;
    int Y_plus;
    int Y_minus;
    
    int switch_bonus = 1;
    int p = 7;
    for (int i = 0; i < 4; i++) 
    {
        X_minus = 1;
        X_plus = 1;

        Y_minus = 1;
        Y_plus = 1;

        for (int j = 1; j <= rad_bmb; j++) 
        {
            if (Y_plus) 
            {
                // verif pos player pour la bombe
                if (game->players[i]->pStats->nbLife && game->players[i]->playerSquarePosition.x == bombeX && game->players[i]->playerSquarePosition.y == bombeY + j && game->map[bombeY + j][bombeX]->caseType <= 3) 
                {
                    if (game->players[i]->pStats->nbLife)
                        game->players[i]->pStats->nbLife--;
                    game->map[bombeY + j][bombeX]->playerCase = 0;
                }
                else if (game->map[bombeY + j][bombeX]->caseType >= 4)
                    Y_plus = 0;
                else if (game->map[bombeY + j][bombeX]->caseType == 3 && switch_bonus <= rad_bmb) 
                {
                    game->map[bombeY + j][bombeX]->caseType = 1;
                    commande[p] = bombeY + j + '0';
                    commande[p + 1] = bombeX + '0';
                    bonus_creation(game, bombeX, bombeY + j, bonus_msg);
                    p += 2;
                }
            }
            if (X_plus) 
            {
                if (game->players[i]->pStats->nbLife && game->players[i]->playerSquarePosition.y == bombeY && game->players[i]->playerSquarePosition.x == bombeX + j && game->map[bombeY][bombeX + j]->caseType <= 3) 
                {
                    if (game->players[i]->pStats->nbLife)
                        game->players[i]->pStats->nbLife--;
                    game->map[bombeY][bombeX + j]->playerCase = 0;
                }
                else if (game->map[bombeY][bombeX + j]->caseType >= 4)
                    X_plus = 0;
                else if (game->map[bombeY][bombeX + j]->caseType == 3 && switch_bonus <= rad_bmb) 
                {
                    game->map[bombeY][bombeX + j]->caseType = 1;
                    commande[p] = bombeY + '0';
                    commande[p + 1] = bombeX + j + '0';
                    if (j>2)
                        bonus_creation(game, bombeX + j, bombeY, bonus_msg);
                    p += 2;
                }
            }
            if (Y_minus) 
            {
                if (game->players[i]->pStats->nbLife && game->players[i]->playerSquarePosition.x == bombeX && game->players[i]->playerSquarePosition.y == bombeY - j && game->map[bombeY - j][bombeX]->caseType <= 3) 
                {
                    if (game->players[i]->pStats->nbLife)
                        game->players[i]->pStats->nbLife--;
                    game->map[bombeY - j][bombeX]->playerCase = 0;
                }
                else if (game->map[bombeY - j][bombeX]->caseType >= 4)
                    Y_minus = 0;
                else if (game->map[bombeY - j][bombeX]->caseType == 3 && switch_bonus <= rad_bmb) 
                {
                    game->map[bombeY - j][bombeX]->caseType = 1;
                    commande[p] = bombeY - j + '0';
                    commande[p + 1] = bombeX + '0';
                    if (j>2)
                        bonus_creation(game, bombeX, bombeY - j, bonus_msg);
                    p += 2;
                }
            }
            if (X_minus) 
            {
                if (game->players[i]->pStats->nbLife && game->players[i]->playerSquarePosition.y == bombeY && game->players[i]->playerSquarePosition.x == bombeX - j && game->map[bombeY][bombeX - j]->caseType <= 3) {
                    if (game->players[i]->pStats->nbLife)
                        game->players[i]->pStats->nbLife--;
                    game->map[bombeY][bombeX - j]->playerCase = 0;
                }
                else if (game->map[bombeY][bombeX - j]->caseType >= 4)
                    X_minus = 0;
                else if (game->map[bombeY][bombeX - j]->caseType == 3 && switch_bonus <= rad_bmb) 
                {
                    game->map[bombeY][bombeX - j]->caseType = 1;
                    commande[p] = bombeY + '0';
                    commande[p + 1] = bombeX - j + '0';
                    if (j>2)
                        bonus_creation(game, bombeX - j, bombeY, bonus_msg);
                    p += 2;
                }
            }
            switch_bonus++;
        }
        if (game->players[i]->pStats->nbLife && game->players[i]->playerSquarePosition.y == bombeY && game->players[i]->playerSquarePosition.x == bombeX)
            if (game->players[i]->pStats->nbLife)
                game->players[i]->pStats->nbLife--;
        commande[i + 1] = game->players[i]->pStats->nbLife + '0';
    }
    
    commande[5] = no_player + '0';
    commande[6] = bombeNum + '0';
    

    char *sign_dollar = malloc(sizeof(char) * 4);
    memset(sign_dollar, '\0', 4);
    
    sign_dollar[0] = '$';


    strcat(commande, bonus_msg); //on ajout le bonus (msg) au comm
    strcat(commande, sign_dollar); // puis le sign (bombe)à la fin
    for (int j = 0; j < 4; j++) 
    {
        if (game->serverSockets->clients[j] >= 0) //on fait touj la verif
            send(game->serverSockets->clients[j], commande, 256, MSG_NOSIGNAL); //on envoie donc les doonéees
        printf("EVENTSERVER bombe explosion------envioyé : %s\n", commande);
    }
}

void bombe_continue_explosion(gameS *game, int bombeNum, int no_player)
{
    int bombTouchedPlayer = 0; // switch used bellow
    char *commande = malloc(sizeof(char) * 16);
    memset(commande, '\0', 16);
    commande[0] = '#'; // for explosion after initial explosion
    for (int i = 0; i < 4; i++) {
        if (game->players[i]->pStats->nbLife && game->players[i]->playerSquarePosition.x == bombeX && (game->players[i]->playerSquarePosition.y == bombeY + 1 || game->players[i]->playerSquarePosition.y == bombeY - 1)) {
            game->players[i]->pStats->nbLife--;
            bombTouchedPlayer = 1;
        }
        if (game->players[i]->pStats->nbLife && game->players[i]->playerSquarePosition.y == bombeY && (game->players[i]->playerSquarePosition.x == bombeX + 1 || game->players[i]->playerSquarePosition.x == bombeX - 1)) {
            game->players[i]->pStats->nbLife--;
            bombTouchedPlayer = 1;
        }
        commande[i + 1] = game->players[i]->pStats->nbLife + '0';
    }
    commande[5] = no_player + '0';
    commande[6] = bombeNum + '0';
    if (bombTouchedPlayer) {
        for (int j = 0; j < 4; j++) {
            if (game->serverSockets->clients[j] >= 0) {
                send(game->serverSockets->clients[j], commande, 16, MSG_NOSIGNAL);
            }
            printf("EVENT SERVER bombe_continue_explosion------server envoyé : %s\n", commande);
        }
    }
}

void bombe_explosion_duree(gameS *game, int no_player)
{
    Uint32 newsec = SDL_GetTicks();
    for (int i = 0; i < game->players[no_player]->pStats->nbPlantedBombs; i++) {
        if (newsec > game->players[no_player]->pBombs->expTime[i] + 4000 && newsec < game->players[no_player]->pBombs->expTime[i] + 5000 && game->players[no_player]->pBombs->activated[i]) {
            printf("AAAAAAAAAA\n");
            bombe_explosion(game, i, no_player);
            game->map[game->players[no_player]->pBombs->caseBombPost[i][0]][game->players[no_player]->pBombs->caseBombPost[i][1]]->bombCase = 0;
        }
        else if (newsec > game->players[no_player]->pBombs->expTime[i] + 4000 && newsec < game->players[no_player]->pBombs->expTime[i] + 5000 && game->players[no_player]->pBombs->exploded[i]) 
            bombe_continue_explosion(game, i, no_player);
        else if (game->players[no_player]->pBombs->exploded[i])
            game->players[no_player]->pBombs->exploded[i] = 0;
    }
}

int bonus_creation(gameS *game, int positionX, int positionY, char *msg)
{
    // petit algo avec des proba pour les creation de bonus
    int random = rand();
    char *bonus_msgNew = malloc(sizeof(char) * 8);
    
    memset(bonus_msgNew, '\0', 8);
    int randomCoonf = random % 30;
    
    if (randomCoonf <= 18) {
        if (randomCoonf >= 0 && randomCoonf <= 7) {
            randomCoonf = 0;
        }
        else if (randomCoonf <= 11) {
            randomCoonf = 1;
        }
        else if (randomCoonf <= 14) {
            randomCoonf = 2;
        }
        else if (randomCoonf <= 15) {
            randomCoonf = 3;
        }
        else if (randomCoonf <= 18) {
            randomCoonf = 4;
        }
        for (int x = 0; x < 16; x++) 
        {
            if (game->bonus->bonusesDT[x]->bonusType == -1) 
            {
                game->bonus->bonusesDT[x]->bonusType = randomCoonf;
                game->bonus->bonusesDT[x]->bonusCasePosition.x = positionX;
                game->bonus->bonusesDT[x]->bonusCasePosition.y = positionY;
                game->bonus->bonusesDT[x]->bonusSpRectPosition.x = 0;
                game->bonus->bonusesDT[x]->bonusSpRectPosition.y = 0;
                game->bonus->bonusesDT[x]->bonusSpRectPosition.h = 64;
                game->bonus->bonusesDT[x]->bonusSpRectPosition.w = 64;
                
                game->map[positionY][positionX]->bonusCase = x;
                bonus_msgNew[0] = x + '0';
                bonus_msgNew[1] = randomCoonf + '0';
                bonus_msgNew[2] = positionX + '0';
                bonus_msgNew[3] = positionY + '0';
                
                printf("le x ===> %d\n", x);
                x = 16; //pour sortir de la boucle, possibilité de faire un while aussi mais j'ai procédé comme ça
            }
        }
        strcat(msg, bonus_msgNew); //on ajoute donc les données à notre meassage
    }
}

int server_bonus_taking(gameS *game, int no_player, char *msg)
{
    if (game->map[playerY][playerX]->bonusCase != -1) 
    {
        int no_bonus = game->bonus->bonusesDT[game->map[playerY][playerX]->bonusCase]->bonusType;
        msg[3] = game->map[playerY][playerX]->bonusCase + '0';
        msg[4] = game->bonus->bonusesDT[game->map[playerY][playerX]->bonusCase]->bonusType + '0';
        game->bonus->bonusesDT[game->map[playerY][playerX]->bonusCase]->bonusType = -1;
        if (no_bonus == 0) 
        {
            if (game->players[no_player]->pStats->nbBombs >= 15)
                game->players[no_player]->pStats->nbBombs = 20;
            else
                game->players[no_player]->pStats->nbBombs += 5;
            msg[5] = game->players[no_player]->pStats->nbBombs + '0';
        }
        else if (no_bonus == 1) 
        {
            if (game->players[no_player]->pStats->playerSpeed > 150)
                game->players[no_player]->pStats->playerSpeed -= 50;
            msg[5] = (game->players[no_player]->pStats->playerSpeed / 50) + '0';
        }
        else if (no_bonus == 2) 
        {
            if (game->players[no_player]->pStats->radBombs < 9)
                game->players[no_player]->pStats->radBombs += 2;
            msg[5] = game->players[no_player]->pStats->radBombs + '0';
        }
        else if (no_bonus == 3) 
        {
            if (game->players[no_player]->pStats->nbLife < 2)
                game->players[no_player]->pStats->nbLife++;
            msg[5] = game->players[no_player]->pStats->nbLife + '0';
        }
        else if (no_bonus == 4) 
        {
            if (game->players[no_player]->pStats->nbPlantedBombs < 4)
                game->players[no_player]->pStats->nbPlantedBombs++;
            msg[5] = game->players[no_player]->pStats->nbPlantedBombs + '0';
        }
        game->map[playerY][playerX]->bonusCase = -1;
    }
}

void mapToMessage(gameS *game, char *message)
{
    message[0] = '(';
    int k = 1; // variable pour marchÃ© 3 cases a la fois
    for (int i = 0; i < game->nbY; i++) {
        for (int j = 0; j < game->nbX; j++) {
            if (game->map[i][j]->caseType == 3) {
                message[k] = 3 + '0';
                message[k + 1] = i + '0';
                message[k + 2] = j + '0';
                k += 3;
            }
        }
    }
}

int bonusTomsg(gameS *game, char *msg)
{
    msg[0] = ')';
    int k = 1; // variable pour marchÃ© 3 cases a la fois
    for (int i = 0; i < 16; i++) {
        if (game->bonus->bonusesDT[i]->bonusType != -1) {
            msg[k] = i + '0';
            msg[k + 1] = game->bonus->bonusesDT[i]->bonusType + '0';
            msg[k + 2] = game->bonus->bonusesDT[i]->bonusCasePosition.x + '0';
            msg[k + 3] = game->bonus->bonusesDT[i]->bonusCasePosition.y + '0';
            k += 4;
        }
    }
}

int playerNumberTomsg(gameS *game, char *msg)
{
    msg[0] = '%';
    for (int i = 0; i < 4; i++) {
        if (game->serverSockets->clients[i] >= 0) {
            msg[i + 1] = 1 + '0';
        } else {
            msg[i + 1] = 0 + '0';
        }
    }
    msg[5] = '\'';
    int j = 6;
    for (int k = 0; k < 4; k++) {
        if (game->serverSockets->clients[k] >= 0) {
            msg[j] = game->players[k]->playerSquarePosition.x + '0';
            msg[j + 1] = game->players[k]->playerSquarePosition.y + '0';
            msg[j + 2] = game->players[k]->pStats->nbLife + '0';
            msg[j + 3] = (game->players[k]->pStats->playerSpeed / 10) + '0';
            msg[j + 4] = game->players[k]->pStats->nbBombs + '0';
            msg[j + 5] = game->players[k]->pStats->nbPlantedBombs + '0';
        } else {
            msg[j] = 'p';
            msg[j + 1] = 'p';
            msg[j + 2] = 'p';
            msg[j + 3] = 'p';
            msg[j + 4] = 'p';
            msg[j + 5] = 'p';
        }
        j += 6;
    }
}

int playerStatsTomsg(gameS *game, char *msg)
{
    msg[0] = '*';
    int j = 1;
    for (int i = 0; i < 4; i++) {
        msg[j] = i + '0';
        msg[j + 1] = game->players[i]->pStats->nbLife + '0';
        msg[j + 2] = (game->players[i]->pStats->playerSpeed / 50) + '0';
        msg[j + 3] = game->players[i]->pStats->nbBombs + '0';
        msg[j + 4] = game->players[i]->pStats->nbPlantedBombs + '0';
        msg[j + 5] = game->players[i]->pStats->radBombs + '0';
        j += 6;
    }
}

int concat_Stat(gameS *game, char *msg)
{
    char *msgMap = malloc(sizeof(char) * 255);
    char *msgBonuses = malloc(sizeof(char) * 128);
    char *msgPlayers = malloc(sizeof(char) * 64);
    char *msgStats = malloc(sizeof(char) * 64);
    memset(msgMap, '\0', 255);
    memset(msgBonuses, '\0', 128);
    memset(msgPlayers, '\0', 64);
    memset(msgStats, '\0', 64);
    mapToMessage(game, msgMap);
    bonusTomsg(game, msgBonuses);
    playerNumberTomsg(game, msgPlayers);
    playerStatsTomsg(game, msgStats);
    strcat(msgMap, msgBonuses);
    strcat(msgMap, msgPlayers);
    strcat(msgMap, msgStats);
    strcat(msg, msgMap);
}
