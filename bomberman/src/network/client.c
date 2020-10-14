#include "../../include/main.h"

int _client(char *ip, int port, gameS *game)
{
    struct sockaddr_in adr_server;
    char *msg = malloc(sizeof(char) * 512);
    
    int players_here[4] = {0, 0, 0, 0};
    int player_no;
    int player_move_num;
    
    int special_comm;
    int dir;
    
    int move_no_backup = 0;
    int move_no = 0;
    int anim_tab[16][4];
    
    Uint32 secondsAnimations[16];
    Uint32 secondsAnimationsBonuses = SDL_GetTicks();

    
    adr_server.sin_family = AF_INET;
    adr_server.sin_addr.s_addr = inet_addr(ip);
    adr_server.sin_port = htons(port);

    int _socket = socket(AF_INET, SOCK_STREAM, 0);
    
    if (connect(_socket, (struct sockaddr *)&adr_server, sizeof(adr_server)) < 0) {    
        perror("CLIENTTT connect()");
        game->menu->inputSelected = -2;
        return 1;
    }
    if (_socket < 0) {
        perror("CLIENTTT socket()");
        return -1;
    }
    

    // reception no player et des players here (présents)
    
    memset(msg, '\0', 512);
    read(_socket, msg, 16);
    
    printf("CLIENTTT--player no : %s\n\n", msg);
    player_no = msg[0] - 48;
    for (int i = 0; i < 4; i++)
        players_here[i] = msg[i + 1] - 48;

    printf("CLIENTTT--map no : %c\n\n", msg[5]);
    game->mapFeatures->mapId = msg[5] - 48;
    for (int j = 0; j < 16; j++)
        anim_tab[j][0] = -1;
    
    memset(msg, '\0', 32);
    
    int continue_jeu;

    Uint32 sec = SDL_GetTicks();
    
    do{
        gameSdl(game, game->nbX, game->nbY, players_here);
        continue_jeu = client_eventmsgSend(game, &sec, player_no, msg);


        if (msg[0] != '\0')
        {
            send(_socket, msg, strlen(msg), MSG_NOSIGNAL);
            memset(msg, '\0', 32);
        }

        recv(_socket, msg, 512, MSG_DONTWAIT);
        
        if (msg[0] != '\0'){
            printf("CLIENTTT---- reçu =>> %s\n", msg);
            move_no_backup = move_no;
            move_no = client_eventmsgReceive(game, _socket, &sec, msg, players_here, move_no, &player_move_num, &dir, &special_comm);

            if (move_no_backup < move_no){
                for (int i = 0; i < 16; i++) {
                    if (anim_tab[i][0] == -1){
                        anim_tab[i][0] = player_move_num;
                        anim_tab[i][1] = dir;
                        anim_tab[i][2] = 0;
                        anim_tab[i][3] = special_comm;
                        secondsAnimations[i] = SDL_GetTicks();
                        i = 16; //pour sortir de la boucle
                    }
                }
            }else
                move_no = move_no_backup;
        }

        animations(game, anim_tab, secondsAnimations);
        bonusAnimations(game, &secondsAnimationsBonuses);
        memset(msg, '\0', 512);
    } while (continue_jeu);

    close(_socket);
    free(msg);
    if (pthread_detach(pthread_self()) != 0)
        puts("thread problem! iencli");
    else
        puts("thread finish iencli good!");
    return 0;
}
