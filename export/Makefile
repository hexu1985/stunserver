include ../common.inc

PROJECT_TARGET := libexport.a
PROJECT_OBJS := stunclient_c.o
INCLUDES := $(BOOST_INCLUDE) -I../common -I../stuncore -I../networkutils -I../resources


.PHONY: all
.PHONY: clean


all: $(PROJECT_TARGET)

clean:
	rm -f $(PROJECT_OBJS) $(PROJECT_TARGET)

$(PROJECT_TARGET): $(PROJECT_OBJS)
	rm -f $@
	$(AR) rv $@ $^ 




