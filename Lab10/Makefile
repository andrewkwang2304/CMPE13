SOURCES   = rpg.c Game.c Player.c
OBJECTS   = rpg.o Game.o Player.o
HEADERS   = Game.h Player.h UnixBOARD.h
EXEBIN    = rpg
FLAGS     = -std=c99 -Wall

all: $(EXEBIN)

$(EXEBIN) : $(OBJECTS) $(HEADERS)
	gcc -o $(EXEBIN) $(OBJECTS)

$(OBJECTS) : $(SOURCES) $(HEADERS)
	gcc -c $(FLAGS) $(SOURCES)

clean :
	rm -f $(EXEBIN) $(OBJECTS)

check:
	valgrind --leak-check=full $(EXEBIN)
