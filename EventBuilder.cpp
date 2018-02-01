#include "EventBuilder.hpp"

EventBuilder::EventBuilder(){
	//Ensure event is initialised when class is first created
	InitialiseEvent();
	correlationScalerOffset = 0;
	
	totalPulserWords = 0;
	totalDecayWords = 0;
	totalImplantWords = 0;
	pulserEvents = 0;

	#ifdef HISTOGRAMMING
		pulserVsChannel = new TH2D("pulserVsChannel","",1536,0,1536,1e3,0,65536);
		//((FEE*64)+ChannelID  (256*6)), ADCData (2**16))

		lowEnergyMultiplicity = new TH1I("lowEnergyMultiplicity","",1536,0,1536);
		highEnergyMultiplicity = new TH1I("highEnergyMultiplicity","",1536,0,1536);
	#endif
	#ifdef OFFSETS
		absPulserVsChannel = new TH2D("absPulserVsChannel","",Common::noFEE64*64,0,Common::noFEE64,1e3,0,32768);
	#endif
	return;
};

void EventBuilder::InitialiseEvent(){

	//Reset the ADC item counter to 0
	for(int i = 0; i < 24; i++){
		for(int j = 0; j < 4; j++){
			adcItemCounts[i][j] = 0;
			adcLastTimestamp[i][j] = 0;
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
	itemADC = ((adcItem.GetChannelID()) / 16);
	itemFEE = adcItem.GetFEE64ID()-1;
	itemTimestamp = adcItem.GetTimestamp();
	if((itemTimestamp-adcLastTimestamp[itemFEE][itemADC] > 250 ) && adcLastTimestamp[itemFEE][itemADC]!=0){
		//If two seperate ADC events within same window reset the adc counter
		adcItemCounts[itemFEE][itemADC]=0;
	}
	adcLastTimestamp[itemFEE][itemADC]=itemTimestamp;
	//normalItems++;
	//timestampCorrection = (200 * adcItemCounts[itemFEE][itemADC]);

	adcItem.SetTimestamp((itemTimestamp-(200 * adcItemCounts[itemFEE][itemADC])));
 	adcItemCounts[itemFEE][itemADC]++;
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

	if(implantEvents.size() > 1){//Need at least two events to make a front back pair
		#ifdef DEB_EVENTBUILDER
			std::cout << "End of event window." <<std::endl;
			std::cout << "Size of implant events > 0. Event defined as implant event. Implant size = " << implantEvents.size() <<std::endl;
			std::cout << "Implant events being passed onto calibrator.\n" <<std::endl;
		#endif
		#ifdef HISTOGRAMMING
			highEnergyMultiplicity->Fill(implantEvents.size());
		#endif
		AddEventToBuffer(implantEvents);
		totalImplantEvents++;
	}
	else if(decayEvents.size() > 800){
		#ifdef DEB_EVENTBUILDER
			std::cout << "End of event window." <<std::endl;
			std::cout << "Size of decay list > 800. Event being defined as a pulser event. Multiplicity = " << decayEvents.size() << "\n" << std::endl;
		#endif
		pulserEvents++;
		totalPulserWords += decayEvents.size();

		#ifdef HISTOGRAMMING
			for(decayEventsIt = decayEvents.begin(); decayEventsIt != decayEvents.end(); decayEventsIt++){
				//Allows all channels to be plotted on one histogram
				pulserVsChannel->Fill(((decayEventsIt->GetFEE64ID()-1)*64)+decayEventsIt->GetChannelID(),decayEventsIt->GetADCData());
			}
			lowEnergyMultiplicity->Fill(decayEvents.size());
		#endif
		#ifdef OFFSETS
			for(decayEventsIt = decayEvents.begin(); decayEventsIt != decayEvents.end(); decayEventsIt++){
				//Allows all channels to be plotted on one histogram
				absPulserVsChannel->Fill(((decayEventsIt->GetFEE64ID()-1)*64)+decayEventsIt->GetChannelID(),abs(decayEventsIt->GetADCData()-32768));
			}
		#endif
	}
	else if(decayEvents.size() > 1){//Need at least two items to make a front back pair
		#ifdef DEB_EVENTBUILDER
			std::cout << "End of event window." <<std::endl;
			std::cout << "Event defined as a decay event. Decays passed onto calibrator. Decay size = " <<decayEvents.size() << "\n" << std::endl;
		#endif
		#ifdef HISTOGRAMMING
			lowEnergyMultiplicity->Fill(decayEvents.size());
		#endif
		AddEventToBuffer(decayEvents);
		totalDecayEvents++;
	}

	totalDecayWords +=decayEvents.size();
	totalImplantWords+=implantEvents.size();

	//Once lists have been copied and are being processed initialise a new event

	return;
}

void EventBuilder::AddADCEvent(ADCDataItem & adcItem){

	//Check if event is still within event window
	if((adcItem.GetTimestamp()-previousTimestamp)>=220 && previousTimestamp != 0){
		//Gap between items greater than multiplexed time period. New item is the start of a new event close the old event
		CloseEvent();
		InitialiseEvent();
	}
	previousTimestamp = adcItem.GetTimestamp();

	CorrectMultiplexer(adcItem);

	ApplyCorrelationScalerOffset(adcItem);


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
void EventBuilder::AddEventToBuffer(std::list<ADCDataItem> closedEvent){
	//Aqquire bufProtect mutex lock to modify list
	std::unique_lock<std::mutex> addLock(bufProtect);
	#ifdef DEB_EVENTBUILDER_THREAD
		std::cout << "Mutex lock acquire for AddToBuffer" << std::endl;
	#endif

	//Wait if list size exceeds maximum buffer
	while(eventsList.size()>=100){
		bufferFullCheck = true;
		#ifdef DEB_EVENTBUILDER_THREAD
			std::cout << "Buffer list is fulle. BufferFulCheck = " << bufferFullCheck <<std::endl;
		#endif
		bufferFull.wait(addLock);
	}

	eventsList.push_back(closedEvent);
	#ifdef DEB_EVENTBUILDER_THREAD
		std::cout << "Data pair added to back of buffer." << std::endl;
	#endif


	#ifdef DEB_EVENTBUILDER_THREAD
		std::cout << "Checking bufferEmptyCheck = " << bufferEmptyCheck << std::endl;
	#endif

	if(bufferEmptyCheck){
		bufferEmptyCheck = false;
		#ifdef DEB_EVENTBUILDER_THREAD
			std::cout << "Buffer no longer empty. Notifying ReadFromBufferThread" << std::endl;
		#endif
		bufferEmpty.notify_all();
	}

	//Unlock the bufProtect mutex lock
	addLock.unlock();
	#ifdef DEB_EVENTBUILDER_THREAD
		std::cout << "Mutext lock on AddToBuffer released" <<std::endl;
		std::cout << " " << std::endl;
	#endif

	return;
}
std::list<ADCDataItem> EventBuilder::GetEventFromBuffer(){
	std::list<ADCDataItem> bufferOut;

	//Aqquire bufProtect mutex lock to modify list
	std::unique_lock<std::mutex> popLock(bufProtect);
	#ifdef DEB_EVENTBUILDER_THREAD
		std::cout << "Mutex lock acquired for ReadFromBuffer" <<std::endl;
	#endif

	//Check if the list size is empty
	while(eventsList.size()<1){
		bufferEmptyCheck = true;
		#ifdef DEB_EVENTBUILDER_THREAD
			std::cout << "Buffer list is empty, thread waiting. bufferEmptyCheck = " << bufferEmptyCheck << std::endl;
		#endif

		bufferEmpty.wait(popLock);
	}

	bufferOut = eventsList.front();
	eventsList.pop_front();

	#ifdef DEB_EVENTBUILDER_THREAD
		std::cout << "Current list buffer size " << eventsList.size() << " items." << std::endl;
	#endif


	#ifdef DEB_EVENTBUILDER_THREAD
		std::cout << "Checking bufferFullCheck = " << bufferFullCheck << std::endl;
	#endif

	if(bufferFullCheck){
		bufferFullCheck=false;
		#ifdef DEB_EVENTBUILDER_THREAD
			std::cout << "Buffer no longer full. Notifying AddToBufferThread" << std::endl;
		#endif
		bufferFull.notify_all();
	}

	popLock.unlock();

	#ifdef DEB_EVENTBUILDER_THREAD
		std::cout << "Releasing mutex lock on ReadFromBuffer" << std::endl;
		std::cout << " " <<std::endl;
	#endif

	return bufferOut;
}
unsigned long EventBuilder::GetDecayWords(){
	return totalDecayWords;
}
unsigned long EventBuilder::GetImplantWords(){
	return totalImplantWords;
}
unsigned int EventBuilder::GetPulserEvents(){
	return pulserEvents;
}
unsigned long EventBuilder::GetPulserWords(){
	return totalPulserWords;
}
unsigned int EventBuilder::GetDecayEvents(){
	return totalDecayEvents;
}
unsigned int EventBuilder::GetImplantEvents(){
	return totalImplantEvents;
}
void EventBuilder::UnpackerFinished(){

	decayEvents.clear();
	decayItem.SetADCRange(2);
	decayEvents.push_back(decayItem);
	AddEventToBuffer(decayEvents);

	#ifdef HISTOGRAMMING
		pulserVsChannel->Write();
		lowEnergyMultiplicity->Write();
		highEnergyMultiplicity->Write();
	#endif
	#ifdef OFFSETS
		absPulserVsChannel->Write();
	#endif
	return;
}