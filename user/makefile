# Compiles every test*.cxx into test*.exe, runs it, and redirects the
# output to test*.out (same idea as the framework from earlier homeworks).
SRCS := $(wildcard test*.cxx)
EXES := $(SRCS:.cxx=.exe)
OUTS := $(SRCS:.cxx=.out)

all: $(OUTS)

%.exe: %.cxx hw1_test.h
	g++ -std=c++11 -Wall $< -o $@

%.out: %.exe
	./$< > $@ 2>&1 || true

clean:
	rm -f $(EXES) $(OUTS)

.PHONY: all clean
