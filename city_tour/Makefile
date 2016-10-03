all: $(addsuffix .ok,$(wildcard *.test)) $(addsuffix .ok,$(wildcard *.assert))

clean:
	rm -f city_tour *.ok

%.assert.ok: city_tour %.assert
	if ./city_tour < $*.assert 2>/dev/null >/dev/null ; then : ; else touch $@; fi

%.test.ok: city_tour %.test
	./city_tour < $*.test > $*.test.out && diff -w $*.test.out $*.test.expect

city_tour: city_tour.c
	gcc -Wall -Werror -g -o $@ $<

