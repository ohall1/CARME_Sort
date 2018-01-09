#ifndef _EVENTCLUSTERING_HPP
#define _EVENTCLUSTERING_HPP

#include <map>

class EventClustering{
	private:
		std::multimap<CalibratedADCDataItem,int> decayMap;			//Map to store the calibrated decay events
		std::multimap<CalibratedADCDataItem,int> implantMap;		//Map to store the calibrated implant events

		std::multimap<CalibratedADCDataItem,int>::iterator clusterIt; 	//Iterator to loop through the maps to be clustered

		std::list<Cluster> dssdLists[6][2];								//Array of lists to store clusters in
		std::list<Cluster>::iterator clusterSide0It;					//Iterator for side 0
		std::list<Cluster>::iterator clusterSide1It;					//Iterator for clusters on side 1

		void ProcessDecays();										//Process the decay map
		void ProcessImplants();										//Process the implant map
		void CloseCluster(Cluster & decayCluster);					//Close, store and reset cluster when no more events to add


	public:
		EventClustering();
		~EventClustering(){};

		void InitialiseClustering();
		void AddEventToMap(CalibratedADCDataItem & dataItem);		//Add event to the applicable map to the event type
		void ProcessMaps();											//Reached the end of the event process the maps

};

#endif