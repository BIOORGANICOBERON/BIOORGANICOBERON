frames_and_grammar:	Parser.frame Scanner.frame Coco.atg

rebuild:
Scanner.cpp:	frames_and_grammar
Scanner.h:	frames_and_grammar
Parser.cpp:	frames_and_grammar
Parser.h:	frames_and_grammar
	./Coco Coco.atg -namespace Coco -trace AJ

Coco:
all:	Scanner.cpp Scanner.h Parser.cpp Parser.h gpp

gpp:
	g++ *.cpp -o Coco $(CFLAGS) -g -Wall
	@echo done.
	
clean:
	rm -f Coco

install:
	ln -s /usr/lib/coco-cpp/Coco $(DESTDIR)/usr/bin/cococpp
	install -m 0755 Coco $(DESTDIR)/usr/lib/coco-cpp
	install -m 0644 *frame $(DESTDIR)/usr/share/coco-cpp

