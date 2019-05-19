CC=g++
CFLAGS=-std=c++17 -Wall -Iinclude
# -lglfw -lGLEW -lGLU -lGL -lSOIL -lOpenCL -O2
OBJDIR=bin
SRCDIR=src

OBJS= $(addprefix $(OBJDIR)/, \
	GameState.o \
	UDPSocket.o \
	)
#	main_client.o	\
#	main_server.o	\

all: $(OBJS)
	$(CC) $(OBJS) $(OBJDIR)/main_server.o -o $(OBJDIR)/main_server $(CFLAGS)
	$(CC) $(OBJS) $(OBJDIR)/main_client.o -o $(OBJDIR)/main_client $(CFLAGS)

$(OBJDIR)/main_server.o: create_bin $(SRCDIR)/main_server.cpp $(OBJDIR)/UDPSocket.o
	g++ -c $(SRCDIR)/main_server.cpp -o $(OBJDIR)/main_server.o -Iinclude

$(OBJDIR)/main_client.o: create_bin $(SRCDIR)/main_client.cpp $(OBJDIR)/UDPSocket.o
	g++ -c $(SRCDIR)/main_client.cpp -o $(OBJDIR)/main_client.o -Iinclude

$(OBJDIR)/GameState.o: create_bin include/GameState.hpp $(SRCDIR)/GameState.cpp
	g++ -c $(SRCDIR)/GameState.cpp -o $(OBJDIR)/GameState.o $(CFLAGS)

$(OBJDIR)/UDPSocket.o: create_bin include/UDPSocket.hpp $(SRCDIR)/UDPSocket.cpp
	g++ -c $(SRCDIR)/UDPSocket.cpp -o $(OBJDIR)/UDPSocket.o $(CFLAGS)

create_bin:
	[ -d $(DIRBIN) ] || mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	rm $(OBJDIR)/*
