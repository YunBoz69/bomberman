#include "../../include/main.h"

void *handle_connection(void *gameVoid)
{
    gameS *game = (gameS *)gameVoid;
    
    int socket_no = game->serverSockets->synchNbSockets;
    int _socket = game->serverSockets->clients[socket_no];

    int msg_int;
    int players_here[4] = {0, 0, 0, 0};
    int ret;
    
    
    Uint32 sec = SDL_GetTicks();

    srand(time(NULL)); // initialise temps

    for (int x = 0; x < 4; x++)
        if (game->serverSockets->clients[x] >= 0)
            players_here[x] = 1;

    
    char *msg = malloc(sizeof(char) * 16); // on va envoyer num joueur et joueur
    memset(msg, '\0', 16);
    
    sprintf(msg, "%d%d%d%d%d%d", socket_no, players_here[0], players_here[1], players_here[2], players_here[3], game->mapFeatures->mapId);
    printf("SERVER --map number : %d\n\n", game->mapFeatures->mapId);
    write(_socket, msg, 16);
    
    memset(msg, '\0', 16);

    char *command = malloc(sizeof(char) * 512);
    memset(command, '\0', 512);
    
    concat_Stat(game, command);
    write(_socket, command, 512);
    free(command);

    while ((ret = recv(_socket, msg, 16, MSG_DONTWAIT)) != 0) {// on a replacé read par recv parce que ça devient non bloquant avec le flag
        if (game->players[socket_no]->pStats->nbLife){
            if (msg[0] != '\0'){
                msg_int = atoi(msg);
                server_event(game, &sec, msg_int,  socket_no);
            }
        }
        bombe_explosion_duree(game, socket_no);
        memset(msg, '\0', 16);
    }

    if (ret == 0)
        puts("SERVER Client disconnected");
    else if (ret == -1){
        puts(" SERVER Failed message reception");
        perror("read()");
    }

    // reinit socket/reset socket dans le tab de socket
    game->serverSockets->clients[socket_no] = -1;

    if (pthread_detach(pthread_self()) != 0)
            puts("problem terminating thread!");
    else{
        puts("terminated thread succesfully!");
        game->serverSockets->maxSockets--;
    }
}

void *_server(void *gameVoid)
{
    gameS *game = (gameS *)gameVoid;
    
    int port = game->infos->portInt;
    
    int connect_to_server;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;

    int iterator = 0;

    server_addr.sin_addr.s_addr = inet_addr(game->infos->ipAdress + 1);
    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;

    int _socket = socket(AF_INET, SOCK_STREAM, 0);
    int client_socket;
    char msg[16];

    if (_socket < 0)
        perror(" SERVER socket()");
    if (bind(_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        perror(" SERVER bind()");
    if (listen(_socket, 5) < 0)
        perror("SERVER listen()");
    memset(msg, '\0', 16);
    while ((client_socket = accept(_socket, (struct sockaddr *)&client_addr, &client_addr_len))){
        if (game->serverSockets->maxSockets < 4){
            iterator = 0;
            while (game->serverSockets->clients[iterator] >= 0){
                iterator++;
            }
            game->serverSockets->clients[iterator] = client_socket;
            game->serverSockets->synchNbSockets = iterator;

            puts("SERVER new iencli");
            pthread_t _thread;

            if (pthread_create(&_thread, NULL, handle_connection, (void *)game) < 0)
                perror(" SERVER problem thread (creation)");
            else {
                puts(" SERVER thread CREATEDDDD");
                game->serverSockets->maxSockets++;
            }
        } else {
            close(client_socket); //
            puts("SERVER Beaucoup trop de client, on close la socket");
        }
    }
    close(_socket);
}
