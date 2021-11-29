#include <iostream>
#include "unistd.h"
#include "vc_hidraw.h"

using namespace std;

int main(){
	if (! open_vc_multimeter_hidraw()){
		cout << "(test_program) Error: VC Multimeter not found.\n";
		return 1;
	}
	
	vc_multimeter_reading reading;
	
	for (int i = 0; i < 100; i++){
		if (read_vc_multimeter_hidraw(&reading))
			cout << reading.str << '\n';
		else {
			cout << "?\n";
			sleep(0.5);
		}
	}
	
	close_vc_multimeter_hidraw();
}

