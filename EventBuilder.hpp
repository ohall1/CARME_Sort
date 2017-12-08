#ifndef _EVENTBUILDER_HPP
#define _EVENTBUILDER_HPP
#include "DataItems.hpp"

#include <list>

class EventBuilder{

private:

	ADCDataItem decayItem;
	ADCDataItem implantItem;

	//Lists to store data events
	std::list <ADCDataItem> decayEvents;		//list to store decay events
	std::list <ADCDataItem> implantEvents;	//list to store implant events

	unsigned long normalItems = 0;
	unsigned long lateItems = 0;

	unsigned short adcItemCounts[24][4];	//2D array: 1st dimension is FEE, 2nd dimension is ADC
	unsigned long adcLastTimestamp[24][4]; //2D array: 1st dimension is FEE, 2nd dimension is ADCs
	unsigned short itemADC;					//Which ADC was the item recorded in
	unsigned short itemFEE;					//Which FEE does the item belong to
	unsigned long itemTimestamp; 			//What is the timestamp of the item
	unsigned long timestampCorrection;		//Value to subtract from the timestamp to correct for multiplexing
	unsigned long previousTimestamp;		//Keeps track of previous timestamp in event
	unsigned long correlationScalerOffset; 	//Offset applied to timestamp to sync with BRIKEN and BigRIPS timestamps

	void AddDecayItem(ADCDataItem decayItem);					//Add decay item to decay list
	void AddImplantItem(ADCDataItem implantItem);				//Add implant item to implant list
	void InitialiseEvent();										//Initialise event, clears lists and resets counters
	void CorrectMultiplexer(ADCDataItem & adcItem);				//Corrects the timestamp for the multiplexing of ADC data
	void ApplyCorrelationScalerOffset(ADCDataItem & adcItem);	//Applys the correlation scaler offset to the timestamp
	void CloseEvent();											//End of current event, send decay/implant events to be processed

public:
	EventBuilder();
	~EventBuilder(){};

	void AddADCEvent(ADCDataItem & adcItem);					//Determine whether decay/implant and add correct timestamp
	void SetCorrelationScaler(unsigned long corrOffset);		//Sets the correlation scaler offset

	unsigned long GetCorrelationScalerOffset();					//Returns the correlation scaler offset

};
#endif