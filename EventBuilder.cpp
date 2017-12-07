#include "EventBuilder.hpp"
#include "DataUnpacker.hpp"

EventBuilder::EventBuilder(){
	//Ensure event is initialised when class is first created
	InitialiseEvent();
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
}

void EventBuilder::AddADCEvent(ADCDataItem & adcItem){

	//Check the FEE and channel of the DSSD and determine how many events from that ADC have been recoreded.
	//Correct the timestamp for the multiplexing
}