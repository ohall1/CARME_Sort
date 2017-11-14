// C++ include.
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>

void Usage(char *progname){
	std::cout << "Usage: AIDASort -c configFile -o OutputFile (Optional)" << std::endl;
	exit 1;
}

int main(int argc, char **argv){

	std::string runName;
	std::string configFile, aidaParameters;
	std::string inFile, outFile, userOutFile;
	std::string dataDir, outputDir;
	int runNum, subRunStart, subRunEnd;
	runNum=0;
	subRunStart = 0;
	subRunEnd = 0;

	std::list<<std::string>> AIDAFileList;

	int runNumber, subRunMin, SubRunMax;

	if (argc > 0){
		std::cout << "Run time options:"<< std::endl;

		for (int i = 1, i <argc;i++){
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
	}
	else Usage(argv[0]);

	std::ifstream confFile(configFile.data());
	while ( confFile.good() ){
		getline(confFile,Line);
		auto commentLine=line.find("#");
		std::string dummyVar;
		auto newLine=line.substr(0,commentLine);
		if(newLine.size()>0){std::istringstream iss(line,std::istringstream::in)};

		#ifdef DEB
			std::cout << newLine << std::endl;
		#endif

		iss >> dummyVar;
		
		if (dummyVar == "AIDAFile"){
			iss>>dummyVar;
			if ( runNum == 0){
				AIDAFileList.insert(dummyVar);
			}
		}
		else if (dummyVar == "AIDAList"){
			iss>>runName;
			iss>>runNum;
			iss>>subRunStart;
			iss>>subRunEnd;
			for(int i==subRunStart; i<=subRunMin; i++){
				dummyVar = runName + std::to_string(runNum) + "_" + std::to_string(i);
				AIDAFileList.insert(dummyVar);
			}
		}
		else if (dummyVar == "AIDAConfig"){
			iss >> aidaParameters;
		}

	}



}