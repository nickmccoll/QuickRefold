# =============================== 
# Makefile for QuickRefold 
# =============================== 

LIBDIR        = lib/
SRCDIR        = src/
INTDIR        = interface/
CINTDIR       = cint/

ROOUNFOLDDIR = ../RooUnfold/

MKARCH := $(wildcard $(shell root-config --etcdir)/Makefile.arch)
include $(ROOTSYS)/etc/Makefile.arch

CXXFLAGS += -I../.. -I.

all: $(CINTDIR)QuickRefoldDict.cc $(LIBDIR)QuickRefoldDict.o $(LIBDIR)Refold.o $(LIBDIR)TObjectContainer.o  $(LIBDIR)MakeRefold.o libQuickRefold.so

$(CINTDIR)QuickRefoldDict.cc: $(INTDIR)BaseCorrector.h $(INTDIR)TObjectContainer.h $(INTDIR)Refold.h $(INTDIR)MakeRefold.h $(SRCDIR)QuickRefold_LinkDef.h
	@echo "Generating dictionary $@..."
	rootcint -v -l -f $@ -c -I../.. $^ 

$(LIBDIR)QuickRefoldDict.o: $(CINTDIR)QuickRefoldDict.cc
	$(CXX)  $(CXXFLAGS) -c $<  -o $@	
			
$(LIBDIR)Refold.o: $(SRCDIR)Refold.cc
	$(CXX)  $(CXXFLAGS) -c $<  -o $@
	
$(LIBDIR)TObjectContainer.o: $(SRCDIR)TObjectContainer.cc
	$(CXX)  $(CXXFLAGS) -c $<  -o $@

$(LIBDIR)MakeRefold.o: $(SRCDIR)MakeRefold.cc
	$(CXX)  $(CXXFLAGS) -c $<  -o $@

libQuickRefold.so:	$(LIBDIR)TObjectContainer.o $(LIBDIR)Refold.o $(LIBDIR)MakeRefold.o $(LIBDIR)QuickRefoldDict.o 
ifeq ($(PLATFORM),macosx)
	$(LD) $(SOFLAGS)$@ $(LDFLAGS) $(ROOUNFOLDDIR)libRooUnfold.so $^ $(OutPutOpt) $@ $(EXPLLINKLIBS)
else
	$(LD) $(SOFLAGS) $(LDFLAGS) $(ROOUNFOLDDIR)libRooUnfold.so $^ $(OutPutOpt) $@ $(EXPLLINKLIBS)
endif


	
	
# =============================== 
clean: 
	rm -f $(LIBDIR)* $(CINTDIR)* libQuickRefold.so 
# =============================== # DO NOT DELETE
# DO NOT DELETE
