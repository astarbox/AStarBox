# Makefile for astarbox_config and astarbox_port

CC = g++
CFLAGS = -fPIC -Wall -Wextra -O2 -g -I. -I./..
CPPFLAGS = -fPIC -Wall -Wextra -O2 -g -I. -I./..
LDFLAGS = -lstdc++ -lm -lpthread -lrt -lcrypt
RM = rm -f
STRIP = strip
TARGET_INIT = astarbox_config
TARGET_CLI = astarbox_port
TARGET_MCP = astarbox_volt

SRCS_INIT = astarbox_config.cpp ../PCA9685.cpp
SRCS_CLI = astarbox_port.cpp ../PCA9685.cpp
SRCS_MCP = mcp3421.cpp

OBJS_INIT = $(SRCS_INIT:.cpp=.o)
OBJS_CLI = $(SRCS_CLI:.cpp=.o)
OBJS_MCP = $(SRCS_MCP:.cpp=.o)

.PHONY: all
all: ${TARGET_INIT} ${TARGET_CLI} ${TARGET_MCP}

$(TARGET_INIT): $(OBJS_INIT)
	$(CC) -o $@ $^ ${LDFLAGS}
	$(STRIP) $@ >/dev/null 2>&1  || true

$(TARGET_CLI): $(OBJS_CLI)
	$(CC) -o $@ $^ ${LDFLAGS}
	$(STRIP) $@ >/dev/null 2>&1  || true

$(TARGET_MCP): $(OBJS_MCP)
	$(CC) -o $@ $^ ${LDFLAGS}
	$(STRIP) $@ >/dev/null 2>&1  || true

$(SRCS_INIT:.cpp=.d):%.d:%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -MM $< >$@

$(SRCS_CLI:.cpp=.d):%.d:%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -MM $< >$@

$(SRCS_MCP:.cpp=.d):%.d:%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -MM $< >$@

.PHONY: clean
clean:
	${RM} ${TARGET_INIT} $(TARGET_CLI) $(TARGET_MCP) ${OBJS_INIT} ${OBJS_CLI} ${OBJS_MCP}
