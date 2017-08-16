include ../common.inc

VPATH := ../client ../common ../networkutils ../stuncore
PROJECT_TARGET := stunclient
PROJECT_OBJS := clientmain.o logger.o socketaddress.o stringhelper.o getconsolewidth.o resolvehostname.o \
				stunsocket.o buffer.o stunclientlogic.o stunclienttests.o recvfromex.o \
				stunbuilder.o datastream.o stunutils.o stunreader.o getmillisecondcounter.o \
				prettyprint.o cmdlineparser.o atomichelpers.o
#	adapters.o 

INCLUDES := $(BOOST_INCLUDE) -I../common -I../stuncore -I../networkutils -I../resources
LIB_PATH := -L../common -L../stuncore -L../networkutils
LIBS := -lws2_32 #-lnetworkutils -lstuncore -lcommon 
#-lwsock32 

all: $(PROJECT_TARGET)

clean:
	rm -f $(PROJECT_OBJS) $(PROJECT_TARGET) 

$(PROJECT_TARGET): $(PROJECT_OBJS)
	$(LINK.cpp) -o $@ $^ $(LIB_PATH) $(LIBS) $(SOCKET_LIBS) $(CRYPTO_LIBS)


