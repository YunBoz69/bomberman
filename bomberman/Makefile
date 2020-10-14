CC 					= gcc
CFLAGS 				= -Wall -Wextra -Werror
THREADS 			= -pthread
FINAL_TARGET_SDL 	= bomberman
RM 					= rm -rf
SRC 				= $(wildcard ./src/core/*.c ./src/network/*.c)
OBJ 				= $(SRC:.c=.o)
LIB 				=  -lSDL2 -lSDL2_image -lSDL2_ttf  
BIN 				= bin
INCLUDE_DIR			= -Isrc -Isrc/core -Isrc/network

default: $(FINAL_TARGET_SDL)

all : $(FINAL_TARGET_SDL)

%.o : %.c
	$(CC) $(INCLUDE_DIR) -o $@ -c $<

$(FINAL_TARGET_SDL) : $(OBJ)
	$(CC) $(CFLAGS) $(THREADS) -o $@ $^ $(LIB)

clean :
	$(RM) ./src/core/*.o
	$(RM) ./src/network/*.o
	$(RM) $(FINAL_TARGET_SDL)

fclean : clean
	$(RM) $(FINAL_TARGET_SDL)

.PHONY: fclean all