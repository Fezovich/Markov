#include "MarkovClUp.h"


int main () {

	bool userFinished = false;
	char userEnd;
	

	Markov test;

	while( userFinished == false){
	test.printDataS();
	cout << "\n\nEnter y to restart / q to quit ";
	cin >> userEnd;

	if( userEnd == 'q'){
		userFinished = true;
	}

}

	
	return 0;

}
