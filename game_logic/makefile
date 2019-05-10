DIRBIN=bin

main: create_bin main.c
	gcc -o $(DIRBIN)/main main.c

create_bin:
	[ -d $(DIRBIN) ] || mkdir -p $(DIRBIN)
