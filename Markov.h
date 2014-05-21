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
using namespace std;

class Markov
{
	string states;
	string stateTemp;
	string absorbChar;
	map<string, vector<float>> DataS;
	map<string, vector<float>>::iterator itCurrent;
	map<string, vector<float>>::iterator itNext;
	vector<int> failLocation;
	vector<int>::iterator failLocationIt;
	float currentCount;
	float nextCount;
	int stateLengthMax;
	int absorbFound;
	bool absorbMap;

public:
	void printDataS();
	void makeStringFile();
	void loadStringFile();
	void generateStateMap();
	void userCalc();
	Markov();

private:
	void userMapDef();
	void mapKeyStore();
	bool absorbStateSet();
	void failFind();
};

Markov::Markov(){

	makeStringFile();
	loadStringFile();
	userMapDef();
	generateStateMap();
	if(absorbMap == true){
	userCalc();
	}

}

void Markov::printDataS(){
std::map<std::string, std::vector<float> >::iterator itr;
for(itr = DataS.begin(); itr != DataS.end(); ++itr)
{
cout << itr->first << endl;
cout << " ";
for(std::vector<float>::iterator j = itr->second.begin(); j != (itr->second).end(); ++j)
{
cout << " " << *j;
}
cout << endl;
}
}

void Markov::makeStringFile(){												// Generates a string of test data for anlysis. Temp will not be in final code

	ofstream myfile;
	myfile.open ("BatteryState.txt");
	myfile << "OOOOOOOOOOBBBOOBBOBBOBBOBBBBFOOOOOOOOOOOOOBBBOBBBOBBOBBOBBOBF";
	myfile.close();

}																

