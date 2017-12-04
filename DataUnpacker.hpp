#ifndef _DATAUNPACKER_HPP
#define _DATAUNPACKER_HPP

//#include "DataReader.cpp"
#include <utility>
#include <thread>

//class DataReader; //Let DataUnpacker know about the existence of DataReader so it can access it

class DataUnpacker{

	private:

		std::pair < unsigned int, unsigned int> dataWords;
		unsigned int word0, word1;

		DataUnpacker::UnpackWords(std::pair < unsigned int, unsigned int> wordsIn)

	public:
		DataUnpacker();
		~DataUnpacker(){};
		void BeginDataUnpacker(DataReader & dataReader);

};


#endif