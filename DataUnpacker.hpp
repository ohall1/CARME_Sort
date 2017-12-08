#ifndef _DATAUNPACKER_HPP
#define _DATAUNPACKER_HPP
#include "DataReader.hpp"
#include "DataItems.hpp"
#include "EventBuilder.hpp"

#include <utility>
#include <thread>

//class DataReader; //Let DataUnpacker know about the existence of DataReader so it can access it

class DataUnpacker{

	private:

		EventBuilder myEventBuilder;

		std::pair < unsigned int, unsigned int> dataWords;
		unsigned int word0, word1;

		unsigned long correlationScalerData0[24];		//Used in calculating the correlation scaler
		unsigned long correlationScalerData1[24];		//
		unsigned long correlationScaler;				//Correlation scaler between AIDA and other DAQs
		long int correlationScalerOffset;				//Offset between correlation scaler and AIDA timstamp
		bool correlationScalerStatus;					//Status off correlation scaler
		unsigned long timestampMSB;						//Most significant bit of the timestamp

		unsigned int pauseItemCounter[24];
		unsigned int resumeItemCounter[24];
		unsigned int sync100Counter[24];

		unsigned int dataType;		//Data type of data words

		bool correlationStatus;		//Bool to keep track of whether correlation scaler has been measured
		bool timestampMSBStatus;	//Bool to keep track of whether timeStampMSB has been set

		void UnpackWords(std::pair < unsigned int, unsigned int> wordsIn);

	public:
		DataUnpacker();
		~DataUnpacker(){};
		void BeginDataUnpacker(DataReader & dataReader);
		void InitialiseDataUnpacker();

};


#endif