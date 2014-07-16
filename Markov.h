#ifndef File_read_H
#define	File_read_H

#include <sstream>
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
	vector<string> fileStore;
	string states;
	string stateTemp;
	string absorbChar;
	map<string, char>eventNameMap;
	map<string, char>::iterator itEventNameMap;
	map<string, vector<int>> EventStatePosition;
	map<string, vector<int>>::iterator itEventState;
	map<string, int> eventIndex;
	map<string, int>::iterator itEventIndex;
	map<string, vector<float>> DataS;
	map<string, vector<float>>::iterator itCurrent;
	vector<int> failLocation;
	int stateLengthMax;
	int absorbFound;
	bool absorbMode;

public:
	void printDataS();
	void makeEventAllocationsFile();
	void loadStringFile();
	void generateMaps();
	void generateStateMap();
	void userCalc();
	Markov();

private:
	void convertAndStoreData();
	void userMapDef();
	void loadStateString(int shiftRight,int vecValue);
	void mapKeyStore();
	void mapKeyPositions(int stateLocation);
	void populateStateMap();
	void eventMapIndex();
	void fillMarkovValues();
	void markovValueCheck();
	void failFind();
	bool fillAbsorbValues(vector<float>& vecData);
	bool endValueReached(vector<float> resultVector, float finishP);
	
};

Markov::Markov(){

	bool userFinished = false;
	char userEnd;

	loadStringFile();
	makeEventAllocationsFile();
	
	userMapDef();
	generateMaps();

	while( userFinished == false){

		if(absorbMode == true){
			userCalc();
		}
	printDataS();
		cout << "\n\nEnter y to restart / q to quit ";
		cin >> userEnd;

		if( userEnd == 'q'){
			userFinished = true;
		}
	}
}

void Markov::printDataS(){

	ofstream myfile;
	myfile.open ("MarkovMap.csv");																	// Start making the file we will store the events and an allocated character in.

	map<string, vector<float> >::iterator itr;

	for(itr = DataS.begin(); itr != DataS.end(); ++itr)
	{
		myfile << itr->first << endl;
		myfile << " ";
		for(std::vector<float>::iterator j = itr->second.begin(); j != (itr->second).end(); ++j)
		{
		myfile << " " << *j;
		}
	myfile << endl;
	}
	myfile.close();
}

void Markov::loadStringFile(){															// Opens the previously generated file and increments through the string printing and storing each character to the screen
	
	ifstream myfile2 ("VEHICLE_HISTORY_Test_Data.csv");													// open file and assign to string 
	string states;

	if (myfile2.is_open()){

		while ( myfile2.good()){

			while (getline(myfile2,states)){

				stringstream ss(states);
				string field;

				for(int i = 0; i < 2; ++i){

					(getline(ss, field, '\t'));	
					if(i == 1){
						fileStore.push_back(field);
					}				
				}		
			}
		}

		myfile2.close();

	}else{

		cout << "Unable to open file states"; 
	}
}

void Markov::makeEventAllocationsFile(){												

	char letter = 'A';

	for (int itVec = 0; itVec != fileStore.size(); ++itVec){													// Store all events that appeared in the data sample and assign a char that will be changed later.

		eventNameMap[fileStore[itVec]] = 'a';		
	}

	ofstream myfile;
	myfile.open ("eventNamevalues.csv");																	// Start making the file we will store the events and an allocated character in.

	for(itEventNameMap = eventNameMap.begin(); itEventNameMap != eventNameMap.end(); itEventNameMap++){		//loop through the map with the name of all unique events that appear in the data.

		itEventNameMap->second = letter;																	// Assign a letter to the value that will be a unique representation of each event.
		letter++;
		myfile << itEventNameMap->second << " : " << itEventNameMap->first << " \n";						// store the key and the value in a file.
	}

	myfile.close();		
	convertAndStoreData();																					//Convert the events data to the characters we have assigned the events.
	

}																

