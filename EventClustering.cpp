#include "EventClustering.hpp"

EventClustering::EventClustering(){

	#ifdef HISTOGRAMMING
		for (int i = 0; i < Common::noDSSD;i++){
			char hname[50];
			sprintf(hname,"lowEnergyExEyDSSD%d",i);
			lowEnergyExEy[i] = new TH2D(hname,"",5e2,0,1e4,5e2,0,1e4);

			hname[0] = '\0';
			sprintf(hname,"highEnergyExEyDSSD%d",i);
			highEnergyExEy[i] = new TH2D(hname,"",5e2,0,1e4,5e2,0,1e4);
		}
	#endif
};

void EventClustering::InitialiseClustering(){
	decayMap.clear();
	implantMap.clear();

	for (int i = 0; i < Common::noDSSD; i++){
		for (int j = 0; j <2; j++){
			dssdDecayLists[i][j].clear();
			dssdImplantLists[i][j].clear();
		}
	}

	implantStoppingLayer = -5;
}
void EventClustering::AddEventToMap(CalibratedADCDataItem &dataItem){

	if(dataItem.GetADCRange() == 0){
		//Store decay items in a map that organises them by DSSD->Side->Strip
		decayMap.emplace(dataItem,1);
	}
	else if(dataItem.GetADCRange() == 1){
		//Store implant items in a map that organises them by DSSD->Side->Strip
		implantMap.emplace(dataItem,1);
	}
}
void EventClustering::ProcessMaps(){

	if(implantMap.size() > 0){
		//If there are events in the implant map, cluster them.
		//Do not expect many clusters here but just in case
		ClusterMap(implantMap);
		implantStoppingLayer = ImplantStoppingLayer();
		PairClusters(implantStoppingLayer, implantEnergyDifference, dssdImplantLists);

		#ifdef DEB_IMPLANT_STOPPING
			if(implantStoppingLayer > -1){
				positiveStopping ++;
			}
			else if(implantStoppingLayer == -1){
				negativeStopping++;
			}	

			std::cout << "Ne event" << std::endl;
			int Etest = 0;
			int Etest2 = 0;	

			for(int i = 0; i<Common::noDSSD ;i++){	

				Etest = dssdImplantLists[i][0].front().GetEnergy();
				Etest2 = dssdImplantLists[i][1].front().GetEnergy();
				std::cout << "Detector " << i << " Side0 Mult: " << dssdImplantLists[i][0].size() << " " << Etest
						  << " Side1 Mult: " << dssdImplantLists[i][1].size() << " " << Etest2 << std::endl;
			}

			std::cout << "Implant stopped in layer: " << implantStoppingLayer << " Percentage of implant events with positive stopping layer: " <<
						(double)positiveStopping / ((double)negativeStopping + (double)positiveStopping) <<std::endl;
		#endif

	}
	if(decayMap.size() > 0){
		//If there are events in the decay map begin clustering.
		ClusterMap(decayMap);
		for(int dssd = 0; dssd < Common::noDSSD; dssd++){
			PairClusters(dssd, decayEnergyDifference,dssdDecayLists);
		}
	}
}
void EventClustering::ClusterMap(std::multimap<CalibratedADCDataItem,int> & eventMap){

	#ifdef CLUSTER_DECAY_DEB
		std::cout << "\n \n \nEvent map size " << eventMap.size() << std::endl;
		int clusterItem = 0;
	#endif

	Cluster eventCluster; //Defines the cluster class used

	for(clusterIt = eventMap.begin(); clusterIt != eventMap.end(); clusterIt++){
		//Iterator that loops through the map of decays from the beginning to the end

		if(abs(clusterIt->first.GetStrip()-eventCluster.GetStrip()) == 1){
			//If current event is adjacent to last strip added to the cluster or cluster currently has no events proceed

			if(eventCluster.GetTimestampDifference(clusterIt->first.GetTimestamp())<=200){
				//If the current event is within 2us of events within the cluster proceed

				if(eventCluster.GetDSSD() == clusterIt->first.GetDSSD() && eventCluster.GetSide() == clusterIt->first.GetSide()){
					//If decay event is on the same DSSD and Side as the cluster add event to cluster 

					eventCluster.AddEventToCluster(clusterIt->first);

					#ifdef CLUSTER_DECAY_DEB
						clusterItem = clusterItem + 1;
						std::cout << "Cluster item: " << clusterItem << " DSSD: " << clusterIt->first.GetDSSD() << " Side: " << clusterIt->first.GetSide() <<
									 " Strip " << clusterIt->first.GetStrip() << " Timestamp: " << clusterIt->first.GetTimestamp() <<std::endl;
					#endif

				}
				else{
					//Cluster is finished. Add cluster to cluster maps and create new cluster

					#ifdef CLUSTER_DECAY_DEB
						std::cout << "\nSide or DSSD not the sameItem not added to cluster creating new cluster" <<std::endl;
					#endif

					//Closes the current cluster. Storing and then resets it
					CloseCluster(eventCluster);
					eventCluster.AddEventToCluster(clusterIt->first);

				#ifdef CLUSTER_DECAY_DEB
					clusterItem = clusterItem + 1;
					std::cout << "Cluster item: " << clusterItem << " DSSD: " << clusterIt->first.GetDSSD() << " Side: " << clusterIt->first.GetSide() <<
								 " Strip " << clusterIt->first.GetStrip() << " Timestamp: " << clusterIt->first.GetTimestamp() <<std::endl;
				#endif
				}
			}
			else{

				#ifdef CLUSTER_DECAY_DEB
					std::cout << "\nTimestamp difference to great. Item not added to cluster creating new cluster" <<std::endl;
				#endif

				//Closes the current cluster. Storing and then resets it
				CloseCluster(eventCluster);
				eventCluster.AddEventToCluster(clusterIt->first);

				#ifdef CLUSTER_DECAY_DEB
					clusterItem = clusterItem + 1;
					std::cout << "Cluster item: " << clusterItem << " DSSD: " << clusterIt->first.GetDSSD() << " Side: " << clusterIt->first.GetSide() <<
								 " Strip " << clusterIt->first.GetStrip() << " Timestamp: " << clusterIt->first.GetTimestamp() <<std::endl;
				#endif
			}
		}
		else if ( eventCluster.GetStrip() == -5){
			//If cluster currently doesn't have items associated with it add to cluster

			eventCluster.AddEventToCluster(clusterIt->first);
				#ifdef CLUSTER_DECAY_DEB
					clusterItem = clusterItem + 1;
					std::cout << "New cluster." <<std::endl;
					std::cout << "Cluster item: " << clusterItem << " DSSD: " << clusterIt->first.GetDSSD() << " Side: " << clusterIt->first.GetSide() <<
								 " Strip " << clusterIt->first.GetStrip() << " Timestamp: " << clusterIt->first.GetTimestamp() <<std::endl;
				#endif

		}
		else if (abs(clusterIt->first.GetStrip()-eventCluster.GetStrip()) > 1){
			//Cluster is finished

			#ifdef CLUSTER_DECAY_DEB
				std::cout << "\nStrip difference greater than 1. Item not added to cluster creating new cluster." <<std::endl;
			#endif
			
			//Closes the current clustedr. Storing and then resets it
			CloseCluster(eventCluster);
			eventCluster.AddEventToCluster(clusterIt->first);
			#ifdef CLUSTER_DECAY_DEB
				clusterItem = clusterItem + 1;
				std::cout << "Cluster item: " << clusterItem << " DSSD: " << clusterIt->first.GetDSSD() << " Side: " << clusterIt->first.GetSide() <<
							 " Strip " << clusterIt->first.GetStrip() << " Timestamp: " << clusterIt->first.GetTimestamp() <<std::endl;
			#endif
		}
	}
	if(eventCluster.GetDSSD() != -5){
		//Cluster isnt empty
		CloseCluster(eventCluster);
	}
}
void EventClustering::CloseCluster(Cluster & eventCluster){

	switch(eventCluster.GetADCRange()){
		case 0:
			//decay event cluster, store with decay clusters
			dssdDecayLists[eventCluster.GetDSSD()][eventCluster.GetSide()].push_back(eventCluster);
			eventCluster.ResetCluster();

			#ifdef CLUSTER_DECAY_DEB
				std::cout << "Decay cluster written to list and then reset ready to be written to again\n" << std::endl;
			#endif

			break;

		case 1:
			//implant event cluster, store with implant clusters
			dssdImplantLists[eventCluster.GetDSSD()][eventCluster.GetSide()].push_back(eventCluster);
			eventCluster.ResetCluster();

			#ifdef CLUSTER_DECAY_DEB
				std::cout << "Implant cluster written to list and then reset ready to be written to again\n" << std::endl;
			#endif
			break;

		default:
			//Unrecognised ADC range
			std::cout << "Unrecognised ADC range error" << std::endl;
			break;
	}

}
short EventClustering::ImplantStoppingLayer(){
	//Currently only care for the final layer in which the impplant stops in.
	//To determine this layer the following conditions must be met
	//1. Stopping layer has clusters in both sides of the detector (Require a pair for position)
	//2. No clusters downstream of the stopping layer
	//3. A cluster in at least one side of all upstream detectors
	//Loop through the implant lists to find the layer where these conditions are satisfied

	for (int stoppingLayer = 0; stoppingLayer < Common::noDSSD; stoppingLayer++){
		//Loops through detectors.
		bool downstreamEvents = false;
		bool upstreamEvents = true;

		if(dssdImplantLists[stoppingLayer][0].size() > 0 && dssdImplantLists[stoppingLayer][1].size() > 0){
			//If an implant cluster in both sides
			if(stoppingLayer < 5){
				//Current layer is not the last layer
				for(int downstreamDet = stoppingLayer + 1; downstreamDet < Common::noDSSD; downstreamDet++){
					if(dssdImplantLists[downstreamDet][0].size() > 0 || dssdImplantLists[downstreamDet][1].size() > 0){
						//Downstream dssd contain implant events. Not stopping layer
						downstreamEvents = true;
						break;
					}
				}
			}
			if(stoppingLayer > 0 && !downstreamEvents){
				//If not the first detector, check for upstream events
				for(int upstreamDet = 0; upstreamDet < stoppingLayer; upstreamDet++){
					if(dssdImplantLists[upstreamDet][0].size() == 0 && dssdImplantLists[upstreamDet][1].size() == 0){
						downstreamEvents = false;
						break;
					}
				}
			}
			if(!downstreamEvents && upstreamEvents){
				//This is a stopping layer
				return stoppingLayer;
			}
		}
	}
	//Has looped through all clusters in the event and doesnt have a positive implant
	return -1;
}
void EventClustering::PairClusters(int dssd, double equalEnergyRange,std::list<Cluster> clusterLists[][2]){
	//Loop through the clusters in the event map and pair front and back clusters based on equal energy and time difference

	if(clusterLists[dssd][0].size() > 0 && clusterLists[dssd][1].size() > 0){
		//If clusters in both sides of the detector loop through and pair them

		for(clusterSide0It = clusterLists[dssd][0].begin(); clusterSide0It != clusterLists[dssd][0].end();clusterSide0It++){
			//Loops through side 0 cluster list
			#ifdef DEB_CLUSTER_PAIR
				std::cout << "New cluster" << std::endl;
				bool clusterPairT = false;
				bool clusterPairE = false;
				clusterTotal++;
			#endif
			for(clusterSide1It = clusterLists[dssd][1].begin();clusterSide1It != clusterLists[dssd][1].end();clusterSide1It++){
				//Loops through side 1 cluster list
				#ifdef HISTOGRAMMING
					if(clusterSide0It->GetADCRange() == 0){
						lowEnergyExEy[dssd]->Fill(clusterSide0It->GetEnergy(),clusterSide1It->GetEnergy());
					}
					else if(clusterSide0It->GetADCRange() == 1){
						highEnergyExEy[dssd]->Fill(clusterSide0It->GetEnergy(),clusterSide1It->GetEnergy());
					}
				#endif
				if(abs(clusterSide0It->GetEnergy()-clusterSide1It->GetEnergy()) <= equalEnergyRange){
					//Is the difference between the two clusters less than the equal energy cuts

					if((clusterSide0It->GetTimestampDifference(clusterSide1It->GetTimestampMin())<250) ||
						 (clusterSide0It->GetTimestampDifference(clusterSide1It->GetTimestampMax())<250) ){
					#ifdef DEB_CLUSTER_PAIR
						if(!clusterPairT){
							pairedTime++;
							clusterPairT = true;
							if(clusterPairE){
								pairedEnergy--;
							}
							clusterPairE=true;
							std::cout << "Pecentage of clusters being paired " << (double)pairedTime/(double)clusterTotal << std::endl;
						}
						//std::cout << "Cluster paired" << " time difference " << abs(clusterSide0It->GetTimestampMin()-clusterSide1It->GetTimestampMin()) <<  std::endl;
					#endif
					}
					#ifdef DEB_CLUSTER_PAIR
					else{
						if(!clusterPairE){
							pairedEnergy++;
							clusterPairE = true;
						}
						//std::cout << "Cluster not paired" << " time difference " << abs(clusterSide0It->GetTimestampMin()-clusterSide1It->GetTimestampMin()) << std::endl;
						std::cout << "Percentage of paired event making E " << (double)pairedEnergy/clusterTotal<< std::endl;
					}
					#endif
				}
				#ifdef DEB_CLUSTER_PAIR
				else{
					std::cout << "Cluster not paired" << " energy difference " << abs(clusterSide0It->GetEnergy()-clusterSide1It->GetEnergy()) << std::endl;
				}
				#endif

			}//Side 1 cluster list close
		}//Side 0 cluster list close
	}//If cluster list size >0 close
}
void EventClustering::CloseClustering(){
	std::cout << "Clustering finished" <<std::endl;

	#ifdef HISTOGRAMMING
		for(int i =0; i<Common::noDSSD;i++){
			lowEnergyExEy[i]->Write();
		}
		for(int i =0; i<Common::noDSSD;i++){
			highEnergyExEy[i]->Write();
		}
	#endif
}
