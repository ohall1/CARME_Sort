#ifndef _DATAUNPACKER_HPP
#define _DATAUNPACKER_HPP

//#include "DataReader.cpp"
#include <utility>
#include <thread>

//class DataReader; //Let DataUnpacker know about the existence of DataReader so it can access it

class ADCDataItem{

	private:

		unsigned long timeStamp;    //Internal AIDA time stamp
		unsigned long timeStampLSB; // Least significant bits of timestamp

		unsigned int adcData;       //ADC value
		unsigned int sampleLength;

		unsigned char dataType;     // Data type: 0 = Sample waveform, 1 = Sample Length, 2 = Info Data, 3 = ADC Data
		unsigned char fee64ID;      //FEE64 ID
		unsigned char channelID;	// Channel ID
		unsigned char adcRange;		// ADC range: 0 = low energy, 1 = high energy

	public:
		ADCDataItem::ADCDataItem(std::pair < unsigned int, unsigned int> inData);
};

class InformationDataItem{

	private:

		unsigned long timeStamp;    		//Internal AIDA time stamp
		unsigned long timeStampLSB; 		// Least significant bits of timestamp
		unsigned long timeStampMSB;			// Most significant bits of timestamp
		unsigned long infoField;    		//Iformation field of AIDA data word

		unsigned int sampleLength;

		unsigned char dataType;     		// Data type: 0 = Sample waveform, 1 = Sample Length, 2 = Info Data, 3 = ADC Data
		unsigned char fee64ID;      		//FEE64 ID
		unsigned char infoCode;				//Info code of AIDA "information" word
		unsigned char corrScalerIndex;		//Index of correlation scaler
		unsigned char corrScalerTimeStamp;	//Timestamp of correlation scaler

	public:
		InformationDataItem::InformationDataItem(std::pair < unsigned int, unsigned int> inData);
};

class DataUnpacker{

	private:

		std::pair < unsigned int, unsigned int> dataWords;
		unsigned int word0, word1;

		unsigned long correlationScalerData0[24];		//Used in calculating the correlation scaler
		unsigned long correlationScalerData1[24];		//
		unsigned long correlationScaler;				//Correlation scaler between AIDA and other DAQs
		unsigned long timeStampMSB;						//Most significant bit of the timestamp

		unsigned int pauseItemCounter[24];
		unsigned int resumeItemCounter[24];
		unsigned int sync100Counter[24];

		unsigned char dataType;		//Data type of data words

		bool correlationStatus;		//Bool to keep track of whether correlation scaler has been measured
		bool timeStampMSBStatus;	//Bool to keep track of whether timeStampMSB has been set

		DataUnpacker::UnpackWords(std::pair < unsigned int, unsigned int> wordsIn)

	public:
		DataUnpacker();
		~DataUnpacker(){};
		void BeginDataUnpacker(DataReader & dataReader);

		unsigned long GetTimeStampMSB();
		unsigned char GetInfoCode();
		unsigned char GetCorrScalerIndex();
		unsigned char GetCorrScalerTimeStamp();

};


#endif