void Markov::convertAndStoreData(){

	for (int itVec = 0; itVec != fileStore.size(); ++itVec){													//Go through the entire event data file that was loaded into the vector fileStore.

		for(itEventNameMap = eventNameMap.begin(); itEventNameMap != eventNameMap.end(); itEventNameMap++){		//for each event in the fileStore vector look through the entire name map.

			if(itEventNameMap->first == fileStore[itVec]){														//If the event in the map key matches the event in the fileStore replace the event with the assigned character.
				fileStore[itVec] = itEventNameMap->second;
				
			}
		}
	}

	ofstream myfile;
	myfile.open ("TestDataConv1.csv");																			//Make a new data file with the now converted events to characters.

	for (int itVec = 0; itVec != fileStore.size(); ++itVec){	
		myfile <<  fileStore[itVec];
	}

	myfile.close();
}

void Markov::userMapDef(){

	cout << "\n\nEnter the size of each state: ";
	cin >> stateLengthMax;
	stateTemp.resize(stateLengthMax);														//user defines the length of a state.	

	cout << "\nIf absorb state exists enter character, else enter \"null\":";
	cin >> absorbChar;

	if(absorbChar.find('null') == string::npos){											//Define if special considerations need to be taken in the map generation.
		absorbMode = true;
	}else{
		absorbMode = false;
	}	 

}

void Markov::generateMaps(){

	for(int generateStage = 1; generateStage < 3; generateStage++){

		for(int itVec = 0; itVec != fileStore.size()-(stateLengthMax-1); ++itVec){					//Look through all the events in fileStore Vector											
			
			loadStateString(0,itVec);

			switch(generateStage){
				case 1:
					mapKeyStore();																	//Store the stateTemp's loaded from fileStore keys in the Maps.
				break;
				case 2:
					mapKeyPositions(itVec);															//Locate and store the position of key occurances within fileStore.	
				break;
			}			
		}
	}
	populateStateMap();																				//Fill the map that will become our state Map with blank float vectors of map.size() length.
	eventMapIndex();																				// allocate a number to each map key so we can access the vectors locations faster.
	failFind();																						// locate the fail transition state values within the map
	fillMarkovValues();
	markovValueCheck();																				// Check that each vector adds up to a total value of 1 i.e. 100%
}

void Markov::loadStateString(int shiftRight,int vecValue){

	stateTemp.clear();

	
	vecValue = vecValue + shiftRight;													// Shift right by one to look at the state that comes after our current state.
																						// add out of range detection.
	if( (vecValue + (stateLengthMax-1) + shiftRight) < fileStore.size()){

		for(unsigned k = 0; k != stateLengthMax; k++){									// group a number of vector elements stateLengthMax long from the itVec position into a string.		

			stateTemp = stateTemp + fileStore[vecValue + k];							// add each char to the string to create our state for the map.
		}
	}
}

void Markov::mapKeyStore(){																			// Stores the stateLengthMax sized string as a key in the map as long as an absorbing state is not found at a position other than the end.
	
	if(absorbMode == true){																			// If absorb map is set additional considerations required

		if((absorbFound = stateTemp.find(absorbChar)) >= 0 && absorbFound <= stateLengthMax-2){     // Absorb character must appear at the end of the state grouping or else it is not a valid state. Nothing comes after and absorbing state.
			return;			
		}else{
			DataS[stateTemp] = vector<float>();
			EventStatePosition[stateTemp]= vector<int>();
			eventIndex[stateTemp];
		}

	}else{																							// Create 3 maps with identical keys.
		DataS[stateTemp] = vector<float>();															// Markov probability map
		EventStatePosition[stateTemp]= vector<int>();												// location the states appear in the fileStore vector.
		eventIndex[stateTemp];																		// Index for speedy access of the vectors of the probability map.
	}	
}

