gcc vc_hidraw.c -shared -fPIC -o libvchidraw.so
g++ test.cpp libvchidraw.so -o test
sudo ./test
