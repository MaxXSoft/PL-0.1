# C++ compiler
CPPFLAGS := $(DEBUG_ARG) $(OPT_ARG) $(INCLUDE_ARG)
CPPFLAGS += -c -Wall -Werror -std=c++17
export CC = clang++ $(CPPFLAGS)

# linker
LDFLAGS := $(LIBRARY_ARG)
export LD = clang++ $(LDFLAGS)
