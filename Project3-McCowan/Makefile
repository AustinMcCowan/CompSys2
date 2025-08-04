# This is (at least the start of) a makefile for CSC45500, 
#   project#2
## NOTE: any LINE starting with a '#' in a Makefile is a comment

# Edits I have made to this makefile are probably suboptimal. Apologies.

#OPTIM=-O
DBG=-g
OPTS=$(DBG) $(OPTIM) -Wall -ansi -pedantic -std=c++20

CC=g++ $(OPTS)  -c
LNK=g++

OBJS=BlockNode.o BlockList.o Token.o main.o
GLBL=Makefile Token.hpp BlockList.hpp BlockNode.hpp

garbo: $(OBJS)
	$(LNK) -o garbo $(OBJS)

main.o: main.cpp Token.hpp BlockList.hpp $(GLBL)
	$(CC) main.cpp

BlockList.o: BlockList.cpp BlockList.hpp BlockNode.hpp $(GLBL)
	$(CC) BlockList.cpp

BlockNode.o: BlockNode.cpp BlockNode.hpp $(GLBL)
	$(CC) BlockNode.cpp

Token.o: Token.cpp Token.hpp $(GLBL)
	$(CC) Token.cpp

clean:
	/bin/rm -f *.o garbo *~
