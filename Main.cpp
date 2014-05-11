#include "Markov.h"


int main () {

	bool userFinished = false;
	char userEnd;
	

	markov test;
	test.generateS();
	test.extract();
	test.map();
	test.failFind();

	while( userFinished == false){

	test.userCalc();

	cout << "\n\nEnter y to restart / q to quit ";
	cin >> userEnd;

	if( userEnd == 'q'){
		userFinished = true;
	}

}

	
	return 0;

}
