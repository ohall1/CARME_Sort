#ifndef _DATAUNPACKER_HPP
#define _DATAUNPACKER_HPP
#include "DataReader.hpp"
#include "DataItems.hpp"
#include "EventBuilder.hpp"
#include "Common.hpp"

//C++ INCLUDES
#include <utility>
#include <thread>

//ROOT INCLUDES
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"

//class DataReader; //Let DataUnpacker know about the existence of DataReader so it can access it

class DataUnpacker{

	private:

		EventBuilder myEventBuilder;
		bool dataCheck;


		std::list <std::pair<unsigned int, unsigned int>> dataWordsList;
		std::list <std::pair<unsigned int, unsigned int>>::iterator dataWordsListIt;
		std::pair < unsigned int, unsigned int> dataWords;
		unsigned int word0, word1;

		unsigned long correlationScalerData0[Common::noFEE64];		//Used in calculating the correlation scaler
		unsigned long correlationScalerData1[Common::noFEE64];		//
		unsigned long correlationScaler;				//Correlation scaler between AIDA and other DAQs
		long int correlationScalerOffset;				//Offset between correlation scaler and AIDA timstamp
		bool correlationScalerStatus;					//Status off correlation scaler
		unsigned long timestampMSB;						//Most significant bit of the timestamp

		unsigned int pauseItemCounter[Common::noFEE64];
		unsigned int resumeItemCounter[Common::noFEE64];
		unsigned int sync100Counter[Common::noFEE64];

		unsigned int totalPauseItem;
		unsigned int totalResumeItem;
		unsigned int totalSYNC100;
		unsigned long totalDataWords;
		unsigned long totalImplantWords;
		unsigned long totalDecayWords;
		unsigned int dataType;		//Data type of data words

		bool correlationStatus;		//Bool to keep track of whether correlation scaler has been measured
		bool timestampMSBStatus;	//Bool to keep track of whether timeStampMSB has been set

		//Histrogramming declarations
		#ifdef HISTOGRAMMING

		TH1D * timestampADCData;
		TH1D * timestampLowEnergy;
		TH1D * timestampHighEnergy;
		TH2D * lowEnergyChannelADC;
		TH2D * highEnergyChannelADC;

		#endif

		bool UnpackWords(std::pair < unsigned int, unsigned int> wordsIn);

		

	public:
		DataUnpacker();
		~DataUnpacker(){};
		void BeginDataUnpacker(DataReader & dataReader);
		EventBuilder * InitialiseDataUnpacker();
		void CloseUnpacker();
};


#endif