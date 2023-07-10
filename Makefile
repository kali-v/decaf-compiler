CXX=g++
CXXFLAGS=-g -Wall -Wno-sign-compare -lLLVM-15
YACC=bison
YFLAGS=--report=state -W -d
LEX=flex++
LFLAGS=--warn
SRC=src/utils.cpp src/LLVMIRGenerator.cpp src/SymbolTable.cpp src/node.cpp src/decaf.cpp src/decaf.tab.cpp src/decaf_lex.cpp
HDRS=src/decaf.tab.hpp src/node.hpp

.PHONY: clean

decaf: $(SRC) $(HDRS)
	$(CXX) $(CXXFLAGS) $(SRC) -o decaf

src/decaf.tab.cpp : src/decaf.ypp src/node.hpp
	$(YACC) $(YFLAGS) src/decaf.ypp

src/decaf_lex.cpp: src/decaf.lpp src/node.hpp
	$(LEX) $(LFLAGS) src/decaf.lpp

tidy:
	/bin/rm -f a.out core.* src/decaf.tab.* decaf.output src/decaf_lex.cpp

clean: tidy
	/bin/rm -f decaf
