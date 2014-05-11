#ifndef File_read_H
#define	File_read_H

#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <conio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#define fail  1
#define bg  2
using namespace std;



class markov
{
	string states;
	string stateTemp;
	map<string, vector<float>> DataS;
	map<string, vector<float>>::iterator itCurrent;
	map<string, vector<float>>::iterator itNext;
	map<string, vector<float>>::iterator mapEnd;
	vector<int> failLocation;
	vector<int>::iterator failLocationIt;
	float currentCount;
	float nextCount;
	float ftally;
	float btally;
	int stateLengthMax;

public:

	void generateS();
	void extract();
	void map();
	void prob();
	void userCalc();
	void failFind();
};



void markov::generateS(){												// Generates a string of test data for anlysis



	ofstream myfile;
	myfile.open ("BatteryState.txt");
	myfile << "OOOOOOOOOOBBBOOBBOBBOBBOBBBBFOOOOOOOOOOOOOBBBOBBBOBBOBBOBBOBF";
	myfile.close();

}																


void markov::extract(){															// Opens the previously generated file and increments through the string printing and storing each character to the screen

	
ifstream myfile2 ("BatteryState.txt");													// open file and assign to string 

if (myfile2.is_open()){ 
	int i = 0;
 while ( myfile2.good()){
	 getline (myfile2,states);
	 while (i != states.size()){		// output to screen
		cout << states[i];
		i++;
	 }
 }
 myfile2.close();
 }else 
 cout << "Unable to open file states"; 
}


void markov::map(){

int a = 0;
stateTemp = "AAA";
itCurrent = DataS.begin();
stateLengthMax = 0;

cout << "\nEnter the size of each state: ";
cin >> stateLengthMax;

stateTemp.resize(stateLengthMax);

																					//###### 1.  Load 3 characters from the string into a temp string for insertion into the map ##### 

for ( unsigned i = 0; i != states.size()-(stateLengthMax-1); i++){	
 for( unsigned k = 0; k != stateLengthMax; k++){
	 stateTemp[k] = states[i+k];
 }
DataS[stateTemp] = vector<float>();																	// load the data into 3 character string stateTemps and store the value in the map
}
																					//#######  2.  Looks through map and total up how many times each key appears in the main string. ###### 

for (itCurrent= DataS.begin(); itCurrent != DataS.end(); ++itCurrent){
	currentCount = 0;
	nextCount = 0;
	bool firstRun = true;
	vector<float> tempVec(0);
	vector<float>::iterator itVec;
		
	for (itNext= DataS.begin(); itNext != DataS.end(); ++itNext){											// searches through the string assigning the next probabilties to the map untill it there are no more possible states.
		int fFound = stateLengthMax;
		stateTemp = itCurrent->first;
		fFound = stateTemp.find('F');

		if(stateTemp[stateLengthMax-1] == 'F' && itNext == itCurrent){											// Checks if the state contains a Fail and overides the next search and sets the percent to 1 creating and absorbing state.
			nextCount = 1;
			currentCount = 1;

		}else if((fFound >= 0 && fFound <= stateLengthMax-2) || (fFound == stateLengthMax-1 && itNext != itCurrent)){     //
			nextCount = 0;
			currentCount = 1;

		}else{
			
			for ( unsigned i = 0; i != states.size()-stateLengthMax; i++){				    	// increments the states string untill it reaches the end
				for( unsigned k = 0; k != stateTemp.size(); k++){								// increments the statesTemp and states string to store 3 values
					stateTemp[k] =	states[i+k]; 
				}

				if(	itCurrent->first == stateTemp && firstRun == true){						// compares the current state string stored in map to stateTemp to locate the current state
						currentCount++;
				}

				if(itCurrent->first == stateTemp &&  i !=states.size()-3){

					int lookforward = 0;													// lookforward variable shifts the string one place to the right
					lookforward = i+1;
					for( unsigned k = 0; k != stateTemp.size(); k++){						// loads the shifted state into stateTemp
						stateTemp[k] =	states[lookforward+k]; 
					}

					if( itNext->first == stateTemp){									// compare the next state to the state that appears after the current state.
						nextCount++;													// increment if valid.
					}
																		
				}
			}
		}


		tempVec.push_back (nextCount/currentCount );									// Calculates the chance a state is followed by a next state and puts the percentage on the end of the vector.																								
		nextCount = 0;																	// ###########			   Reset variables.          ##############
		firstRun = false;
			
	}


	DataS[itCurrent->first].swap(tempVec);											// puts the vector contents into the map.


}
}


