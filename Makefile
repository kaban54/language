CFLAGS += -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE
CC = g++

all: front back asm proc run


back: obj/back.o obj/backmain.o obj/tree.o obj/treedump.o obj/stack.o obj/front.o obj/gram.o
	$(CC) -o back.exe obj/backmain.o obj/back.o obj/tree.o obj/treedump.o obj/stack.o obj/front.o obj/gram.o $(CFLAGS)

obj/backmain.o: backmain.cpp
	$(CC) -o obj/backmain.o backmain.cpp -c $(CFLAGS)

obj/back.o: back.cpp
	$(CC) -o obj/back.o back.cpp -c $(CFLAGS)

front: obj/frontmain.o obj/front.o obj/tree.o obj/treedump.o obj/stack.o obj/gram.o
	$(CC) -o front.exe obj/frontmain.o obj/front.o obj/tree.o obj/treedump.o obj/stack.o obj/gram.o $(CFLAGS)

obj/frontmain.o: frontmain.cpp
	$(CC) -o obj/frontmain.o frontmain.cpp -c $(CFLAGS)

obj/front.o: front.cpp
	$(CC) -o obj/front.o front.cpp -c $(CFLAGS)

obj/tree.o: tree/tree.cpp
	$(CC) -o obj/tree.o tree/tree.cpp -c $(CFLAGS)

obj/treedump.o: tree/treedump.cpp
	$(CC) -o obj/treedump.o tree/treedump.cpp -c $(CFLAGS)

obj/stack.o: stack/stack.cpp
	$(CC) -o obj/stack.o stack/stack.cpp -c $(CFLAGS)

obj/gram.o: gram.cpp
	$(CC) -o obj/gram.o gram.cpp -c $(CFLAGS)

asm: obj/asm.o obj/txtfuncs.o
	$(CC) -o asm.exe obj/asm.o obj/txtfuncs.o $(CFLAGS)

proc: obj/proc.o obj/stack.o
	$(CC) -o proc.exe obj/proc.o obj/stack.o $(CFLAGS)

obj/asm.o: proc/asm.cpp
	$(CC) -o obj/asm.o proc/asm.cpp -c $(CFLAGS)

obj/txtfuncs.o: proc/txtfuncs.cpp 
	$(CC) -o obj/txtfuncs.o proc/txtfuncs.cpp -c $(CFLAGS)

obj/proc.o: proc/proc.cpp 
	$(CC) -o obj/proc.o proc/proc.cpp -c $(CFLAGS)

run: proc/run.cpp
	$(CC) -o run.exe proc/run.cpp $(CFLAGS)

compile: compile.cpp
	$(CC) -o compile.exe compile.cpp $(CFLAGS)

clean:
	rm *.o
	clear
	
.PHONY: clean