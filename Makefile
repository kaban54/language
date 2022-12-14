CFLAGS += -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE
CC = g++

all: front


front: frontmain.o front.o tree.o treedump.o stack.o
	$(CC) -o front.exe obj/frontmain.o obj/front.o obj/tree.o obj/treedump.o obj/stack.o $(CFLAGS)

frontmain.o: frontmain.cpp
	$(CC) -o obj/frontmain.o frontmain.cpp -c $(CFLAGS)

front.o: front.cpp
	$(CC) -o obj/front.o front.cpp -c $(CFLAGS)

tree.o: tree/tree.cpp 
	$(CC) -o obj/tree.o tree/tree.cpp -c $(CFLAGS)

treedump.o: tree/treedump.cpp 
	$(CC) -o obj/treedump.o tree/treedump.cpp -c $(CFLAGS)

stack.o: stack/stack.cpp 
	$(CC) -o obj/stack.o stack/stack.cpp -c $(CFLAGS)

clean:
	rm *.o
	clear
	
.PHONY: clean