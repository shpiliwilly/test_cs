engine_CXX_SRCS := $(wildcard src/*.cpp)
engine_CXX_OBJS := ${engine_CXX_SRCS:.cpp=.o}
engine_OBJS := $(engine_CXX_OBJS)

tests_CXX_SRCS := $(wildcard test/*.cpp)
tests_CXX_SRCS += $(filter-out src/main.cpp, $(engine_CXX_SRCS))
tests_CXX_OBJS := ${tests_CXX_SRCS:.cpp=.o}
tests_OBJS := $(tests_CXX_OBJS)

CPPFLAGS += -std=c++1y -pthread -O3

.PHONY: all clean 

all: engine tests

engine: $(engine_OBJS)
	$(LINK.cc) $(engine_OBJS) -o engine-exec

tests: $(tests_OBJS)
	$(LINK.cc) $(tests_OBJS) -o tests-exec

clean:
	@- $(RM) engine-exec
	@- $(RM) tests-exec
	@- $(RM) $(engine_OBJS)
	@- $(RM) $(tests_OBJS)
