#ifndef _EVENTCLUSTERING_HPP
#define _EVENTCLUSTERING_HPP

#include <map>

class EventClustering{
	private:
		std::multimap<CalibratedADCDataItem,int> decayMap;			//Map to store the calibrated decay events
		std::multimap<CalibratedADCDataItem,int> implantMap;		//Map to store the calibrated implant events

		std::multimap<CalibratedADCDataItem,int>::iterator clusterIt; 	//Iterator to loop through the maps to be clustered

	public:
		EventClustering(){};
		~EventClustering(){};
		void ProcessDecays();										//Process the decay map
		void ProcessImplants();										//Process the implant map

		void AddEventToMap(CalibratedADCDataItem & dataItem);		//Add event to the applicable map to the event type
		void PrcessMap();											//Reached the end of the event process the maps

};

#endif