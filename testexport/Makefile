RM = rm -f
CC = gcc
INCLUDE = -I../export
CFLAGS = -Wall -g -O2 ${INCLUDE}
LIB_PATH := -L../common -L../stuncore -L../networkutils -L../export
LIBS := -lexport -lnetworkutils -lstuncore -lcommon -lstdc++ -lcrypto

PROGS =	stunclient_c

all:	${PROGS}

stunclient_c:	clientmain.o sock_ntop.o
		${CC} ${CFLAGS} -o $@ clientmain.o sock_ntop.o ${LIB_PATH} ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES} *.o
