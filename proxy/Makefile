include ../common.inc

PROJECT_TARGET := stunclient
PROJECT_OBJS := clientmain.o json_util.o sock_ntop.o sock_ntop_host.o sock_get_port.o
	

#rapidjson: cd .. && git clone https://github.com/miloyip/rapidjson.git
INCLUDES := $(BOOST_INCLUDE) -I../common -I../stuncore -I../networkutils -I../resources -I../export -I../rapidjson/include
LIB_PATH := -L../common -L../stuncore -L../networkutils -L../export
LIBS := -lexport -lnetworkutils -lstuncore -lcommon 


all: $(PROJECT_TARGET)

clean:
	rm -f $(PROJECT_OBJS) $(PROJECT_TARGET) 

stunclient: clientmain.o json_util.o sock_ntop.o sock_ntop_host.o sock_get_port.o
	$(LINK.cpp) -o $@ $^ $(LIB_PATH) $(LIBS) $(SOCKET_LIBS) $(CRYPTO_LIBS)