void markov::userCalc(){

int day = 0;
int RunningTotal = 0;
bool endValueReached = false;
vector<float> MapVec(0);													// empty vector to swap with the vector stored in the map.
vector<float> ResultPVec(0);												// empty vector to store the initial state percents.
vector<float> TempPVec(0);													// empty vector to store the percent while calculations take place.

cout << "\n \n Please enter a "<< stateLengthMax <<" length character String: ";						// user enters a char string the size of the chosen state legnth. 
cin >> stateTemp;																						// store in temp string
	
cout << "\nPlease enter the percentage you wish to reach: ";											// User enters the target Percentage they want the calculation reach.
float finishPercent = 0;																				// initialise.
cin >> finishPercent;																					// Stores.
																			// #######  4.  Takes user entered data and performs calculation untill user percent is reached. ###### 
for (itCurrent= DataS.begin(); itCurrent != DataS.end(); ++itCurrent){									// Loops through the map.	
	if( stateTemp == itCurrent->first){																	//Checks if the state the user entered matches the state the iterator points to.	
		ResultPVec = DataS[itCurrent->first];															// Copy the vector into a new Vector that will store the results of the calculation.																
	}
}
																			// #######  5.  Takes user entered data and performs calculation untill user percent is reached. ###### 
while(endValueReached == false){																	// Continue untill this flag is changed by the Chosen Percent == the Calculated Percent.
	TempPVec.resize(ResultPVec.size());																//Resize a Temporary vector to the legnth of the map vectors.
	for (int MapVecLength = 0; MapVecLength != ResultPVec.size(); MapVecLength++){					// Matrix multiplication
		itCurrent= DataS.begin();
		for(int ResultPLength = 0; ResultPLength != ResultPVec.size(); ResultPLength++){
			MapVec = DataS[itCurrent->first];
			TempPVec[MapVecLength] += ResultPVec[ResultPLength] * MapVec[MapVecLength];		
			itCurrent++;
		}
	}

day++;
ResultPVec = TempPVec;													// store the finished calculation into the result vector.
TempPVec.clear();														// clear the Temp vector for the next calculation.
float failPTotal = 0;													// used to add up all states that are fails.

	for(int  i =0; i < failLocation.size(); i++){						// Increment 
		failPTotal = failPTotal + ResultPVec[failLocation[i]];
	}
	if(failPTotal > finishPercent){
		endValueReached = true;
		cout << "\n\nPercentage of fail reached = " << failPTotal;
	}
}
				
cout << "\nEstimated days till this chance of fail is reached = " <<day;

}
																			// #######  3.  Locates the states that end a Fail state and stores the location so that they can be quickly looked up for Results. ###### 

void markov::failFind(){
	
int locationCount = 0;
for (itCurrent= DataS.begin(); itCurrent != DataS.end(); ++itCurrent){					// Loop through all states stored in the Map.
	stateTemp = itCurrent->first;														// Assign the key to a temporary string.
	if(stateTemp[stateLengthMax-1] == 'F'){												//Check if the state ends with a F character.
		failLocation.push_back (locationCount);											// stores the location of the Key with the fail state in a vector.
	}
locationCount++;																		// increment the locationCount so it matches the location of the current map key. eg ItCurrent iterator points to the second key, 
}																						// locationCount = 1 if this ended in an F it would be stored in a Vector.

}


#endif