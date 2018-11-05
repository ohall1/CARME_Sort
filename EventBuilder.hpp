#ifndef _EVENTBUILDER_HPP
#define _EVENTBUILDER_HPP
#include "DataItems.hpp"
#include "Common.hpp"

#include "TH2D.h"
#include "TH2I.h"

#include <list>
#include <mutex>
#include <condition_variable>

class EventBuilder{

private:

	ADCDataItem decayItem;
	ADCDataItem implantItem;

	//Lists to store data events
	std::deque <ADCDataItem> decayEvents;		//list to store decay events
	std::deque <ADCDataItem> implantEvents;		//list to store implant events

	std::queue <std::deque<ADCDataItem>> eventsList;			//List to store events to be calibrated

	unsigned long normalItems = 0;
	unsigned long lateItems = 0;

	unsigned short adcItemCounts[Common::noFEE64][4];	//2D array: 1st dimension is FEE, 2nd dimension is ADC
	unsigned long adcLastTimestamp[Common::noFEE64][4]; //2D array: 1st dimension is FEE, 2nd dimension is ADCs
	unsigned short itemADC;					//Which ADC was the item recorded in
	unsigned short itemFEE;					//Which FEE does the item belong to
	unsigned long itemTimestamp; 			//What is the timestamp of the item
	unsigned long timestampCorrection;		//Value to subtract from the timestamp to correct for multiplexing
	unsigned long previousTimestamp;		//Keeps track of previous timestamp in event
	long int correlationScalerOffset; 	//Offset applied to timestamp to sync with BRIKEN and BigRIPS timestamps
	unsigned long totalDecayWords;
	unsigned long totalImplantWords;
	unsigned long totalPulserWords;
	unsigned int pulserEvents;
	unsigned int totalDecayEvents;
	unsigned int totalImplantEvents;

	#ifdef HISTOGRAMMING
		TH2D * pulserVsChannel;								//Pulser signal across the channels
		TH1I * lowEnergyMultiplicity;						//Multiplicity of low energy events
		TH1I * highEnergyMultiplicity;						//Multiplicity of high energy events

		std::deque <ADCDataItem>::iterator decayEventsIt;	//Iterator for the decay events list
	#endif
	#ifdef OFFSETS
		TH2D * absPulserVsChannel;							//Pulsers all put on a positive scale
	#endif

	//Mutex locks and variables used for protecting the buffer
	std::mutex bufProtect;

	std::condition_variable bufferFull;
	std::condition_variable bufferEmpty;

	bool bufferFullCheck;
	bool bufferEmptyCheck;

	void AddDecayItem(ADCDataItem decayItem);					//Add decay item to decay list
	void AddImplantItem(ADCDataItem implantItem);				//Add implant item to implant list
	void InitialiseEvent();										//Initialise event, clears lists and resets counters
	void CorrectMultiplexer(ADCDataItem & adcItem);				//Corrects the timestamp for the multiplexing of ADC data
	void ApplyCorrelationScalerOffset(ADCDataItem & adcItem);	//Applys the correlation scaler offset to the timestamp
	void CloseEvent();											//End of current event, send decay/implant events to be processed
	void AddEventToBuffer(std::deque <ADCDataItem> closedEvent);

public:
	EventBuilder();
	~EventBuilder(){};

	void AddADCEvent(ADCDataItem & adcItem);					//Determine whether decay/implant and add correct timestamp
	void SetCorrelationScaler(long int corrOffset);		//Sets the correlation scaler offset
	void UnpackerFinished();

	unsigned long GetDecayWords();								//Gets the total amount of decay words
	unsigned long GetImplantWords();
	unsigned long GetPulserWords();
	unsigned int GetPulserEvents();
	unsigned int GetDecayEvents();
	unsigned int GetImplantEvents();

	long int GetCorrelationScalerOffset();					//Returns the correlation scaler offset

	std::deque<ADCDataItem> GetEventFromBuffer();							//Gets the event at the front of the buffer

};
#endif