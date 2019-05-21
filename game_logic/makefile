CC=g++
CFLAGS=-std=c++17 -Wall -Iinclude -pthread
# -lglfw -lGLEW -lGLU -lGL -lSOIL -lOpenCL -O2
OBJDIR=bin
SRCDIR=src

OBJS= $(addprefix $(OBJDIR)/, \
	GameState.o \
	UDPSocket.o \
	)
MAIN_CLIENT_O=$(OBJDIR)/main_client.o
MAIN_SERVER_O=$(OBJDIR)/main_server.o

all: $(OBJS) $(MAIN_SERVER_O) $(MAIN_CLIENT_O)
	$(CC) $(OBJS) $(MAIN_SERVER_O) -o $(OBJDIR)/main_server $(CFLAGS)
	$(CC) $(OBJS) $(MAIN_CLIENT_O) -o $(OBJDIR)/main_client $(CFLAGS)

$(MAIN_SERVER_O): create_bin $(SRCDIR)/main_server.cpp $(OBJDIR)/UDPSocket.o
	g++ -c $(SRCDIR)/main_server.cpp -o $(MAIN_SERVER_O) -Iinclude

$(MAIN_CLIENT_O): create_bin $(SRCDIR)/main_client.cpp $(OBJDIR)/UDPSocket.o
	g++ -c $(SRCDIR)/main_client.cpp -o $(MAIN_CLIENT_O) -Iinclude

$(OBJDIR)/GameState.o: create_bin include/GameState.hpp $(SRCDIR)/GameState.cpp
	g++ -c $(SRCDIR)/GameState.cpp -o $(OBJDIR)/GameState.o $(CFLAGS)

$(OBJDIR)/UDPSocket.o: create_bin include/UDPSocket.hpp $(SRCDIR)/UDPSocket.cpp
	g++ -c $(SRCDIR)/UDPSocket.cpp -o $(OBJDIR)/UDPSocket.o $(CFLAGS)

create_bin:
	[ -d $(DIRBIN) ] || mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	rm $(OBJDIR)/*
