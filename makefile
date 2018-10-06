.PHONY : all

all : clean newton compress

newton : newton
	@gcc -o newton threads.c -O2 -lm -pthread

run : run
	@echo "inputs are -t1 -l800 7"; \
	./newton -t1 -l800 7

run2 : run2
	@echo "inpus are -t1 -l800 2"; \
	./newton -t1 -l800 2
#failtest : failtest
#	@echo "inputs are -t100 aaaa8000 b"; \
#	./threads -t100 aaa8000 b

test : test
	/home/hpc2018/a2_grading/check_submission.py ~/assignment2/newtondir.tar.gz

compress : compress
	@mkdir newtondir; cp threads.c newtondir; cp makefile newtondir; cp report.md newtondir;\
	cd newtondir && tar czvf ../newtondir.tar.gz . && cd -
clean : clean
	@rm -rf newton; rm -rf newtondir;
