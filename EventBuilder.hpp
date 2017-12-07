#ifndef _EVENTBUILDER_HPP
#define _EVENTBUILDER_HPP

#include <list>

#include "EventBuilder.cpp"
#include "DataUnpacker.hpp"

class EventBuilder{

private:

	//Lists to store data events
	std::list <ADCDataItem> decayEvents;	//List to store decay events
	std::list <ADCDataItem> implantEvents;	//List to store implant events

	unsigned short adcItemCounts[24][4]		//2D array: 1st dimension is FEE, 2nd dimension is ADC

	void AddDecayItem(ADCDataItem & decayItem);		//Add decay item to decay list
	void AddImplantItem(ADCDataItem & implantItem);	//Add implant item to implant list
	void InitialiseEvent();							//Initialise event, clears lists and resets counters

public:
	EventBuilder();
	~EventBuilder(){};

	void AddADCEvent(ADCDataItem & adcItem);		//Determine whether decay/implant and add correct timestamp

}
#endif