PROJECT=ReceiveData
CC=g++-4.9
CC+=-std=c++14
LIBS=/usr/local/lib
CFLAGS+=-larmadillo
mmacosx_flag=-mmacosx-version-min=10.8
${PROJECT}: ${PROJECT}.o
	${CC} ${mmacosx_flag} ${CFLAGS} -Wall -o ${PROJECT} ${PROJECT}.o   -L${LIBS}  /usr/local/lib/liblsl.dylib  -lxml2

${PROJECT}.o: ${PROJECT}.cpp
	${CC} -g3 -fno-omit-frame-pointer ${mmacosx_flag} -Wall -c ${PROJECT}.cpp -O2 -larmadillo

clean:
	rm -rf *.o


