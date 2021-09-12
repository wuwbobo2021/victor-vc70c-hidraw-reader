build:
	gcc vc_hidraw.c -shared -fPIC -o libvc70c-hidraw.so
	g++ -o test_program test.cpp ./libvc70c-hidraw.so
	./test_program

