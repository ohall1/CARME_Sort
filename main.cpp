// C++ include.
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>
#include<vector>
#include<map>
#include<thread>
#include<mutex>

//ROOT Libraries
#include "TFile.h"




#include "DataReader.cpp"
#include "DataUnpacker.cpp"
#include "DataItems.cpp"
#include "EventBuilder.cpp"
#include "Calibrator.cpp"

void Usage(char *progname){
	std::cout << "Usage: AIDASort -c configFile -o OutputFile" << std::endl;
	exit;
}

int main(int argc, char **argv){

	std::string runName, line;
	std::string configFile, aidaParameters;
	std::string inFile, outFile, userOutFile;
	std::string dataDir, outputDir;
	int runNum, subRunStart, subRunEnd;
	runNum=0;
	subRunStart = 0;
	subRunEnd = 0;

	//Define root files

	std::list<std::string> AIDAFileList;

	int runNumber, subRunMin, SubRunMax;

	if (argc > 3){
		std::cout << "Run time options:"<< std::endl;

		for (int i = 1; i <argc;i++){
			std::cout << argv[i] << std::endl;

			if ( (argv[i][0] == '-') || (argv[i][0] == '/') ) {
				switch( argv[i][1]){

					case 'c':
						configFile = argv[++i];
						std::cout << "Configuration file: "<< configFile << std::endl;
						break;

					case 'o':
						userOutFile = argv[++i];
						std::cout << "User output file: " << userOutFile << std::endl;
						break;

					default:
						Usage(argv[0]);
						break;

				}
			}
			else Usage(argv[0]);
		}
	}// End of reading in command line arguments
	else Usage(argv[0]);

	std::ifstream confFile(configFile.data());
	while ( confFile.good() ){
		getline(confFile,line);
		auto commentLine=line.find("#");
		std::string dummyVar;
		auto newLine=line.substr(0,commentLine);
		if(newLine.size()>0){std::istringstream iss(line,std::istringstream::in);

			#ifdef DEB
				std::cout << newLine << std::endl;
			#endif	

			iss >> dummyVar;
			
			if (dummyVar == "AIDAFile"){
				iss>>dummyVar;
				if ( AIDAFileList.size() == 0){
					AIDAFileList.push_back(dummyVar);
					std::cout << "Added file: " << dummyVar << " to be sorted" <<std::endl;
				}
				else {
					std::cout << "AIDAFile configure error" << std::endl;
					std::cout << "AIDA files already defined. Please only use one method of defining files" << std::endl;
				}
			}//End of AIDAFIle
			else if (dummyVar == "AIDAList"){
				iss>>runName;
				iss>>runNum;
				iss>>subRunStart;
				iss>>subRunEnd;

				if (AIDAFileList.size()==0){
					std::cout << "Adding files to be sorted:" << std::endl;
					for(int i=subRunStart; i<=subRunEnd; i++){
						dummyVar = runName + std::to_string(runNum) + "_" + std::to_string(i);
						std::cout << dummyVar << std::endl;
						AIDAFileList.push_back(dummyVar);
					}
				}
				else {
					std::cout << "AIDAList configure error" << std::endl;
					std::cout << "AIDA files already defined. Please only use one method of defining files" << std::endl;
				}
			}//End of AIDAList
			else if (dummyVar == "AIDAConfig"){
				iss >> aidaParameters;
			}//End of AIDAConfig
		}//End of if (newLine)

	}// End of reading in configuration file

	TFile * fOutRoot = new TFile(userOutFile.data(),"RECREATE");
	if (!fOutRoot){
		std::cout << "Problem opening output file check input for file name" << std::endl;
		return -1;
	}

	//Initialise data reader
	DataReader myDataReader;
	DataUnpacker myDataUnpacker;
	Calibrator myCalibrator;

	DataReader *dataReaderPoint;
	DataUnpacker *dataUnpackerPoint;
	Calibrator *calibratorPoint;

	dataReaderPoint = &myDataReader;
	dataUnpackerPoint = &myDataUnpacker;

	myDataReader.InitialiseReader(AIDAFileList);
	myDataUnpacker.InitialiseDataUnpacker();
	myCalibrator.InitialiseCalibrator(aidaParameters);

	std::thread th1 (&DataReader::BeginReader,dataReaderPoint);

	std::thread th2 (&DataUnpacker::BeginDataUnpacker,dataUnpackerPoint,std::ref(myDataReader));

	th1.join();
	th2.join();


}