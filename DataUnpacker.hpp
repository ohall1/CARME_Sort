#ifndef _DATAUNPACKER_HPP
#define _DATAUNPACKER_HPP

//#include "DataReader.cpp"
#include <utility>
#include <thread>

//class DataReader; //Let DataUnpacker know about the existence of DataReader so it can access it

class ADCDataItem{

	private:

		unsigned long timestamp;    //Internal AIDA time stamp
		unsigned long timestampLSB; // Least significant bits of timestamp

		unsigned int adcData;       //ADC value
		unsigned int sampleLength;

		unsigned int dataType;     // Data type: 0 = Sample waveform, 1 = Sample Length, 2 = Info Data, 3 = ADC Data
		int fee64ID;      //FEE64 ID
		unsigned int channelID;	// Channel ID
		unsigned int adcRange;		// ADC range: 0 = low energy, 1 = high energy

	public:
		ADCDataItem();
		~ADCDataItem(){};
		ADCDataItem(std::pair < unsigned int, unsigned int> inData);

		void SetTimestamp(unsigned long MSB);
};

class InformationDataItem{

	private:

		unsigned long timestamp;    		//Internal AIDA time stamp
		unsigned long timestampLSB; 		// Least significant bits of timestamp
		unsigned long timestampMSB;			// Most significant bits of timestamp
		unsigned long infoField;    		//Iformation field of AIDA data word

		unsigned int sampleLength;

		unsigned int dataType;     		// Data type: 0 = Sample waveform, 1 = Sample Length, 2 = Info Data, 3 = ADC Data
		unsigned int fee64ID;      		//FEE64 ID
		unsigned int infoCode;				//Info code of AIDA "information" word
		unsigned char corrScalerIndex;		//Index of correlation scaler
		unsigned long corrScalerTimestamp;	//Timestamp of correlation scaler

	public:
		InformationDataItem();
		~InformationDataItem(){};
		InformationDataItem(std::pair < unsigned int, unsigned int> inData);
		unsigned int GetInfoCode();
		unsigned long GetTimestampMSB();
		unsigned int GetCorrScalerIndex();
		unsigned long GetCorrScalerTimestamp();
		unsigned int GetFEE64ID();
		unsigned long GetTimestamp();
		unsigned long GetTimestampLSB();

		void SetTimestamp(unsigned long MSB);
};

class DataUnpacker{

	private:

		std::pair < unsigned int, unsigned int> dataWords;
		unsigned int word0, word1;

		unsigned long correlationScalerData0[24];		//Used in calculating the correlation scaler
		unsigned long correlationScalerData1[24];		//
		unsigned long correlationScaler;				//Correlation scaler between AIDA and other DAQs
		long int correlationScalerOffset;
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