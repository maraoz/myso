TARGET = main
OBJS = ./obj/draw.o ./obj/core.o ./obj/marshall.o ./obj/protocol.o ./obj/util.o
SOURCES = ./src/draw.c ./src/core.c ./src/marshall.c ./src/protocol.c ./src/util.c
CFLAGS = -Wall -O -Wuninitialized

.SILENT:

$(TARGET): $(OBJS)
	gcc -o$(TARGET) $(OBJS) -lm 
	mv $(TARGET) ./bin
	echo "Linkeando los archivos $(OBJS) para obtener $(TARGET)... "


./obj/draw.o: ./inc/typedef.h ./inc/draw.h ./inc/core.h

./obj/core.o: ./inc/typedef.h ./inc/draw.h ./inc/core.h

./obj/marshall.o: ./inc/typedef.h ./inc/draw.h ./inc/core.h

./obj/protocol.o: ./inc/typedef.h ./inc/draw.h

./obj/util.o: ./inc/typedef.h ./inc/draw.h ./inc/core.h


$(OBJS) : $(SOURCES) ./inc/typedef.h
	gcc -c $(SOURCES)
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
