#include "Calibrator.hpp"

Calibrator::Calibrator(){};

void Calibrator::InitialiseCalibrator(std::string variablesFile){

	ReadInVariables(variablesFile);

}
void Calibrator::ReadInVariables(std::string variablesFile){

	int fee64, dssd, channelID;
	int value, side;
	std::string line;

	std::ifstream variables(variablesFile.data());
	while (variables.good()){
		getline(variables,line);
		auto commentLine=line.find("#");
		std::string dummyVar;
		auto newLine=line.substr(0,commentLine);
		if(newLine.size()>0){

			std::istringstream iss(line,std::istringstream::in);

			#ifdef DEB_CALIBRATOR
				std::cout << newLine << std::endl;
			#endif	

			iss >> dummyVar;

			if(dummyVar == "dssdMap"){
				iss >> fee64;
				iss >> dssd;
				feeDSSDMap[fee64] = dssd;

			}
			else if(dummyVar == "sideMap"){
				iss >> fee64;
				iss >> side;
				feeSideMap[fee64] = side;

			}
			else if(dummyVar == "stripMap"){
				iss >> fee64;
				iss >> value;
				feeStripMap[fee64] = value;

			}
			else if(dummyVar == "adcOffset"){
				iss >> fee64;
				iss >> channelID;
				iss >> value;
				channelADCOffsets[fee64][channelID] = value;

			}
			else{
				std::cout << "Problem in reading in variables file. Unrecognised parameter type." <<std::endl;
				std::cout << "Program exiting" << std::endl;
				exit(-1);
				return;
			}

		}
	}
}