# =============================== 
# Makefile for QuickRefold 
# =============================== 

LIBDIR        = lib/
SRCDIR        = src/
INTDIR        = interface/
CINTDIR       = cint/

MKARCH := $(wildcard $(shell root-config --etcdir)/Makefile.arch)
include $(ROOTSYS)/etc/Makefile.arch

CXXFLAGS += -I../.. -I.
ANACLASSES = $(LIBDIR)Refold.o $(LIBDIR)QuickRefoldDict.o 

# =============================== 
QuickRefold: $(SRCDIR)Refold.cc 
# ------------------------------- 
	$(ROOTSYS)/bin/rootcint  -v -l -f $(CINTDIR)QuickRefoldDict.cc -c $(INTDIR)Refold.h $(SRCDIR)LinkDef.h 
	$(CXX) $(CXXFLAGS) -c $(CINTDIR)QuickRefoldDict.cc -o $(LIBDIR)QuickRefoldDict.o 	
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)Refold.cc -o $(LIBDIR)Refold.o  	
	$(CXX) $(SOFLAGS) $(ANACLASSES) -o libQuickRefold.so $(ROOTGLIBS) 

# =============================== 
clean: 
	rm -f $(LIBDIR)* $(CINTDIR)* libQuickRefold.so 
# =============================== 