#include "Calibrator.hpp"
Calibrator::Calibrator(){};
void Calibrator::InitialiseCalibrator(std::string variablesFile, EventBuilder *eventBuilderPointIn){

	ReadInVariables(variablesFile);
	myEventBuilder = eventBuilderPointIn;

	for(int i =0; i < Common::noFEE64 ; i++){
		for(int j =0; j<Common::noChannel; j++){
			adcLowEnergyGain[i][j] = 0.7;	//keV/ch
			adcHighEnergyGain[i][j] = 0.7; 	//MeV/ch
		}
	}

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
				feeDSSDMap[fee64-1] = dssd;

			}
			else if(dummyVar == "sideMap"){
				iss >> fee64;
				iss >> side;
				feeSideMap[fee64-1] = side;

			}
			else if(dummyVar == "stripMap"){
				iss >> fee64;
				iss >> value;
				feeStripMap[fee64-1] = value;

			}
			else if(dummyVar == "adcOffset"){
				iss >> fee64;
				iss >> channelID;
				iss >> value;
				channelADCOffsets[fee64-1][channelID] = value;

			}
			else if (dummyVar == "adcPolarity"){
				iss >> fee64;
				iss >> value;
				feePolarityMap[fee64-1] = value;
			}
			/*else{
				std::cout << "Problem in reading in variables file. Unrecognised parameter type - " << dummyVar.data() << "."<<std::endl;
				std::cout << "Program exiting" << std::endl;
				exit(-1);
				return;
			}*/

		}
	}
}
void Calibrator::ProcessEvents(){
	
	while(true){
		//Read in events list from buffer
		eventList = myEventBuilder->GetEventFromBuffer();
		myClustering.InitialiseClustering();
		while(eventList.size()>0){
			//Reads event from the front of the event list and initialises it as a calibrated data item
			CalibratedADCDataItem calibratedItem(eventList.front());
			CalibrateData(eventList.front(),calibratedItem);
			eventList.pop_front();
			myClustering.AddEventToMap(calibratedItem);
		}
		//Once all items from an event has been read in and stored in maps. Begin clustering
		myClustering.ProcessMaps();
	}

	return;
}
unsigned int Calibrator::GetOrder(int channelID) const{
	return feeChannelOrder[channelID];
}
void Calibrator::CalibrateData(ADCDataItem & adcDataItemIn, CalibratedADCDataItem & calibratedItemOut){

	SetGeometry(adcDataItemIn, calibratedItemOut);
	CalibrateEnergy(adcDataItemIn, calibratedItemOut);

}
void Calibrator::SetGeometry(ADCDataItem & adcDataItemIn, CalibratedADCDataItem & calibratedItemOut){

	//FEE channel does not map perfectly to strip channels. Instead need to set the geometry of each detector depending on which FEE position and which channel.

	calibratedItemOut.SetDSSD(feeDSSDMap[adcDataItemIn.GetFEE64ID()-1]-1);
	if(feeStripMap[adcDataItemIn.GetFEE64ID()-1] == 1){
		calibratedItemOut.SetStrip(GetOrder(adcDataItemIn.GetChannelID()));
			}
	else if(feeStripMap[adcDataItemIn.GetFEE64ID()-1] == 2){
		calibratedItemOut.SetStrip(127-GetOrder(adcDataItemIn.GetChannelID()));		
	}
	else{
		std::cout << "Warning! FEE mapped to strip map improperly check variables file." << std::endl;
	}
	calibratedItemOut.SetSide(feeSideMap[adcDataItemIn.GetFEE64ID()-1]);
}
void Calibrator::CalibrateEnergy(ADCDataItem & adcDataItemIn, CalibratedADCDataItem & calibratedItemOut){

	//Takes the raw ADC data and applies the offsets and the polarity of the signal.

	if(adcDataItemIn.GetADCRange() == 0){
		//Low energy event
		calibratedItemOut.SetADCRange(0);
		calibratedItemOut.SetEnergy(((double)adcDataItemIn.GetADCData()-adcZero - (double)channelADCOffsets[adcDataItemIn.GetFEE64ID()-1][adcDataItemIn.GetChannelID()])
				*(double)feePolarityMap[adcDataItemIn.GetFEE64ID()-1]*adcLowEnergyGain[adcDataItemIn.GetFEE64ID()-1][adcDataItemIn.GetChannelID()]);
	}
	else if(adcDataItemIn.GetADCRange() == 1){
		//High energy event
		calibratedItemOut.SetADCRange(1);
		calibratedItemOut.SetEnergy((	(double)adcDataItemIn.GetADCData()-adcZero)*(double)feePolarityMap[adcDataItemIn.GetFEE64ID()-1]
										*adcHighEnergyGain[adcDataItemIn.GetFEE64ID()-1][adcDataItemIn.GetChannelID()]);
	}
}