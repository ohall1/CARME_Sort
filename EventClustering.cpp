#include "EventClustering.hpp"

EventClustering::EventClustering(){};

void EventClustering::InitialiseClustering(){
	decayMap.clear();
	implantMap.clear();
}
void EventClustering::AddEventToMap(CalibratedADCDataItem &dataItem){

	if(dataItem.GetADCRange() == 0){
		decayMap.emplace(dataItem,1);
	}
	else if(dataItem.GetADCRange() == 1){
		implantMap.emplace(dataItem,1);
	}
}
void EventClustering::ProcessMaps(){

	if(implantMap.size() > 0){

	}
	if(decayMap.size() > 0){

		Cluster decayCluster;
		for(clusterIt = decayMap.begin(); clusterIt != decayMap.end(); clusterIt++){
			if(abs(clusterIt->first.GetStrip()-decayCluster.GetStrip()) == 1 || decayCluster.GetStrip() == -5){
				if(decayCluster.GetTimestampDifference(clusterIt->first.GetTimestamp())<200){
					if(decayCluster.GetDSSD() == clusterIt->first.GetDSSD() && decayCluster.GetSide() == clusterIt->first.GetSide()){
						decayCluster.AddEventToCluster(clusterIt->first);
					}
					else if (decayCluster.GetDSSD() == -5){
						decayCluster.AddEventToCluster(clusterIt->first);
					}
					else{
						//Cluster is finished. Add cluster to cluster maps and create new cluster
					}
				}
			}
		}
	}
}