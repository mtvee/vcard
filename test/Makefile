# Unit testing with qunit
#
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
INCLUDE=-I../src
CC=g++
CFLAGS=-std=c++11
LIBFLAGS=-lm


# add an executable test here and make a target for it below
TESTS=testopt

tests: $(TESTS)
	@$(foreach test,$(TESTS), ./$(test); )

testopt: testopt.cpp ../src/optparser.cpp
	@echo "[$(CC)] $<"
	@$(CC) $(CFLAGS) $(INCLUDE) $? -o $@ $(LIBFLAGS)

clean:
	@$(foreach var,$(TESTS), rm -f $(var); )

