include ../common.inc

DEFINES := -DDEBUG -DUSE_WINSOCK #-DNDEBUG

PROJECT_TARGET := stunproxyclient
PROJECT_OBJS := json_util.o sock_ntop.o sock_ntop_host.o sock_get_port.o readline.o writen.o proxy_client.o inet_ntop.o inet_pton.o stunclient_c_stub.o


#rapidjson: cd .. && git clone https://github.com/miloyip/rapidjson.git
INCLUDES := $(BOOST_INCLUDE) -I../common -I../stuncore -I../networkutils -I../resources -I../export -I../rapidjson/include
LIB_PATH := -L../export
LIBS := -lwsock32


all: $(PROJECT_TARGET)

clean:
	rm -f $(PROJECT_OBJS) $(PROJECT_TARGET) 

stunproxyclient: json_util.o sock_ntop.o sock_ntop_host.o sock_get_port.o readline.o writen.o clientmain.o inet_ntop.o inet_pton.o stunclient_c_stub.o
	$(LINK.cpp) -o $@ $^ $(LIB_PATH) $(LIBS) $(SOCKET_LIBS) $(CRYPTO_LIBS)


