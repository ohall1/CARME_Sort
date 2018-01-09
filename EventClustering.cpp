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

		#ifdef CLUSTER_DECAY_DEB
			std::cout << "\n \n \nDecay map size " << decayMap.size() << std::endl;
			int clusterItem = 0;
		#endif

		Cluster decayCluster;
		for(clusterIt = decayMap.begin(); clusterIt != decayMap.end(); clusterIt++){
			#ifdef CLUSTER_DECAY_DEB
				clusterItem = clusterItem + 1;
				std::cout << "Cluster item: " << clusterItem << " DSSD: " << clusterIt->first.GetDSSD() << "Side: " << clusterIt->first.GetSide() <<
							  " Strip " << clusterIt->first.GetStrip() << " Timestamp: " << clusterIt->first.GetTimestamp() <<std::endl;
			#endif
			if(abs(clusterIt->first.GetStrip()-decayCluster.GetStrip()) == 1 || decayCluster.GetStrip() == -5){
				if(decayCluster.GetTimestampDifference(clusterIt->first.GetTimestamp())<=200){
					if(decayCluster.GetDSSD() == clusterIt->first.GetDSSD() && decayCluster.GetSide() == clusterIt->first.GetSide()){
						decayCluster.AddEventToCluster(clusterIt->first);
						#ifdef CLUSTER_DECAY_DEB
							std::cout << "Item added to decay cluster" << std::endl;
						#endif
					}
					else if (decayCluster.GetDSSD() == -5){
						decayCluster.AddEventToCluster(clusterIt->first);
					}
					else{
						//Cluster is finished. Add cluster to cluster maps and create new cluster
						#ifdef CLUSTER_DECAY_DEB
							std::cout << "Side or DSSD not the sameItem not added to cluster creating new cluster\n" <<std::endl;
						#endif
						decayCluster.ResetCluster();
						decayCluster.AddEventToCluster(clusterIt->first);
					}
				}
				else{
					#ifdef CLUSTER_DECAY_DEB
						std::cout << "Timestamp difference to great. Item not added to cluster creating new cluster\n" <<std::endl;
					#endif
					decayCluster.ResetCluster();
					decayCluster.AddEventToCluster(clusterIt->first);
				}
			}
			else if (abs(clusterIt->first.GetStrip()-decayCluster.GetStrip()) > 1){
				//Cluster is finished
				#ifdef CLUSTER_DECAY_DEB
					std::cout << "Strip difference greater than 1. Item not added to cluster creating new cluster.\n" <<std::endl;
				#endif
				decayCluster.ResetCluster();
				decayCluster.AddEventToCluster(clusterIt->first);
			}
		}
	}
}