EXEC=3-22-22-spreadsheet

INCLUDES=sheet.h

${EXEC}: ${INCLUDES} ${EXEC}.cpp Makefile
	g++ -g ${EXEC}.cpp -o ${EXEC} -Wall

run: ${EXEC}
	 ./${EXEC}

debug: ${EXEC}
	gdb ${EXEC}

clean:
	rm ${EXEC}