
CC = gcc
#CFLAGS = -O
CFLAGS = -g -Wall -lpthread

LD = gcc
LDFLAGS = 

RM = /bin/rm -f

PROG = gridapp

#############################################
all: $(PROG)

gridapp: gridapp_no_flags.c
	$(CC) -o gridapp gridapp_no_flags.c $(CFLAGS)

clean:
	$(RM) $(PROG) *.o core *~











