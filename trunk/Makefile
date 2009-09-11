all: lineas core

TARGET1 = core
OBJS1 = ./obj/draw.o ./obj/core.o ./obj/marshall.o ./obj/protocol.o\
./obj/util.o ./obj/lineas.o ./obj/maincore.o ./obj/files.o
SOURCES1 = ./src/draw.c ./src/core.c ./src/marshall.c ./src/protocol.c\
./src/util.c ./src/lineas.c ./src/maincore.c ./src/files.c
CFLAGS = -Wall -O -Wuninitialized

TARGET2 = lineas
OBJS2 = ./obj/draw.o ./obj/core.o ./obj/marshall.o ./obj/protocol.o\
./obj/util.o ./obj/lineas.o ./obj/mainlines.o ./obj/files.o
SOURCES2 = ./src/draw.c ./src/core.c ./src/marshall.c ./src/protocol.c\
./src/util.c ./src/lineas.c ./src/mainlines.c ./src/files.c
CFLAGS = -Wall -O -Wuninitialized

.SILENT:
$(TARGET2): $(OBJS2)
	    gcc -o$(TARGET2) $(OBJS2) -lm -pthread -lncurses
	    mv $(TARGET2) ./bin
	    echo "Linkeando los archivos $(OBJS) para obtener $(TARGET)... "

.SILENT:
$(TARGET1): $(OBJS1)
	gcc -o$(TARGET1) $(OBJS1) -lm -pthread -lncurses
	mv $(TARGET1) ./bin
	echo "Linkeando los archivos $(OBJS) para obtener $(TARGET)... "


./obj/draw.o: ./inc/typedef.h ./inc/core.h

./obj/lineas.o: ./inc/typedef.h ./inc/lineas.h ./inc/files.h

./obj/core.o: ./inc/typedef.h ./inc/core.h ./inc/draw.h ./inc/util.h \
./inc/files.h

./obj/maincore.o: ./inc/typedef.h ./inc/core.h ./inc/draw.h ./inc/util.h \
./inc/files.h

./obj/files.o: ./inc/typedef.h

./obj/core.o: ./inc/typedef.h ./inc/core.h ./inc/draw.h ./inc/util.h \
./inc/files.h

./obj/marshall.o: ./inc/typedef.h ./inc/protocol.h ./inc/core.h \
./inc/marshall.h

./obj/protocol.o: ./inc/typedef.h ./inc/protocol.h ./inc/util.h

./obj/util.o: ./inc/typedef.h

.SILENT:
$(OBJS1) : $(SOURCES1) ./inc/typedef.h
	gcc -c $(SOURCES1)
	mv *.o ./obj
	echo "Compilando los archivos $(SOURCES) ... "

.SILENT:
$(OBJS2) : $(SOURCES2) ./inc/typedef.h
	gcc -c $(SOURCES2)
	mv *.o ./obj
	echo "Compilando los archivos $(SOURCES) ... "


clear:
	- rm -f ./*.o
	- rm -f ./obj/*
	- rm -f ./bin/*
	- rm -f ./tests/*.o
	echo "Borrando objs y binarios ..."

garbage:
	- rm -f ./src/*~ 
	- rm -f ./inc/*~
	- rm -f ./doc/*~ 
	- rm -f ./*~
	- rm -f ./tests/*~
	echo "Borrando molestos archivos temporales ..."
