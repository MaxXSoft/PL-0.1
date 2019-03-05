# C++ compiler
CPPFLAGS := $(DEBUG_ARG) $(OPT_ARG) $(INCLUDE_ARG)
CPPFLAGS += -c -Wall -Werror -std=c++17
export CC = g++ $(CPPFLAGS)

# linker
LDFLAGS := $(LIBRARY_ARG)
export LD = g++ $(LDFLAGS)
