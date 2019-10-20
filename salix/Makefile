EXEC=alix
OBJFILES=alix.o

%.o:%.cpp
	g++ $< -c -o $@ -Wall -O2

${EXEC}: ${OBJFILES}
	g++ ${OBJFILES} -o ${EXEC} -O2 `allegro-config --libs`

clean:
	rm ${EXEC} ${OBJFILES}
