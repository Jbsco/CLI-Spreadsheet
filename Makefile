# name of executable
EXEC=sheet
# source files - dependencies
SRC=sheet.cpp

# text replacement
${EXEC}: sheet.h ${SRC} ${EXEC}.cpp
	g++ -g ${EXEC}.cpp ${SRC} -o ${EXEC} -Wall
# '$@' is same as '${EXEC}'
# '$<' is same as everything in dependency list