CXX=g++
CXXFLAGS=-g -Wall -Wno-sign-compare -lLLVM-15
YACC=bison
YFLAGS=--report=state -W -d
LEX=flex++
LFLAGS=--warn
SRC=utils.cpp LLVMIRGenerator.cpp SymbolTable.cpp node.cpp decaf.cpp decaf.tab.cpp decaf_lex.cpp
HDRS=decaf.tab.hpp node.hpp

.PHONY: clean

decaf: $(SRC) $(HDRS)
	$(CXX) $(CXXFLAGS) $(SRC) -o decaf

decaf.tab.cpp : decaf.ypp node.hpp
	$(YACC) $(YFLAGS) decaf.ypp

decaf_lex.cpp: decaf.lpp node.hpp
	$(LEX) $(LFLAGS) decaf.lpp

tidy:
	/bin/rm -f a.out core.* decaf.tab.* decaf.output decaf_lex.cpp

clean: tidy
	/bin/rm -f decaf 


