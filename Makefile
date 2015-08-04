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
#ANACLASSES = $(LIBDIR)Refold.o $(LIBDIR)MakeRefold.o $(LIBDIR)QuickRefoldDict.o
# ANACLASSES = $(LIBDIR)Refold.o  $(LIBDIR)QuickRefoldDict.o

# # ===============================
# QuickRefold: $(SRCDIR)Refold.cc
# # -------------------------------
# 	$(ROOTSYS)/bin/rootcint  -v -l -f $(CINTDIR)QuickRefoldDict.cc -c -I../../ $(INTDIR)Refold.h  $(SRCDIR)QuickRefold_LinkDef.h
# 	$(CXX) $(CXXFLAGS) -c $(CINTDIR)QuickRefoldDict.cc -o $(LIBDIR)QuickRefoldDict.o
# 	$(CXX) $(CXXFLAGS) -c $(SRCDIR)Refold.cc -o $(LIBDIR)Refold.o
# #	$(CXX) $(CXXFLAGS) -c $(SRCDIR)MakeRefold.cc -o $(LIBDIR)MakeRefold.o
# 	# $(CXX) $(SOFLAGS) $(ANACLASSES) -o libQuickRefold.so $(ROOTGLIBS)
# 	$(LD) $(SOFLAGS) $(LDFLAGS) $^ -o libQuickRefold.so $(ROOTGLIBS)
# 	# $(LD) $(SOFLAGS) $(LDFLAGS) $^ $(OutPutOpt) $@ $(EXPLLINKLIBS)

# QuickRefold: $(SRCDIR)Refold.cc
# 	$(LD) $(SOFLAGS)$@ $(LDFLAGS) $^ $(OutPutOpt) $@ $(EXPLLINKLIBS) $(ROOTGLIBS)


all: $(CINTDIR)QuickRefoldDict.cc $(LIBDIR)QuickRefoldDict.o $(LIBDIR)Refold.o  $(LIBDIR)MakeRefold.o libQuickRefold.so

$(CINTDIR)QuickRefoldDict.cc: $(INTDIR)Refold.h $(INTDIR)MakeRefold.h $(SRCDIR)QuickRefold_LinkDef.h
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -v -l -f $@ -c -I../.. $^ 

$(LIBDIR)QuickRefoldDict.o: $(CINTDIR)QuickRefoldDict.cc
	$(CXX)  $(CXXFLAGS) -c $<  -o $@	
			
$(LIBDIR)Refold.o: $(SRCDIR)Refold.cc
	$(CXX)  $(CXXFLAGS) -c $<  -o $@

$(LIBDIR)MakeRefold.o: $(SRCDIR)MakeRefold.cc
	$(CXX)  $(CXXFLAGS) -c $<  -o $@

libQuickRefold.so:	$(LIBDIR)Refold.o $(LIBDIR)MakeRefold.o $(LIBDIR)QuickRefoldDict.o 
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
