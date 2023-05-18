.PHONY: run gdb premake
INCS = -I ~/builds/orx/code/include/ -I /home/pwest/builds/orx/code/include/core/ -I /home/pwest/repos/libgff/include -I include/
SRCS = src/soloscuro.c  src/register_gff.c src/window_generator.c src/ui/button.c src/ui/window.c src/ui/font.c src/ui/ds1/*.c src/state.c src/sol_window.c src/eng/*.c
LIBS = -L /home/pwest/builds/orx/code/bin -L /home/pwest/repos/libgff/build/src/ -L /home/pwest/repos/libgff/build/ext/ -lgff -lxmi2mid -lorxd -lm

soloscuro: src/* include/*
	gcc -fsanitize=address -g -O0 ${INCS} ${SRCS} ${LIBS} -o soloscuro

run: premake
	#LD_LIBRARY_PATH=/home/pwest/builds/orx/code/bin:/home/pwest/repos/libgff/build/src/:/home/pwest/repos/libgff/build/ext/ ./soloscuro
	LD_LIBRARY_PATH=ext/orx/code/bin:ext/libgff/lib/ ./soloscuro

gdb: soloscuro
	LD_LIBRARY_PATH=/home/pwest/builds/orx/code/bin:/home/pwest/repos/libgff/build/src/:/home/pwest/repos/libgff/build/ext/ gdb ./soloscuro

premake:
	premake/premake4
	cd linux/gmake/ ; make LIBGFF=../../ext/libgff/ ORX=../ext/orx