void Markov::mapKeyPositions(int stateLocation){

	if(EventStatePosition.find(stateTemp) != EventStatePosition.end()){											//Make sure the state is in the map. Fixes Issue caused by absorbMode as some states that appear in raw data are ignored.

		itEventState = EventStatePosition.find(stateTemp); 														//Find the key that matches the loaded state from the raw data.
		itEventState->second.push_back(stateLocation);															//Store the value itEventState which is pointing to the first character in our loaded string from the fileStore Vector into the map value location.																												
		
	}
}

void Markov::populateStateMap(){

	for (itCurrent = DataS.begin(); itCurrent != DataS.end(); ++itCurrent){			// Prepare the Markov map with vectors that are the same size as the number of keys in the map.

		vector<float> tempVec(DataS.size());	
		DataS[itCurrent->first].swap(tempVec);
	}

}

void Markov::eventMapIndex(){														//Assign an int to each key of the map.

	int indexValue = 0;

	for (itEventIndex = eventIndex.begin(); itEventIndex != eventIndex.end(); ++itEventIndex){
	
		eventIndex[itEventIndex->first] = indexValue;
		indexValue++;
	}

}

void Markov::failFind(){
	
	for (itEventIndex = eventIndex.begin(); itEventIndex != eventIndex.end(); ++itEventIndex){		// Loop through all states stored in the Map.

		stateTemp = itEventIndex->first;															// Assign the key to a temporary string.

		if(stateTemp.compare(stateLengthMax-1,1,absorbChar,0,1) == 0){								//Check if the state ends with an Absorb character.
			failLocation.push_back (itEventIndex->second);												// stores the location of the Key with the fail state in a vector.
		}																							// increment the locationCount so it matches the location of the current map key. eg ItCurrent iterator points to the second key, 
	}																						

}

void Markov::fillMarkovValues(){

	float totalEventCount;																					

	for(itEventState = EventStatePosition.begin(); itEventState != EventStatePosition.end(); ++itEventState){			//Loop through our event positions

		vector<int> tempVecPos;
		vector<float> tempVecData;

		itCurrent = DataS.find(itEventState->first);																	//find tha iterator that points to the same stateTemp value in the datastore.
		DataS[itCurrent->first].swap(tempVecData);																		//Swap out the vector we need to manipulate.
	
		if(fillAbsorbValues(tempVecData) == false){																		//Absorb skip start. If it is an absorbing state it has 100% chance to transition to itself therefore the rest of the calculation is not required.
		
			EventStatePosition[itEventState->first].swap(tempVecPos);													//Swap out the vector storing all the locations of the key state.
			totalEventCount = tempVecPos.size();																		//Get a total count of how many times this state appears in fileStore
			
			for(int itVec = 0; itVec != tempVecPos.size(); ++itVec){													//Look at every possible location that the state appears in fileStore

				int statePos = tempVecPos[itVec];																		//load the postions that this state appears in fileStore.
				loadStateString(1,statePos);																			//shift 1 place to the right to load the next state.

				if(eventIndex.find(stateTemp) != eventIndex.end()){														//Check the stateTemp exsists in our maps

					itEventIndex = eventIndex.find(stateTemp);															//find the iterator that points to the stateTemp.	

					tempVecData[itEventIndex->second] = tempVecData[itEventIndex->second] + 1;							//Add one to the correct location state current -> state next.
				}
			}

			for(int itVec = 0; itVec != tempVecData.size(); ++itVec){													// Go through every element in the loaded Data Vector and divide the next state counts by the current state total.

				if(tempVecData[itVec] != 0){																			// Ignore 0 values.
					tempVecData[itVec] = tempVecData[itVec] / totalEventCount;											// divide and store in the same location giving us a float percentage value.
				}
			}
																														//absorb skip end.
		}
		DataS[itCurrent->first].swap(tempVecData);																		//Replace the vectors so it is ready to start again on the next iteration.
		EventStatePosition[itEventState->first].swap(tempVecPos);

	}

}

