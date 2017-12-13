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

		int feeDSSDMap[24];				//Which DSSD does a FEE correspond to
		int feeSideMap[24];				//What side of the detector is a FEE64
		int feeStripMap[24]; 			//How does the FEE Map to the DSSD (1:Left/Bottom or 2: Right/Top)
		int channelADCOffsets[24][64];	//Whats the channel ADC offset

		void ReadInVariables(std::string variablesFile);
		CalibratedADCDataItem SetGeometry(ADCDataItem & adcDataItemIn);

	public:
		Calibrator();
		~Calibrator(){};

		void InitialiseCalibrator(std::string valirablesFile);

		

};

#endif