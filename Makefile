#
# Makefile for vcard
#

OS   := $(shell uname -s)
ARCH := $(shell uname -m)


SRCS=$(wildcard src/*.cpp)
OBJS=$(SRCS:.cpp=.o)
INCLUDES=$(wildcard src/*.h)
CFLAGS=-Wall -g -std=c++11
#CC=g++
CC=clang++

ifeq ($(OS),Linux)
   LIBFLAGS=-L. -Wl,-rpath=.
else
$(error Unknown os: [$(OS)])
   #LIBFLAGS=-Llib -static-libgcc -static-libstdc++
endif

src/%.o : src/%.cpp ${INCLUDES}
	@echo "[$(CC)] $<"
	@$(CC) $(DEFINES) $< -c -o $@$(INCLUDE) $(CFLAGS)

vcard: $(OBJS)
	@echo "[link: $(APP_BUILD)] $@"
	@$(CC) $(OBJS) -o $@ $(LIBFLAGS) $(CFLAGS)

tags: ${SRCS} ${INCLUDES}
	@ctags -R src/

.PHONY: doc test clean

doc:
	@doxygen vcard.doxygen

test: 
	cd test; make

clean:
	@rm -f src/*.o
	@rm -f vcard
	@rm -Rf doc/html
	@cd test; make clean


