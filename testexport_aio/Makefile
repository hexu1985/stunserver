RM = rm -f
CC = gcc
INCLUDE = -I../export
CFLAGS = -Wall -g -O2 ${INCLUDE}
LIB_PATH := -L../export_aio
LIBS := -lexport -lstdc++ -lcrypto

VPATH = ../testexport
PROGS =	stunclient_c

all:	${PROGS}

stunclient_c:	clientmain.o sock_ntop.o
		${CC} ${CFLAGS} -o $@ clientmain.o sock_ntop.o ${LIB_PATH} ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES} *.o
