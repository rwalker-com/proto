#!/usr/bin/env python

import sys

f = open(sys.argv[1], "w")

space=" "

f.writelines([space.join(sys.argv[2:]), "\n"])

f.close()
