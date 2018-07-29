### Misc ###
FINDSRCS = $(addprefix $(strip $(1))/, \
           $(notdir $(wildcard $(SRCDIR)/$(strip $(1))/$(strip $(2)))))

### Sources ###
SRCS += $(call FINDSRCS, ., *.c)

### Includes ###

### Compiler ###
CC = gcc
CXX = g++
CFLAGS = -std=gnu11
CXXFLAGS = -std=gnu++11
CPPFLAGS += -Wall -Wextra -g -DNDEBUG -O3
CPPFLAGS += -MT $@ -MMD -MP -MF $(@:.o=.d)
CPPFLAGS += $(addprefix -I $(SRCDIR)/, $(INCDIRS))
CPPFLAGS += $(addprefix -isystem $(SRCDIR)/, $(SYSINCDIRS))
CPPFLAGS += -pthread
LDLIBS =
LDFLAGS = -pthread
AR = ar
ARFLAGS = -rcs

### Targets ###
# EXTLIBS: built externally, define rule below
# LIBS: built internally, define prerequisite .o files only
# PROGS: built internally, define prerequisite .o and .a files only
EXTLIBS =
LIBS =
PROGS = risk
OTHER =

### Rules ###
risk: $(addsuffix .o, $(basename $(SRCS)))

tags: $(SRCS) $(shell find $(addprefix $(SRCDIR)/, $(INCDIRS)) -type f)
	ctags -f $@ $^

### Standard Rules ###
.DEFAULT_GOAL = all
.PHONY: all
all: $(EXTLIBS) $(LIBS) $(PROGS) $(OTHER)

VPATH = $(SRCDIR)

OBJS = $(addsuffix .o, $(basename $(SRCS)))
DEPS = $(addsuffix .d, $(basename $(SRCS)))

OBJDIRS = $(filter-out $(CURDIR)/, $(sort $(dir $(abspath $(OBJS)))))
$(OBJDIRS):
	@mkdir -p $@
$(OBJS): | $(OBJDIRS)
$(DEPS): | $(OBJDIRS)

%.o: %.c %.d
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

%.o: %.cpp %.d
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

$(LIBS):
	$(AR) $(ARFLAGS) $@ $^

$(PROGS):
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

-include $(DEPS)

print-%:
	@echo '$*=$($*)'
	@echo '  origin = $(origin $*)'
	@echo '  flavor = $(flavor $*)'
	@echo '  value  = $(value  $*)'
