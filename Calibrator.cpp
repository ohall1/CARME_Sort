#include "Calibrator.hpp"
Calibrator::Calibrator(){};
void Calibrator::InitialiseCalibrator(std::string variablesFile, EventBuilder *eventBuilderPointIn){

	ReadInVariables(variablesFile);
	myEventBuilder = eventBuilderPointIn;

	for(int i =0; i < 24 ; i++){
		for(int j =0; j<64; j++){
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

			CalibratedADCDataItem calibratedItem(eventList.front());
			CalibrateData(eventList.front(),calibratedItem);
			eventList.pop_front();
			std::cout << calibratedItem.GetEnergy()<<std::endl;
			myClustering.AddEventToMap(calibratedItem);
		}
		myClustering.ProcessMaps();
	}

	return;
}
unsigned int Calibrator::GetOrder(int channelID) const{
	return feeChannelOrder[channelID];
}
void Calibrator::CalibrateData(ADCDataItem & adcDataItemIn, CalibratedADCDataItem & calibratedItemOut){

	SetGeometry(adcDataItemIn, calibratedItemOut);

}
void Calibrator::SetGeometry(ADCDataItem & adcDataItemIn, CalibratedADCDataItem & calibratedItemOut){

	calibratedItemOut.SetDSSD(feeDSSDMap[adcDataItemIn.GetFEE64ID()-1]);
	if(feeStripMap[adcDataItemIn.GetFEE64ID()-1] == 1){
		calibratedItemOut.SetStrip(GetOrder(adcDataItemIn.GetChannelID()));
			}
	else if(feeStripMap[adcDataItemIn.GetFEE64ID()-1] == 2){
		calibratedItemOut.SetStrip(127-GetOrder(adcDataItemIn.GetChannelID()));		
	}
	else{
		std::cout << "Warning! FEE mapped to strip map improperly check variables file." << std::endl;
	}
	calibratedItemOut.SetSide(feeStripMap[adcDataItemIn.GetFEE64ID()-1]);
}
void Calibrator::CalibrateEnergy(ADCDataItem & adcDataItemIn, CalibratedADCDataItem & calibratedItemOut){

	if(adcDataItemIn.GetADCRange() == 0){
		//Low energy event
		calibratedItemOut.SetADCRange(0);
		calibratedItemOut.SetEnergy((adcDataItemIn.GetADCData()-adcZero - channelADCOffsets[adcDataItemIn.GetFEE64ID()-1][adcDataItemIn.GetChannelID()])
				*feePolarityMap[adcDataItemIn.GetFEE64ID()-1]*adcLowEnergyGain[adcDataItemIn.GetFEE64ID()-1][adcDataItemIn.GetChannelID()]);
	}
	else if(adcDataItemIn.GetADCRange() == 1){
		//High energy event
		calibratedItemOut.SetADCRange(1);
		calibratedItemOut.SetEnergy((	adcDataItemIn.GetADCData()-adcZero)*feePolarityMap[adcDataItemIn.GetFEE64ID()-1]
										*adcHighEnergyGain[adcDataItemIn.GetFEE64ID()-1][adcDataItemIn.GetChannelID()]);
	}
}