#include "EventBuilder.hpp"
#include "DataUnpacker.hpp"

EventBuilder::EventBuilder(){
	//Ensure event is initialised when class is first created
	InitialiseEvent();
	correlationScalerOffset = 0;
	return;
};

void EventBuilder::InitialiseEvent(){

	//Reset the ADC item counter to 0
	for(int i = 0; i < 24; i++){
		for(int j = 0, j < 4; j++){
			adcItemCounts[i][j] = 0;
		}
	}

	//Clear the event lists
	decayEvents.clear();
	implantEvents.clear();

	//Reset previousTimestamp to zero
	previousTimestamp = 0;
	return;
}
void EventBuilder::CorrectMultiplexer(ADCDataItem & adcItem){

	//Check the FEE and channel of the DSSD and determine how many events from that ADC have been recoreded.
	//Correct the timestamp for the multiplexing
	itemADC = ((adcItem.GetChannelID() - 1) / 16);
	timestampCorrection = (200 * adcItemCounts[adcItem.GetFEE64ID()-1][itemADC]);
	adcItem.SetTimestamp((adcItem.GetTimestamp()-timestampCorrection));
 	adcItemCounts[adcItem.GetFEE64ID()-1][itemADC] += 1;
 	return;

}
void EventBuilder::ApplyCorrelationScalerOffset(ADCDataItem & adcItem){

	//Applys the correlation scaler offset to the timestamp
	//This allows the timestamp to be synced with BRIKEN
	adcItem.SetTimestamp(adcItem.GetTimestamp()+correlationScalerOffset);

}
void EventBuilder::CloseEvent(){
	//End of event has been reached. Copy the lists to be processed
	//As decay lists are being processed the multiplicity is calculated and used to deterine if a pulser event.
	//First check: If multiplicity of decay map is greater than 800 define as a pulser event
	//Second check: If size of implant map > 0 - Define as implant event. Will currently veto all decays
	//Thir check: If implant map == 0 and decay map > 0 define as decay event

	#ifdef DEB_EVENTBUILDER
		std::cout << "Difference between adjacent tiestamps greater than event window closing event" << std::endl;
	#endif
	if(decayEvents.size() > 800){
		#ifdef DEB_EVENTBUILDER
			std::cout << "Size of decay list > 800. Event being defined as a pulser event." <<std::endl;
		#endif
	}
	else if(implantEvents.size() > 0){
		#ifdef DEB_EVENTBUILDER
			std::cout << "Size of implant events > 0. Event defined as implant event" <<std::endl;
			std::cout << "Implant events being passed onto calibrator." <<std::endl;
		#endif
	}
	else if(decayEvents.size() > 0){
		#ifdef DEB_EVENTBUILDER
			std::cout << "Event defined as a decay event. Decays passed onto calibrator." << std::endl;
		#endif
	}


	//Once lists have been copied and are being processed initialise a new event

	return;
}

void EventBuilder::AddADCEvent(ADCDataItem & adcItem){

	//Check if event is still within event window
	if((adcItem.GetTimestamp()-previousTimestamp)>=250 && previousTimestamp != 0){
		//Gap between items greater than multiplexed time period. New item is the start of a new event close the old event
		CloseEvent();
		InitialiseEvent();
	}

	CorrectMultiplexer(adcItem);

	//ApplyCorrelationScalerOffset(adcDataItem);


 	if( adcItem.GetADCRange() == 0){
 		//Low energy event. Add to decay list
 		decayEvents.push_back(adcItem);
 	}
 	else if( adcItem.GetADCRange() == 1){
 		//High energy event. Add to implant list
 		implantEvents.push_back(adcItem);
 	}

 	return;

}
void EventBuilder::SetCorrelationScaler(unsigned long corrOffset){
	correlationScalerOffset = corrOffset;
	return;
}
unsigned long EventBuilder::GetCorrelationScalerOffset(){
	return correlationScalerOffset;
}