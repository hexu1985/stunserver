include ../common.inc

VPATH := ../export ../common ../networkutils ../stuncore
PROJECT_TARGET := libexport_aio.a
PROJECT_OBJS := stunclient_c.o logger.o socketaddress.o stringhelper.o getconsolewidth.o resolvehostname.o \
				stunsocket.o buffer.o stunclientlogic.o stunclienttests.o \
				stunbuilder.o datastream.o stunutils.o stunreader.o getmillisecondcounter.o \
				recvfromex.o prettyprint.o cmdlineparser.o atomichelpers.o
INCLUDES := $(BOOST_INCLUDE) -I../common -I../stuncore -I../networkutils -I../resources -I../export


.PHONY: all
.PHONY: clean


all: $(PROJECT_TARGET) stunclient

clean:
	rm -f $(PROJECT_OBJS) $(PROJECT_TARGET) *.o stunclient

$(PROJECT_TARGET): $(PROJECT_OBJS)
	rm -f $@
	$(AR) rv $@ $^ 

stunclient: clientmain.o sock_ntop.o $(PROJECT_TARGET) 
	$(CXX) -o $@ $^ -lws2_32 -lcrypto -lstdc++


