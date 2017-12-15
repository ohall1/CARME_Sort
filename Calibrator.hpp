#ifndef _CALIBRATOR_HPP
#define _CALIBRATOR_HPP

#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstdlib>

#include "DataItems.hpp"
#include "EventBuilder.hpp"

class Calibrator{

	private:

		std::string variablesFile;
		EventBuilder *myEventBuilder;
		std::list <ADCDataItem> eventList;		//Event list read in from buffer

		const unsigned int feeChannelOrder[64]={62, 63, 59, 60, 61, 56, 57, 58, 52, 53, 54, 55, 49, 50, 51, 45,
												46, 47, 48, 42, 43, 44, 38, 39, 40, 41, 35, 36, 37, 31, 32, 33,
			    								34, 28, 29, 30, 24, 25, 26, 27, 21, 22, 23, 17, 18, 19, 20, 14,
												15, 16, 10, 11, 12,  7,  3,  0,  8,  4,  1,  9,  5,  2, 13,  6};

		const int adcZero = 23768; 				//2**15

		int feeDSSDMap[24];						//Which DSSD does a FEE correspond to
		int feeSideMap[24];						//What side of the detector is a FEE64
		int feeStripMap[24]; 					//How does the FEE Map to the DSSD (1:Left/Bottom or 2: Right/Top)
		int feePolarityMap[24];					//What is the polarity of the FEE;
		int channelADCOffsets[24][64];			//Whats the channel ADC offset

		int adcLowEnergyGain[24][64];			//Gain applied to low energy channels. Defaults to 0.7keV/ch
		int adcHighEnergyGain[24][64];			//Gain applied to high energy channels. Defaults to 0.7MeV/ch

		void ReadInVariables(std::string variablesFile);
		void SetGeometry(ADCDataItem & adcDataItemIn, CalibratedADCDataItem & calibratedItemOut);
		void CalibrateData(ADCDataItem & adcDataItemIn, CalibratedADCDataItem & calibratedItemOut);
		void CalibrateEnergy(ADCDataItem & adcDataItemIn, CalibratedADCDataItem & calibratedItemOut);
		unsigned int GetOrder(int channelID) const;

	public:
		Calibrator();
		~Calibrator(){};

		void InitialiseCalibrator(std::string valirablesFile, EventBuilder * eventBuilderPointIn);
		void ProcessEvents();

		

};

#endif