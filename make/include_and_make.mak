
include date.min

$(info date=$(date))

all:
	true

clean:
	rm -f date.min date.min.pre

force: ;

.PHONY: force all clean

date.min: force
	echo "date=$$(date)" > date.min.pre
	if [ ! -r date.min ] || ! cmp -s date.min date.min.pre; then echo moving; mv date.min.pre date.min; fi


