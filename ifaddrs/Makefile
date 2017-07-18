include ../common.inc.android

PROJECT_TARGET := libifaddrs.a
PROJECT_OBJS := ifaddrs.o
INCLUDES :=


.PHONY: all
.PHONY: clean


all: $(PROJECT_TARGET)

clean:
	rm -f $(PROJECT_OBJS) $(PROJECT_TARGET)

$(PROJECT_TARGET): $(PROJECT_OBJS)
	rm -f $@
	$(AR) rv $@ $^ 




