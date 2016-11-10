PROJECT=ReceiveData
CC=g++-4.9
CC+=-std=c++14
LIBS=/usr/local/lib
CFLAGS+=-larmadillo
mmacosx_flag=-mmacosx-version-min=10.8
${PROJECT}: ${PROJECT}.o LogFile.o
	${CC} ${mmacosx_flag} ${CFLAGS} -Wall -o ${PROJECT} ${PROJECT}.o  LogFile.o  -L${LIBS}  /usr/local/lib/liblsl.dylib  -lxml2 -larmadillo

${PROJECT}.o: ${PROJECT}.cpp
	${CC} -g3 -fno-omit-frame-pointer ${mmacosx_flag} -Wall -c ${PROJECT}.cpp -O2

LogFile.o: LogFile.cpp
	${CC} -g3 -fno-omit-frame-pointer ${mmacosx_flag} -Wall -c LogFile.cpp -O2




clean:
	rm -rf *.o


