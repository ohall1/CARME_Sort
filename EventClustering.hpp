#ifndef _EVENTCLUSTERING_HPP
#define _EVENTCLUSTERING_HPP

#include <map>

class EventClustering{
	private:
		std::multimap<CalibratedADCDataItem,int> decayMap;			//Map to store the calibrated decay events
		std::multimap<CalibratedADCDataItem,int> implantMap;		//Map to store the calibrated implant events

		std::multimap<CalibratedADCDataItem,int>::iterator clusterIt; 	//Iterator to loop through the maps to be clustered

		std::list<Cluster> dssdDecayLists[6][2];						//Array of lists to store decay clusters in
		std::list<Cluster> dssdImplantLists[6][2];						//Array of lists to store implant clusters in
		std::list<Cluster>::iterator clusterSide0It;					//Iterator for side 0
		std::list<Cluster>::iterator clusterSide1It;					//Iterator for clusters on side 1

		short implantStoppingLayer;										//DSSD that an implant stops in
		#ifdef DEB_IMPLANT_STOPPING
			int positiveStopping = 0;
			int negativeStopping =0;
		#endif

		void ClusterMap(std::multimap<CalibratedADCDataItem,int> & eventMap);	//Cluster the maps once ready
		void CloseCluster(Cluster & decayCluster);								//Close, store and reset cluster when no more events to add
		short ImplantStoppingLayer();											//Find the stopping layer for an implant event

	public:
		EventClustering();
		~EventClustering(){};

		void InitialiseClustering();
		void AddEventToMap(CalibratedADCDataItem & dataItem);		//Add event to the applicable map to the event type
		void ProcessMaps();											//Reached the end of the event process the maps

};

#endif