bool Markov::fillAbsorbValues(vector<float>& vecData){												// Set absorb states to 1.

	bool vecDataEdit = false;
	itEventIndex = eventIndex.find(itCurrent->first);												//Find the state in the index and store the iterator location.
	for(int i = 0; i < failLocation.size(); ++i){													//Use the failLocation values which indicate the location of absorb states.

		if(failLocation[i] == itEventIndex->second){												//Check if the failLocation matches the state index.
			vecData[failLocation[i]] = 1;														    //If it matches then a 1 is put in that location in the DataS vector.
			vecDataEdit = true;																		//Set this to true so we can skip the rest of the calulation.
		}


	}
	return vecDataEdit;
}

void Markov::markovValueCheck(){																				//Check the total of each vector adds up to 1.

	vector<float> tempVecData;

	for (itCurrent = DataS.begin(); itCurrent != DataS.end(); ++itCurrent){
		
		float vectorTotal = 0;
		DataS[itCurrent->first].swap(tempVecData);

		for(int itVec = 0; itVec != tempVecData.size(); ++itVec){

			vectorTotal = tempVecData[itVec] + vectorTotal;

		}

		cout << "\nState: " << itCurrent->first << " Total: " << vectorTotal;
		DataS[itCurrent->first].swap(tempVecData);
	}


}

void Markov::userCalc(){

	int day = 0;
	int RunningTotal = 0;
	
	vector<float> MapVec(0);																	// empty vector to swap with the vector stored in the map.
	vector<float> ResultPVec(0);																// empty vector to store the initial state percents.
	vector<float> TempPVec(0);																	// empty vector to store the percent while calculations take place.
	

	cout << "\nPlease enter a "<< stateLengthMax <<" length character String: ";				// user enters a char string the size of the chosen state legnth. 
	cin >> stateTemp;																			// store in temp string
	
	cout << "\nPlease enter the percentage you wish to reach: ";								// User enters the target Percentage they want the calculation reach.
	float finishPercent = 0;																			
	cin >> finishPercent;																																								

	itCurrent = DataS.find(stateTemp);															//Checks if the state the user entered matches the state the iterator points to.	
	ResultPVec = DataS[itCurrent->first];														// Copy the vector into a new Vector that will store the results of the calculation.																

																			
	while(endValueReached(ResultPVec, finishPercent) == false){									// Continue untill this flag is changed by the Chosen Percent == the Calculated Percent.

		TempPVec.resize(ResultPVec.size());														//Resize a Temporary vector to the legnth of the map vectors.

		for (int MapVecLength = 0; MapVecLength != ResultPVec.size(); MapVecLength++){			// Matrix multiplication

			itCurrent = DataS.begin();

			for(int ResultPLength = 0; ResultPLength != ResultPVec.size(); ResultPLength++){
				MapVec = DataS[itCurrent->first];
				TempPVec[MapVecLength] += ResultPVec[ResultPLength] * MapVec[MapVecLength];		
				itCurrent++;
			}
		}

		day++;
		ResultPVec = TempPVec;													// store the finished calculation into the result vector.
		TempPVec.clear();														// clear the Temp vector for the next calculation.	
	}

	cout << "\nEstimated days till this chance of fail is reached = " <<day <<"\n";

}		

bool Markov::endValueReached(vector<float> resultVector, float finishP){

	float failPTotal = 0;													//Used to add up all states that are fails.
	bool valueReached = false;

		for(int  i =0; i != failLocation.size(); i++){						//Look at all locations in the vector that are fail states.

			failPTotal = failPTotal + resultVector[failLocation[i]];		//Add the transition percentage for each fail state.

		}

		if(failPTotal >= finishP){

			valueReached = true;
			cout << "\n\nPercentage of fail reached = " << failPTotal;

		}

	return valueReached;
}


#endif