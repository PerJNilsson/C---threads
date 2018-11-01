all:	newton

newton:	newton.c
	gcc -o newton newton.c -O2 -std=c11 -lm -pthread

maketar:
	tar -cvf hpcg031.tar.gz newton.c makefile report.md

test:
	/home/hpc2018/a2_grading/check_submission.py hpcg031.tar.gz

cleantest:
	rm -rf hpcg031.tar.gz extracted pictures reports

clean:
	-rm -rf newton
