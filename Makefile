all: build

build:
	cd src && make

pack:
	cd src && zip ../Tema_1_Mihai_Popescu_312CA.zip *.c *.h Makefile

clean:
	cd src && make clean
	rm -rf transpiler
