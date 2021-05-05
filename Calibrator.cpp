#include "Calibrator.hpp"
Calibrator::Calibrator(){};
void Calibrator::InitialiseCalibrator(std::string variablesFile, EventBuilder *eventBuilderPointIn){

	myEventBuilder = eventBuilderPointIn;
	eventBuilderStatus = true;

	for(int i =0; i < Common::noFEE64 ; i++){
		for(int j =0; j<Common::noChannel; j++){
			adcLowEnergyGain[i][j] = 0.7;	//keV/ch
			adcHighEnergyGain[i][j] = 0.7; 	//MeV/ch
		}
	}

	ReadInVariables(variablesFile);
}

void Calibrator::ReadInVariables(std::string variablesFile){

	int fee64, dssd, channelID;
	int side;
	double value;
	std::string line;

	for(int i =0; i < Common::noFEE64 ; i++){
		for(int j =0; j<Common::noChannel; j++){
			channelADCOffsets[i][j] = 0.0;
		}

		feeDSSDMap[i] = -1;
		feeSideMap[i] = -1;
		feeStripMap[i] =-1; 			
		feePolarityMap[i] = 0;
		feeWaferNumber[i] = 0;
	}


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
				if(dssd != -1 && fee64 <= Common::noFEE64){
					feeDSSDMap[fee64-1] = dssd;
				}

			}
			else if(dummyVar == "sideMap"){
				iss >> fee64;
				iss >> side;
				if (side != -1 && fee64 <= Common::noFEE64){
					feeSideMap[fee64-1] = side;
				}

			}
			else if(dummyVar == "stripMap"){
				iss >> fee64;
				iss >> value;
				if(fee64 <= Common::noFEE64){
					feeStripMap[fee64-1] = value;
				}

			}
			else if(dummyVar == "adcOffset"){
				iss >> fee64;
				iss >> channelID;
				iss >> value;
				if(value != -9999.99 && fee64 <= Common::noFEE64){
					channelADCOffsets[fee64-1][channelID] = value;
				}

			}
			else if(dummyVar == "adcGain"){
				iss >> fee64;
				iss >> channelID;
				iss >> value;
				if(value != -9999.99 && fee64 <= Common::noFEE64){
					adcLowEnergyGain[fee64-1][channelID] = value;
				}
			}
			else if (dummyVar == "adcPolarity"){
				iss >> fee64;
				iss >> value;
				if(fee64 <= Common::noFEE64){
					feePolarityMap[fee64-1] = value;
				}
			}
            else if (dummyVar == "waferNumber"){
                iss >> fee64;
                iss >> value;
                if(fee64 <= Common::noFEE64){
                    feeWaferNumber[fee64-1] = value;
                }
            }
			/*else{
				std::cout << "Problem in reading in variables file. Unrecognised parameter type - " << dummyVar.data() << "."<<std::endl;
				std::cout << "Program exiting" << std::endl;
				exit(-1);
				return;
			}*/

		}
	}// Variables all read in
	variables.close();
}
void Calibrator::ProcessEvents(){
	
	while(eventBuilderStatus){
		//Read in events list from buffer
		eventList = myEventBuilder->GetEventFromBuffer();
		myClustering.InitialiseClustering();
		while(eventList.size()>0){
			//Reads event from the front of the event list and initialises it as a calibrated data item
			calibratedItem.BuildItem(eventList.front());
			CalibrateData(eventList.front(),calibratedItem);
			eventList.pop_front();
			if(calibratedItem.GetDSSD()>=0){
				myClustering.AddEventToMap(calibratedItem);
			}
		}
		//Once all items from an event has been read in and stored in maps. Begin clustering
		if(eventBuilderStatus){
			myClustering.ProcessMaps();
		}
		else{
			std::cout << "I'm out " << std::endl;
		}

	}

	return;
}
double Calibrator::GetOrder(int channelID) const{
	return feeChannelOrder[channelID];
}
void Calibrator::CalibrateData(ADCDataItem & adcDataItemIn, CalibratedADCDataItem & calibratedItemOut){

	SetGeometry(adcDataItemIn, calibratedItemOut);
	eventBuilderStatus = CalibrateEnergy(adcDataItemIn, calibratedItemOut);

}
void Calibrator::SetGeometry(ADCDataItem & adcDataItemIn, CalibratedADCDataItem & calibratedItemOut){

	//FEE channel does not map perfectly to strip channels. Instead need to set the geometry of each detector depending on which FEE position and which channel.

	calibratedItemOut.SetDSSD(feeDSSDMap[adcDataItemIn.GetFEE64ID()-1]-1);
	if(feeStripMap[adcDataItemIn.GetFEE64ID()-1] == 1){
		calibratedItemOut.SetStrip(GetOrder(adcDataItemIn.GetChannelID())
		                                + (128 * feeWaferNumber[adcDataItemIn.GetFEE64ID()-1]));
			}
	else if(feeStripMap[adcDataItemIn.GetFEE64ID()-1] == 2){
		calibratedItemOut.SetStrip(127.-GetOrder(adcDataItemIn.GetChannelID())
                                        + (128 * feeWaferNumber[adcDataItemIn.GetFEE64ID()-1]));
	}
	else if(feeStripMap[adcDataItemIn.GetFEE64ID()-1] == -1){}
	else{
		std::cout << "Warning! FEE mapped to strip map improperly check variables file." << std::endl;
		std::cout << "FEE " << adcDataItemIn.GetFEE64ID() << std::endl;
		std::cout << "Channel " << adcDataItemIn.GetChannelID() << std::endl;
	}
	calibratedItemOut.SetSide(feeSideMap[adcDataItemIn.GetFEE64ID()-1]);
}
bool Calibrator::CalibrateEnergy(ADCDataItem & adcDataItemIn, CalibratedADCDataItem & calibratedItemOut){

	//Takes the raw ADC data and applies the offsets and the polarity of the signal.

	if(adcDataItemIn.GetADCRange() == 0){
		//Low energy event
		calibratedItemOut.SetADCRange(0);

		//Calibration method for Chris' offsets
		//calibratedItemOut.SetEnergy(((double)adcDataItemIn.GetADCData()-adcZero - (double)channelADCOffsets[adcDataItemIn.GetFEE64ID()-1][adcDataItemIn.GetChannelID()])
				//*(double)feePolarityMap[adcDataItemIn.GetFEE64ID()-1]*adcLowEnergyGain[adcDataItemIn.GetFEE64ID()-1][adcDataItemIn.GetChannelID()]);

		//Calibration method for Tom's offsets
		calibratedItemOut.SetEnergy(((((double)adcDataItemIn.GetADCData()-adcZero)*(double)feePolarityMap[adcDataItemIn.GetFEE64ID()-1])
				- channelADCOffsets[adcDataItemIn.GetFEE64ID()-1][adcDataItemIn.GetChannelID()])*adcLowEnergyGain[adcDataItemIn.GetFEE64ID()-1][adcDataItemIn.GetChannelID()]);

		/*std::cout << "ADCData: " << adcDataItemIn.GetADCData() << std::endl;
		std::cout << "FEE: " << adcDataItemIn.GetFEE64ID() << " Channel: " << adcDataItemIn.GetChannelID() << " Offset: " << channelADCOffsets[adcDataItemIn.GetFEE64ID()-1][adcDataItemIn.GetChannelID()] << std::endl;
		std::cout << "Calibrated Energy: " << calibratedItemOut.GetEnergy() <<std::endl;	*/

		return true;
	}
	else if(adcDataItemIn.GetADCRange() == 1){
		//High energy event
		calibratedItemOut.SetADCRange(1);
		calibratedItemOut.SetEnergy((	(double)adcDataItemIn.GetADCData()-adcZero)*(double)feePolarityMap[adcDataItemIn.GetFEE64ID()-1]
										*adcHighEnergyGain[adcDataItemIn.GetFEE64ID()-1][adcDataItemIn.GetChannelID()]);

		return true;
	}
	else if(adcDataItemIn.GetADCRange() == 2){
		return false;
	}
}
void Calibrator::CloseCalibrator(){
	std::cout << "Calibrator thread finished" <<std::endl;
	myClustering.CloseClustering();
}
