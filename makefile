.PHONY : all

all : gcc run failtest

gcc : gcc
	@gcc -o threads threads.c -O2

run : run
	@echo "inputs are -t1 -l1000 8"; \
	./threads -t1 -l1000 8

failtest : failtest
	@echo "inputs are -t100 aaaa8000 b"; \
	./threads -t100 aaa8000 b