void Markov::loadStringFile(){															// Opens the previously generated file and increments through the string printing and storing each character to the screen
	
	ifstream myfile2 ("BatteryState.txt");													// Temp will not be in final code
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

void Markov::generateStateMap(){															//Creates keys from a string and calculates initial transition probabilties between keys.

	for( unsigned i = 0; i != states.size()-(stateLengthMax-1); i++){						// Store stateLengthMax number of characters from the string as keys in a map shifting one place to the right untill the end of the string.
		for( unsigned k = 0; k != stateLengthMax; k++){
			 stateTemp[k] = states[i+k];
		}
		 mapKeyStore();																		// Map store function with absorbing state check.
	}
																				
	for (itCurrent = DataS.begin(); itCurrent != DataS.end(); ++itCurrent){					// Iterate through each key in the map this represents our current state itCurrent.
		currentCount = 0;
		nextCount = 0;
		bool totalCurrentStates = true;
		vector<float> tempVec(0);
		
		for (itNext = DataS.begin(); itNext != DataS.end(); ++itNext){						// For each key in the map represented by itCurrent go through every key in the same map represented by itNext and check how many times follows									
																							// The current key appearance in the map.
			if(absorbStateSet() == false){		

				for ( unsigned i = 0; i != states.size()-(stateLengthMax-1); i++){			// increments through the entire states string so we can calculate transition probabilites.
					for( unsigned k = 0; k != stateLengthMax; k++){							// loads the stateLengthMax number of characters into a temporary string every time the position in the states string is increased.
						stateTemp[k] =	states[i+k]; 
					}

					if(itCurrent->first == stateTemp){										// Check if we have found the Key value in the string

						if(totalCurrentStates == true){										// If this is the first time through the string with this key representing our current state total up how many times the current state appears.

							currentCount++;
						}
	
						if( i != states.size()-(stateLengthMax)){							// Make sure we wont increment out of the string.

							int lookforward = 0;												
							lookforward = i+1;												// lookforward variable shifts the string one place to the right as we want to see what next state follows the current state.
							for( unsigned k = 0; k != stateTemp.size(); k++){				// loads the next state into stateTemp
							stateTemp[k] =	states[lookforward+k]; 
							}
							if( itNext->first == stateTemp){								// compare the next state value in the map to the string that appears after the current state which is now stored in stateTemp.
							nextCount++;													// If we match add to the total
							}

						}			
					}
				}
			}
			tempVec.push_back(nextCount/currentCount );									// when we know how many times the itCurrent key appears and the following itNext key appears calculate the percentage and add it to a temp Vector.																						
			nextCount = 0;																// We are going to check the next key in the map so we need to reset the next count to zero.
			totalCurrentStates = false;													// We already have how many times itCurrent, the current key appears in the string so dont bother counting again.
		}
	DataS[itCurrent->first].swap(tempVec);												// By going through the Keys in order and pushing the value on the end we make the vector values line up with the Key values. 
	}																				
}

void Markov::userMapDef(){

	cout << "\n\nEnter the size of each state: ";
	cin >> stateLengthMax;
	stateTemp.resize(stateLengthMax);														//user defines the length of a state.	
	cout << "\nIf absorb state exists enter character, else enter \"null\":";
	cin >> absorbChar;
	if(absorbChar.find('null') == string::npos){
		absorbMap = true;
	}else{
		absorbMap = false;
	}	 

}

void Markov::mapKeyStore(){													// Stores the stateLengthMax sized string as a key in the map as long as an absorbing state is not found at a position other than the end.
	

	if(absorbMap == true){													// Temporary condition used to skip absorb state markov logic. Refine into a parameter

		if((absorbFound = stateTemp.find(absorbChar)) >= 0 && absorbFound <= stateLengthMax-2){     
			return;			
		}else{
			DataS[stateTemp] = vector<float>();
		}

	}else{
		DataS[stateTemp] = vector<float>();	
	}	
}

bool Markov::absorbStateSet(){

	if(absorbMap == true){															// Temporary condition used to skip absorb state markov logic. Refine into a parameter
		stateTemp = itCurrent->first;																		// load the current key value into a tempory string for check.
	
		if(stateTemp.compare(stateLengthMax-1,1,absorbChar,0,1) == 0 && itNext == itCurrent){				// If Current state pointer points to an absorbing state and the Next state pointer points to the same state set the value to 1.
				nextCount = 1;
				currentCount = 1;
				return true;																				// Skip Count.

		}else if(stateTemp.compare(stateLengthMax-1,1,absorbChar,0,1) == 0 && itNext != itCurrent){			// If Current state pointer is pointing to an absorbing state and the Next state is pointing to a diffrent state set the value to 0.
				nextCount = 0;
				currentCount = 1;
				return true;																				// Skip Count.

		}else{																								//If nethier condition is true treat it as a normal state and perform calculation.
				return false;	
		}
	}
	return false;
}

void Markov::userCalc(){

	int day = 0;
	int RunningTotal = 0;
	bool endValueReached = false;
	vector<float> MapVec(0);													// empty vector to swap with the vector stored in the map.
	vector<float> ResultPVec(0);												// empty vector to store the initial state percents.
	vector<float> TempPVec(0);													// empty vector to store the percent while calculations take place.
	failFind();																	// locate the fail transition state values within the map

	cout << "\nPlease enter a "<< stateLengthMax <<" length character String: ";						// user enters a char string the size of the chosen state legnth. 
	cin >> stateTemp;																						// store in temp string
	
	cout << "\nPlease enter the percentage you wish to reach: ";											// User enters the target Percentage they want the calculation reach.
	float finishPercent = 0;																			
	cin >> finishPercent;																					
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
	

		for(int  i =0; i != failLocation.size(); i++){						// Increment 
			failPTotal = failPTotal + ResultPVec[failLocation[i]];
		}

		if(failPTotal >= finishPercent){
			endValueReached = true;
			cout << "\n\nPercentage of fail reached = " << failPTotal;
		}
	}

	cout << "\nEstimated days till this chance of fail is reached = " <<day <<"\n";

}		

void Markov::failFind(){
	
	int locationCount = 0;
	for (itCurrent= DataS.begin(); itCurrent != DataS.end(); ++itCurrent){					// Loop through all states stored in the Map.
		stateTemp = itCurrent->first;														// Assign the key to a temporary string.
		if(stateTemp.compare(stateLengthMax-1,1,absorbChar,0,1) == 0){												//Check if the state ends with a F character.
		failLocation.push_back (locationCount);											// stores the location of the Key with the fail state in a vector.
		}
	locationCount++;																		// increment the locationCount so it matches the location of the current map key. eg ItCurrent iterator points to the second key, 
	}																						// locationCount = 1 if this ended in an F it would be stored in a Vector.

}


#endif