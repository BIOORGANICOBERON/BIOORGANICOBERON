coco_build:
	
Parser.h:
Parser.cpp:
Scanner.cpp:
Scanner.h:
coco:	coco_build 
	../../discipline_cocor/Coco/Coco Oberon.atg -namespace Oberon
	echo Coco/R done

oberon_exec:	clean coco Parser.cpp Scanner.cpp Scanner.h Parser.h
	g++ -std=c++0x -g *.cpp -o test1/oberon $(CFLAGS)
	
all:	oberon_exec

wakeOberon:	oberon_exec
	cd test1 && ./oberon test.Mod && cd ..
	
clean:
	rm -f oberon Parser.cpp Parser.cpp.old Parser.h Parser.h.old Scanner.h Scanner.h.old Scanner.cpp Scanner.cpp.old

