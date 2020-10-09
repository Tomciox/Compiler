CC = g++
CCFLAGS = -g #-fmax-errors=1
FLEX = flex
BISON = bison
LATEX = latex
DVIPS = dvips

all: latc Latte.ps

clean:
	rm -f *.o latc

# distclean:
# 	 rm -f *.o Absyn.C Absyn.H Test.C Parser.C Parser.H Lexer.C Skeleton.C Skeleton.H Printer.C Printer.H Latte.l Latte.y Latte.tex Latte.dvi Latte.aux Latte.log Latte.ps testLatte Makefile

latc: Absyn.o Latte.o Types.o Operators.o Lexer.o Parser.o Printer.o Test.o Helper.o
	@echo "Linking testLatte..."
	${CC} ${CCFLAGS} *.o -o latc

Helper.o: Helper.C Helper.H Absyn.H
	${CC} ${CCFLAGS} -c Helper.C

Absyn.o: Absyn.C Absyn.H Helper.H
	${CC} ${CCFLAGS} -c Absyn.C

Latte.o: Latte.C Absyn.H Helper.H
	${CC} ${CCFLAGS} -c Latte.C

Types.o: Types.C Absyn.H Helper.H
	${CC} ${CCFLAGS} -c Types.C

Operators.o: Operators.C Absyn.H Helper.H
	${CC} ${CCFLAGS} -c Operators.C

Lexer.C: Latte.l
	${FLEX} -oLexer.C Latte.l

Parser.C: Latte.y
	${BISON} Latte.y -o Parser.C

Lexer.o: Lexer.C Parser.H
	${CC} ${CCFLAGS} -c Lexer.C 

Parser.o: Parser.C Absyn.H
	${CC} ${CCFLAGS} -c Parser.C

Printer.o: Printer.C Printer.H Absyn.H
	${CC} ${CCFLAGS} -c Printer.C

Test.o: Test.C Parser.H Printer.H Absyn.H Helper.H
	${CC} ${CCFLAGS} -c Test.C

# Latte.dvi: Latte.tex
# 	${LATEX} Latte.tex

# Latte.ps: Latte.dvi
# 	${DVIPS} Latte.dvi -o Latte.ps

