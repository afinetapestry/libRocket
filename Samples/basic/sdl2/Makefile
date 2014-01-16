EXE=sdl2
SRCDIR=src/
OBJDIR=obj/

DAO=-g3 -O0

CC=clang
CXX=clang++
LD=clang++
CFLAGS=-c $(DAO) -Wall -I$(SRCDIR)
CXXFLAGS=-c $(DAO) -Wall -I$(SRCDIR) -std=c++11
LDFLAGS= $(DAO) -std=c++11
LDADD=-lSDL2 -lGL -lGLEW -lRocketCore -lRocketDebugger
CSRCS:=$(wildcard $(SRCDIR)*.c)
CXXSRCS:=$(wildcard $(SRCDIR)*.cpp)
COBJS:=$(CSRCS:$(SRCDIR)%.c=$(OBJDIR)%.o)
CXXOBJS:=$(CXXSRCS:$(SRCDIR)%.cpp=$(OBJDIR)%.o)

all: $(EXE)

$(EXE): $(COBJS) $(CXXOBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDADD)

$(COBJS): $(OBJDIR)%.o : $(SRCDIR)%.c
	$(CC) $(CFLAGS) $< -o $@

$(CXXOBJS): $(OBJDIR)%.o : $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm $(COBJS) $(CXXOBJS) $(EXE)

.PHONY: clean

