  #include "../../include/main.h"

int main(int argc, char const *argv[])
{
    //Init the game and set the defaut value for the main parts of it
    gameS *game = gameInit();
    //Values of continueMenu if === 1 : main Menu, if == 4 : create Game menu, if == 3 : Join game Menu
    int continueMenu = 1;
    int continueGame = 1;
    int correct_input = 1;
    int lengthInfos = 0;
    int continueServer;
    Uint32 secondsMenu = SDL_GetTicks();

    //While on main Menu
    while (continueMenu) {
        //sets the menuSdl with the game and the seconds elapsed since the SDL library initialization
        menuSdl(game, &secondsMenu);
        //gets the value of the menu selected
        continueMenu = eventMenu(game);
        correct_input = 1;

        // initialize port number and ip adress
        memset(game->infos->port, '\0', 8);
        memset(game->infos->ipAdress, '\0', 24);
        game->infos->port[0] = ' ';
        game->infos->ipAdress[0] = ' ';

        // 4 = Create Party Menu
        if (continueMenu == 4) {
          //To allow the text input event
            SDL_StartTextInput();
            //To affect the new input value to the port
            game->menu->inputTexture = textChange(game, game->infos->port, 24);
            //To query the attributes of the new port value
            SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
            //loop over while the port input is not modified
            do
            {
                menuSdl(game, &secondsMenu);
                game->menu->inputSelected = inputPortEvent(game->infos->port, game, 7);
                game->menu->inputTexture = changeTextSize(game, game->infos->port);
                SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
                //SDL_Delay(80);
            } while (game->menu->inputSelected);
            //Same as before but this time for the ipAdress
            game->menu->menuSelected = 4;
            game->menu->inputTexture = textChange(game, game->infos->ipAdress, 20);
            SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
            do
            {
                menuSdl(game, &secondsMenu);
                game->menu->inputSelected = inputPortEvent(game->infos->ipAdress, game, 17);
                game->menu->inputTexture = changeTextSize(game, game->infos->ipAdress);
                SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
                //SDL_Delay(80);
            } while (game->menu->inputSelected);
            //If no port specified, affects default 5000 port
            if (game->infos->port[1] == '\0') {
                memset(game->infos->port, '\0', 8);
                strcpy(game->infos->port, ":5000");
            }
            //Tests to see if the entred port is not a dot (.) because only numbers and dots are allowed
            else
            {
                for (int i = 1; i < strlen(game->infos->port); i++) {
                  //if there's a dot value change correct_input value to 0
                  if (game->infos->port[i] == 46)
                    {
                        correct_input = 0;
                    }
                }
            }
            //If no port specified, affects defaut 127.0.0.1
            if (game->infos->ipAdress[1] == '\0') {
                memset(game->infos->ipAdress, '\0', 24);
                strcpy(game->infos->ipAdress, ":127.0.0.1");
            }
            //Tests to see if the ip adress has a correct format (only 3 dots (.) and not 2 dots that follows each other)
            else
            {
                for (int i = 1; i < strlen(game->infos->ipAdress); i++) {
                    if (game->infos->ipAdress[i] == 46)
                    {
                        if (game->infos->ipAdress[i + 1] == 46)
                            correct_input = 0;
                        lengthInfos++;
                    }
                }
                //if ip adress has a wrong format
                if (lengthInfos != 3) {
                    correct_input = 0;
                    puts("Wrong ip input");
                    printf("ip : %s\n", game->infos->ipAdress);
                  }
            }
            //if the format entred is not correct
            if (!correct_input) {
                puts("Wrong port input or ip input");
                game->menu->inputSelected = -1;
            }
            else
            {
                //Stops the text input and starts the map choosing
                SDL_StopTextInput();
                game->menu->menuSelected = 5;
                int chooseMap = 1;
                while (chooseMap) {
                    menuSdl(game, &secondsMenu);
                    chooseMap = selectMapEvent(game);
                }
                printf("--map number : %d\n\n", game->mapFeatures->mapId);
                //stores the value of the port (there's always a minus 1) in portInt as int to be used later on the client datas
                game->infos->portInt = atoi(game->infos->port + 1);

                // creating server thread and connect to it with a client
                pthread_t server_thread;
                if (pthread_create(&server_thread, NULL, _server, (void *)game) < 0) {
                    perror("Server thread creation problem");
                    return 1;
                }
                puts("Server thread created");
                game = mapInitServer(game->nbX, game->nbY, game);
                puts("New map initialised");

                // general loop after server start
                do
                {
                    continueServer = evnt_server_inside();
                    menuServer_Sdl(game);
                } while (continueServer);

                // terminating server thread
                if (pthread_detach(server_thread) != 0)
                    puts("problem terminating server thread!");
                else
                    puts("terminated server thread succesfully!");
            }
            game->menu->menuSelected = 1;
            game->menu->inputSelected = 0;
            // 3 = client
        }
        //Join game menu
        else if (continueMenu == 3) {
          //Same as continueMenu == 4
            SDL_StartTextInput();
            game->menu->inputTexture = textChange(game, game->infos->port, 20);
            SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
            do
            {
                menuSdl(game, &secondsMenu);
                game->menu->inputSelected = inputPortEvent(game->infos->port, game, 7);
                game->menu->inputTexture = changeTextSize(game, game->infos->port);
                SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
                //SDL_Delay(80);
            } while (game->menu->inputSelected);
            game->menu->menuSelected = 4;
            game->menu->inputTexture = textChange(game, game->infos->ipAdress, 20);
            SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
            do
            {
                menuSdl(game, &secondsMenu);
                game->menu->inputSelected = inputPortEvent(game->infos->ipAdress, game, 17);
                game->menu->inputTexture = changeTextSize(game, game->infos->ipAdress);
                SDL_QueryTexture(game->menu->inputTexture, NULL, NULL, &game->menu->inputPosition.w, &game->menu->inputPosition.h);
            } while (game->menu->inputSelected);
            if (game->infos->port[1] == '\0') { // gestion du default
                memset(game->infos->port, '\0', 8);
                strcpy(game->infos->port, ":5000");
            }
            else
            {
                for (int i = 1; i < strlen(game->infos->port); i++) {
                    if (game->infos->port[i] == 46)
                        correct_input = 0;
                }
            }
            if (game->infos->ipAdress[1] == '\0') { // gestion du default
                memset(game->infos->ipAdress, '\0', 24);
                strcpy(game->infos->ipAdress, ":127.0.0.1");
            }
            else
            {
                for (int i = 1; i < strlen(game->infos->ipAdress); i++) {
                    if (game->infos->ipAdress[i] == 46)
                    {
                        if (game->infos->ipAdress[i + 1] == 46)
                            correct_input = 0;
                        lengthInfos++;
                    }
                }
                if (lengthInfos != 3) {
                    correct_input = 0;
                    puts("Wrong ip input");
                    printf("ip : %s\n", game->infos->ipAdress);
                    }
            }
            if (correct_input) {
                SDL_StopTextInput();
                game->infos->portInt = atoi(game->infos->port + 1);
                game->infos->ipAdress;
                _client(game->infos->ipAdress + 1, game->infos->portInt, game);
            }
            else
            {
                puts("Wrong port input or ip input");
                game->menu->inputSelected = -1;
            }
            game->menu->menuSelected = 1;
            game->menu->inputSelected = 0;
            lengthInfos = 0;
        }
    }
    gameDestroy(game);
    return 0;
}
