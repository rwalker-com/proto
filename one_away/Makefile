# trivial makefile for test-driven development, one executable per directory
me=$(lastword $(subst /, ,$(CURDIR)))

all: $(addsuffix .ok,$(wildcard *.test)) $(addsuffix .ok,$(wildcard *.assert))

clean:
	rm -f $(me) *.ok

%.assert.ok: $(me) %.assert
	if ./$(me) < $*.assert 2>/dev/null >/dev/null ; then false ; else touch $@; fi

%.test.ok: $(me) %.test
	./$(me) < $*.test > $*.out && diff -w $*.out $*.expect

$(me): $(me).c
	gcc -Wall -Werror -g -o $@ $<

