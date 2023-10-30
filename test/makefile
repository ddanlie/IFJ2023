lexical: lexical_test.o lexical.o defs.o
	gcc defs.o lexical.o lexical_test.o -o lexical
	rm lexical_test.o lexical.o defs.o
	clear
	./lexical <lexical_text.txt

defs.o:
	gcc -c ../defs.c

lexical.o:
	gcc -c ../lexical.c

lexical_test.o:
	gcc -c lexical_test.c

clean:
	rm lexical