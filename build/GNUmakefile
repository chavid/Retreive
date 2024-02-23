
#############################################################################
## Variables
#############################################################################

SHELL = /bin/bash

MDL_HDRS = $(wildcard ../src/*.hh)
MDL_SRCS = $(wildcard ../src/*.cc)
MDL_OBJS = $(patsubst ../src/%.cc, %.o, $(MDL_SRCS))
MDL_DEPS = $(patsubst %.o, %.d, $(MDL_OBJS))

APP_SRCS = $(wildcard ../src/*.cpp)
APP_EXES = $(patsubst ../src/%.cpp, %.exe, $(APP_SRCS))

CPPFLAGS = -I.
LDFLAGS = -L.
CXX = g++
CXXFLAGS = -static -std=c++20 -O3 -Wall -Wextra -Wfloat-equal
DEPFLAGS = -MT $@ -MMD -MP -MF $*.td


#############################################################################
## General targets
#############################################################################

all: $(APP_EXES)

$(APP_EXES): $(MDL_OBJS)

verbose:
	@echo ""
	@echo "MDL_HDRS:" $(MDL_HDRS)
	@echo "MDL_SRCS:" $(MDL_SRCS)
	@echo "MDL_OBJS:" $(MDL_OBJS)
	@echo "MDL_DEPS:" $(MDL_DEPS)
	@echo ""
	@echo "APP_SRCS:" $(APP_SRCS)
	@echo "APP_EXES:" $(APP_EXES)
	@echo ""

clean:
	rm -f *.exe *.o *.d *.td
	rm -f *~ core.* core vgcore.* .stamp-*




#############################################################################
## Correction of default variables
## Passing through a *.td file (temporary dependencies) which
## is renamed at the end of a successful compilation, followed by a touch
## ensures that the *.o will have a later date than the *.d
#############################################################################

COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $*.td $*.d && touch $@


#############################################################################
## Implicit rules
#############################################################################

%.exe: ../src/%.cpp $(MDL_OBJS)
	rm -f $@
	$(LINK.cc) -o $@ $^

%.o : ../src/%.cc
%.o : ../src/%.cc %.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

%.d: ;
.PRECIOUS: %.d %.o

include $(MDL_DEPS)
