.PHONY: run gdb
INCS = -I ~/builds/orx/code/include/ -I /home/pwest/builds/orx/code/include/core/ -I /home/pwest/repos/libgff/include -I include/
SRCS = src/soloscuro.c  src/register_gff.c
LIBS = -L /home/pwest/builds/orx/code/bin -L /home/pwest/repos/libgff/build/src/ -L /home/pwest/repos/libgff/build/ext/ -lgff -lxmi2mid -lorxd -lm

soloscuro: src/* include/*
	gcc -g -O0 ${INCS} ${SRCS} ${LIBS} -o soloscuro

run: soloscuro
	LD_LIBRARY_PATH=/home/pwest/builds/orx/code/bin:/home/pwest/repos/libgff/build/src/:/home/pwest/repos/libgff/build/ext/ ./soloscuro

gdb: soloscuro
	gdb ./soloscuro
