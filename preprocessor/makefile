jpmx: main.o fileInput.o tokenInterpretation.o fileOutput.o noteList.o
	gcc -o jpmx main.o fileInput.o tokenInterpretation.o fileOutput.o noteList.o

main.o: main.c fileInput.h
	gcc -c -Wall main.c -o main.o

fileInput.o: fileInput.c tokenInterpretation.h fileOutput.h
	gcc -c -g -Wall fileInput.c -o fileInput.o

tokenInterpretation.o: tokenInterpretation.c noteList.h
	gcc -c -Wall tokenInterpretation.c

fileOutput.o: fileOutput.c noteList.h
	gcc -c -Wall fileOutput.c -o fileOutput.o

noteList.o: noteList.c
	gcc -c -Wall noteList.c -o noteList.o

clean:
	rm *.o
	rm jpmx
