#include <iostream>
#include "vc_hidraw.h"

using namespace std;

int main(){
	if (! open_vc_multimeter_hidraw()){
		cout << "VC Multimeter not found.\n";
		return 1;
	}
	
	vc_multimeter_reading reading;
	
	for (int i = 0; i < 100; i++){
		read_vc_multimeter_hidraw(&reading);
		cout << reading.str << '\n';
	}
	
	close_vc_multimeter_hidraw();
}

