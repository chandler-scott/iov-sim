#
# OMNeT++/OMNEST Makefile for $(LIB_PREFIX)iov_sim
#
# This file was generated with the command:
#  opp_makemake --make-so -f --deep -o iov_sim -O out -pIOV_SIM -KVEINS_PROJ=../../veins -DVEINS_IMPORT -I. -I$$\(VEINS_PROJ\)/src -I. -L$$\(VEINS_PROJ\)/src -lveins$$\(D\)
#

# Name of target to be created (-o option)
TARGET_DIR = .
TARGET_NAME = $(LIB_PREFIX)iov_sim$(D)
TARGET = $(TARGET_NAME)$(SHARED_LIB_SUFFIX)
TARGET_IMPLIB = $(TARGET_NAME)$(IMPLIB_SUFFIX)
TARGET_IMPDEF = $(TARGET_NAME)$(IMPDEF_SUFFIX)
TARGET_FILES = $(TARGET_DIR)/$(TARGET)

# C++ include paths (with -I)
INCLUDE_PATH = -I. -I$(VEINS_PROJ)/src -I.

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = $(LDFLAG_LIBPATH)$(VEINS_PROJ)/src  -lveins$(D)

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/iov_sim/base/applications/BaseApplicationLayer.o \
    $O/iov_sim/base/python/AgentWrapper.o \
    $O/iov_sim/base/python/AggregatorWrapper.o \
    $O/iov_sim/base/python/BaseWrapper.o \
    $O/iov_sim/base/python/PythonLifecycle.o \
    $O/iov_sim/base/python/PythonWrapper.o \
    $O/iov_sim/base/util/Logger.o \
    $O/iov_sim/base/util/NeighborTable.o \
    $O/iov_sim/modules/applications/RSUApplication.o \
    $O/iov_sim/modules/applications/VehicleApplication.o \
    $O/iov_sim/base/messages/BaseMessage_m.o \
    $O/iov_sim/modules/messages/ClusterBeaconMessage_m.o \
    $O/iov_sim/modules/messages/ClusterElectionMessage_m.o \
    $O/iov_sim/modules/messages/ClusterSelectionMessage_m.o \
    $O/iov_sim/modules/messages/ModelRequestMessage_m.o \
    $O/iov_sim/modules/messages/ModelUpdateMessage_m.o \
    $O/iov_sim/modules/messages/NeighborTablePruneMessage_m.o \
    $O/iov_sim/modules/messages/RSUClusterDataMessage_m.o \
    $O/iov_sim/modules/messages/VehicleClusterDataMessage_m.o

# Message files
MSGFILES = \
    iov_sim/base/messages/BaseMessage.msg \
    iov_sim/modules/messages/ClusterBeaconMessage.msg \
    iov_sim/modules/messages/ClusterElectionMessage.msg \
    iov_sim/modules/messages/ClusterSelectionMessage.msg \
    iov_sim/modules/messages/ModelRequestMessage.msg \
    iov_sim/modules/messages/ModelUpdateMessage.msg \
    iov_sim/modules/messages/NeighborTablePruneMessage.msg \
    iov_sim/modules/messages/RSUClusterDataMessage.msg \
    iov_sim/modules/messages/VehicleClusterDataMessage.msg

# SM files
SMFILES =

# Other makefile variables (-K)
VEINS_PROJ=../../veins

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
CONFIGFILE = $(shell opp_configfilepath)
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIBS = -loppenvir$D $(KERNEL_LIBS) $(SYS_LIBS)
ifneq ($(PLATFORM),win32)
LIBS += -Wl,-rpath,$(abspath $(VEINS_PROJ)/src)
endif

COPTS = $(CFLAGS) $(IMPORT_DEFINES) -DVEINS_IMPORT -DIOV_SIM_EXPORT $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH) -PIOV_SIM_API
SMCOPTS =

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE (if COPTS has changed since last build)
# and make the object files depend on it
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
  $(shell $(MKPATH) "$O")
  $(file >$(COPTS_FILE),$(COPTS))
endif

# On Windows, the target has additional file(s). An import lib and an optional debug symbol file is created too.
ifeq ($(PLATFORM),win32)
  TARGET_FILES+= $(TARGET_DIR)/$(TARGET_IMPLIB)
  LDFLAGS+= $(LDFLAG_IMPLIB)$O/$(TARGET_IMPLIB)
  ifeq ($(TOOLCHAIN_NAME),clang-msabi)
    ifeq ($(MODE),debug)
      TARGET_FILES+=$(TARGET_DIR)/$(TARGET_NAME).pdb
    endif
  else
    TARGET_FILES+= $(TARGET_DIR)/$(TARGET_IMPDEF)
    LDFLAGS+= $(LDFLAG_IMPDEF)$O/$(TARGET_IMPDEF)
  endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
-include makefrag

#------------------------------------------------------------------------------

# Main target
all: $(TARGET_FILES)

$(TARGET_DIR)/% :: $O/%
	@mkdir -p $(TARGET_DIR)
	$(Q)$(LN) $< $@
ifeq ($(TOOLCHAIN_NAME),clang-msabi)
	-$(Q)-$(LN) $(<:%.dll=%.lib) $(@:%.dll=%.lib) 2>/dev/null

$O/$(TARGET_NAME).pdb: $O/$(TARGET)
endif

$O/$(TARGET) $O/$(TARGET_IMPDEF) $O/$(TARGET_IMPLIB) &: $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile $(CONFIGFILE)
	@$(MKPATH) $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) -o $O/$(TARGET) $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS) $(LDFLAGS)
	$(Q)$(SHLIB_POSTPROCESS) $O/$(TARGET)
ifeq ($(PLATFORM),win32)
	$(Q)llvm-ar d $O/$(TARGET_IMPLIB) $(TARGET) # WORKAROUND: throw away the first file from the archive to make the LLD generated import lib valid
endif

.PHONY: all clean cleanall depend msgheaders smheaders

# disabling all implicit rules
.SUFFIXES :

$O/%.o: %.cc $(COPTS_FILE) | msgheaders smheaders
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc -MD -MP -MF $O/$(basename $<)_m.h.d $(MSGCOPTS) $?

%_sm.cc %_sm.h: %.sm
	$(qecho) SMC: $<
	$(Q)$(SMC) -c++ -suffix cc $(SMCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

smheaders: $(SMFILES:.sm=_sm.h)

clean:
	$(qecho) Cleaning $(TARGET)
	$(Q)-rm -rf $O
	$(Q)-rm -f $(TARGET_FILES)
	$(Q)-rm -f $(call opp_rwildcard, . , *_m.cc *_m.h *_sm.cc *_sm.h)

cleanall:
	$(Q)$(CLEANALL_COMMAND)
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

help:
	@echo "$$HELP_SYNOPSYS"
	@echo "$$HELP_TARGETS"
	@echo "$$HELP_VARIABLES"
	@echo "$$HELP_EXAMPLES"

# include all dependencies
-include $(OBJS:%=%.d) $(MSGFILES:%.msg=$O/%_m.h.d)
