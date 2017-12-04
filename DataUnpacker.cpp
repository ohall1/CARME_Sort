#include "DataUnpacker.hpp"
#include "DataReader.hpp"

DataUnpacker::DataUnpacker(){};

void DataUnpacker::BeginDataUnpacker(DataReader & dataReader){

	for(;;){
	dataWords = dataReader.ReadFromBuffer();
	//std::cout << "first word " << dataWords.first << " second word " << dataWords.second << std::endl;

	UnpackWords(dataWords);

	}
}

void DataUnpacker::UnpackWords(std::pair < unsigned int, unsigned int> wordsIn){
	
}
