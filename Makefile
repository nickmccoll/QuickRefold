# =============================== 
# Makefile for ana 
# =============================== 

LIBDIR        = lib/
SRCDIR        = src/
INTDIR        = interface/
CINTDIR       = cint/

ROOTGLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs)
ROOTCFLAGS    = $(shell $(ROOTSYS)/bin/root-config --cflags) 
ARCH         := $(shell $(ROOTSYS)/bin/root-config  --arch)
# CXX           = g++
# CXXFLAGS      = -g -pipe -Wall -fPIC -I../.. -I.

# SOFLAGS       = -shared
# ifeq ($(ARCH),macosx64)
# SOFLAGS       = -dynamiclib -single_module -undefined dynamic_lookup -install_name
# endif

# CXXFLAGS     += $(ROOTCFLAGS)

MKARCH := $(wildcard $(shell root-config --etcdir)/Makefile.arch)
include $(ROOTSYS)/etc/Makefile.arch

CXXFLAGS += -I../.. -I.
ANACLASSES = $(LIBDIR)Refold.o $(LIBDIR)QuickRefoldDict.o 

# =============================== 
ana: $(SRCDIR)Refold.cc 
# ------------------------------- 
	$(ROOTSYS)/bin/rootcint  -v -l -f $(CINTDIR)QuickRefoldDict.cc -c $(INTDIR)Refold.h $(SRCDIR)LinkDef.h 
	$(CXX) $(CXXFLAGS) -c $(CINTDIR)QuickRefoldDict.cc -o $(LIBDIR)QuickRefoldDict.o 	
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)Refold.cc -o $(LIBDIR)Refold.o  	
	$(CXX) $(SOFLAGS) $(ANACLASSES) -o libQuickRefold.so $(ROOTGLIBS) 

# =============================== 
clean: 
	rm -f $(LIBDIR)* $(CINTDIR)* libQuickRefold.so 
# =============================== 