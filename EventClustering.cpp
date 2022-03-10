#include "EventClustering.hpp"

EventClustering::EventClustering(){

	#ifdef HISTOGRAMMING
		for (int i = 0; i < Common::noDSSD;i++){
			char hname[50];
			sprintf(hname,"lowEnergyExEyDSSD%d",i);
			lowEnergyExEy[i] = new TH2D(hname,"",1e3,0,2e4,1e3,0,2e4);

			hname[0] = '\0';
			sprintf(hname,"highEnergyExEyDSSD%d",i);
			highEnergyExEy[i] = new TH2D(hname,"",1e3,0,2e4,1e3,0,2e4);

			hname[0] = '\0';
			sprintf(hname,"lowEnergyExEyPairDSSD%d",i);
			lowEnergyExEyPair[i] = new TH2D(hname,"",1e3,0,2e4,1e3,0,2e4);

			hname[0] = '\0';
			sprintf(hname,"highEnergyExEyDSSDPair%d",i);
			highEnergyExEyPair[i] = new TH2D(hname,"",1e3,0,2e4,1e3,0,2e4);

			hname[0] = '\0';
			sprintf(hname,"xyMultiplicityDSSD%d",i);
			xyMultiplicity[i] = new TH2I(hname,"",128,0,128,128,0,128);

			hname[0] = '\0';
			sprintf(hname,"lowEnergyXYDSSD%d",i);
			lowEnergyXY[i] = new TH2D(hname,"",128,0,128,128,0,128);

			hname[0] = '\0';
			sprintf(hname,"highEnergyXYDSSD%d",i);
			highEnergyXY[i] = new TH2D(hname,"",128,0,128,128,0,128);

            hname[0] = '\0';
            sprintf(hname,"lowEnergyXYRateDSSD%d",i);
            lowEnergyXYRate[i] = new TH2I(hname, "", 128,0,128,128,0,128);
			lowEnergyXYRate[i]->GetXaxis()->SetTitle("Strip number (x)");
			lowEnergyXYRate[i]->GetYaxis()->SetTitle("Strip number (y)");
            hname[0] = '\0';
            sprintf(hname,"lowEnergyXYTotalDSSD%d",i);
            lowEnergyXYTotal[i] = new TH2I(hname, "", 128,0,128,128,0,128);
			lowEnergyXYTotal[i]->GetXaxis()->SetTitle("Strip number (x)");
			lowEnergyXYTotal[i]->GetYaxis()->SetTitle("Strip number (y)");
            hname[0] = '\0';
            sprintf(hname,"lowEnergyExXRateDSSD%d",i);
            lowEnergyExXRate[i] = new TH2D(hname, "", 128,0,128,1000,0,20e3);
            hname[0] = '\0';
            sprintf(hname,"lowEnergyEyYRateDSSD%d",i);
            lowEnergyEyYRate[i] = new TH2D(hname, "", 128,0,128,1000,0,20e3);
            hname[0] = '\0';
            sprintf(hname,"lowEnergyExXTotalDSSD%d",i);
            lowEnergyExXTotal[i] = new TH2D(hname, "", 128,0,128,1000,0,20e3);
            hname[0] = '\0';
            sprintf(hname,"lowEnergyEyYTotalDSSD%d",i);
            lowEnergyEyYTotal[i] = new TH2D(hname, "", 128,0,128,1000,0,20e3);
			hname[0] = '\0';
            sprintf(hname,"lowEnergyEyTotalDSSD%d",i);
			lowEnergyEyTotal[i] = new TH1D(hname, "", 250,0,20e3);
			lowEnergyEyTotal[i]->GetXaxis()->SetTitle("Energy [MeV]");
			lowEnergyEyTotal[i]->GetYaxis()->SetTitle("Counts");
			hname[0] = '\0';
            sprintf(hname,"lowEnergyExTotalDSSD%d",i);
			lowEnergyExTotal[i] = new TH1D(hname, "", 250,0,20e3);
			lowEnergyExTotal[i]->GetXaxis()->SetTitle("Energy [MeV]");
			lowEnergyExTotal[i]->GetYaxis()->SetTitle("Counts");

			sprintf(hname,"lowEnergyEyRateDSSD%d",i);
			lowEnergyEyRate[i] = new TH1D(hname, "", 250,0,20e3);
			lowEnergyEyRate[i]->GetXaxis()->SetTitle("Energy [MeV]");
			lowEnergyEyRate[i]->GetYaxis()->SetTitle("Counts");

			            sprintf(hname,"lowEnergyExRateDSSD%d",i);
			lowEnergyExRate[i] = new TH1D(hname, "", 250,0,20e3);
			lowEnergyExRate[i]->GetXaxis()->SetTitle("Energy [MeV]");
			lowEnergyExRate[i]->GetYaxis()->SetTitle("Counts");
		}
    

#endif

	#ifdef OLD_OUTPUT
    	outputTree = new TTree("aida","aida");
	    outputTree->Branch("aida",&oldOutput,"T/l:Tfast/l:E/D:Ex/D:Ey/D:x/D:y/D:z/D:nx/I:ny/I:nz/I:ID/b");
	#endif
	#ifdef NEW_OUTPUT
    	outputTree = new TTree("aida","aida");
	    outputTree->Branch("aida",&newOutput,"T/l:Tfast/l:E/D:Ex/D:Ey/D:xMin/I:yMin/I:xMax/I:yMax/I:z/D:nx/I:ny/I:nz/I:ID/b");
	#endif
	#ifdef MERGER_OUTPUT
    	outputTree = new TTree("AIDA_hits","AIDA_hits");
	    outputTree->Branch("aida_hit",&mergerOutput,"T/l:Tfast/l:E/D:Ex/D:Ey/D:x/D:y/D:z/D:nx/I:ny/I:nz/I:ID/b");
	#endif

	implantStoppingCounter = 0;
	implantPairCounter = 0;
	implantWindowCounter = 0;
	implantEnergyMatchCount = 0;
	implantTimeMatchCounter = 0;
    currentTimestamp = 0;

};
int EventClustering::StartMonitor(bool monitor){
	onlineMonitor = monitor;
	if(monitor){
		serv = new THttpServer("http:8085");
		
		outFile.open("/home/npg/rates.txt");
	}
	return 0;
}

void EventClustering::InitialiseClustering(){
	decayMap.clear();
	implantMap.clear();
	#ifdef OLD_OUTPUT
		outputEvents.clear();
	#endif
	#ifdef NEW_OUTPUT
		outputEvents.clear();
	#endif
	
	outputEvents.clear();
	
	decayMapCurrent = false;

	for (int i = 0; i < Common::noDSSD; i++){
		for (int j = 0; j <2; j++){
			dssdDecayLists[i][j].clear();
			dssdImplantLists[i][j].clear();
			dssdSideMultiplicity[i][j] = 0;
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
		implantWindowCounter++;
		implantStoppingLayer = ImplantStoppingLayer();
		if(implantStoppingLayer >= 0){
			implantStoppingCounter++;
			PairClusters(implantStoppingLayer, implantEnergyDifference, dssdImplantLists);
		}

		#ifdef DEB_IMPLANT_STOPPING
			if(implantStoppingLayer > -1){
				positiveStopping ++;
			}
			else if(implantStoppingLayer == -1){
				negativeStopping++;
			}	

			std::cout << "New event" << std::endl;
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

	//Clusters have been formed and paired

	WriteToFile();

}
void EventClustering::ClusterMap(std::multimap<CalibratedADCDataItem,int> & eventMap){

	#ifdef CLUSTER_DECAY_DEB
		std::cout << "\n \n \nEvent map size " << eventMap.size() << std::endl;
		int clusterItem = 0;
    #endif

	if(eventMap.begin()->first.GetADCRange() == 0){
		decayMapCurrent = true;
	}

	Cluster eventCluster; //Defines the cluster class used

	for(clusterIt = eventMap.begin(); clusterIt != eventMap.end(); clusterIt++){
		//Iterator that loops through the map of decays from the beginning to the end

		if(((clusterIt->first.GetStrip()-eventCluster.GetStrip()) == 1 || (clusterIt->first.GetStrip()-eventCluster.GetStrip()) == -1)
			&& ((clusterIt->first.GetEnergy()>energyThreshold && decayMapCurrent) || !decayMapCurrent)){
			//If current event is adjacent to last strip added to the cluster or cluster currently has no events proceed

			if(eventCluster.GetTimestampDifference(clusterIt->first.GetTimestamp())<=2000){
				//If the current event is within 2us of events within the cluster proceed

				if(eventCluster.GetSide() == clusterIt->first.GetSide() && eventCluster.GetDSSD() == clusterIt->first.GetDSSD()){
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
		else if ( eventCluster.GetStrip() == -5 && ((clusterIt->first.GetEnergy()>energyThreshold && decayMapCurrent)|| !decayMapCurrent)){
			//If cluster currently doesn't have items associated with it add to cluster

			eventCluster.AddEventToCluster(clusterIt->first);
				#ifdef CLUSTER_DECAY_DEB
					clusterItem = clusterItem + 1;
					std::cout << "New cluster." <<std::endl;
					std::cout << "Cluster item: " << clusterItem << " DSSD: " << clusterIt->first.GetDSSD() << " Side: " << clusterIt->first.GetSide() <<
								 " Strip " << clusterIt->first.GetStrip() << " Timestamp: " << clusterIt->first.GetTimestamp() <<std::endl;
				#endif

		}
		else if (((clusterIt->first.GetStrip()-eventCluster.GetStrip()) > 1 || (clusterIt->first.GetStrip()-eventCluster.GetStrip()) < -1)
				&& ((clusterIt->first.GetEnergy()>energyThreshold && decayMapCurrent)||!decayMapCurrent)){
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
		else if((clusterIt->first.GetSide() != eventCluster.GetSide()) &&
				((clusterIt->first.GetEnergy() > energyThreshold && decayMapCurrent) || !decayMapCurrent)){
			//Cluster is finished

			#ifdef CLUSTER_DECAY_DEB
				std::cout << "\nSides are different. Item not added to cluster creating new cluster." <<std::endl;
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
	}//Close for loop iterating round cluster map
	if(eventCluster.GetDSSD() != -5){
		//Cluster isnt empty
		CloseCluster(eventCluster);
	}
	#ifdef HISTOGRAMMING
		for(int i=0; i < Common::noDSSD;i++){
			xyMultiplicity[i]->Fill(dssdSideMultiplicity[i][0],dssdSideMultiplicity[i][1]);
		}
	#endif
}
void EventClustering::CloseCluster(Cluster & eventCluster){

	int dssd = eventCluster.GetDSSD();
	int side = eventCluster.GetSide();

	switch(eventCluster.GetADCRange()){
		case 0:
			//decay event cluster, store with decay clusters
			dssdDecayLists[dssd][side].push_back(eventCluster);
			//Add 1 to the multiplicity of the event side
			dssdSideMultiplicity[dssd][side] += eventCluster.GetSize();
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
			if(stoppingLayer < (Common::noDSSD)){
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
						upstreamEvents = false;
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
void EventClustering::PairClusters(int dssd, double equalEnergyRange,std::deque<Cluster> clusterLists[][2]){
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
					if(equalEnergyRange == decayEnergyDifference){
						lowEnergyExEy[dssd]->Fill(clusterSide1It->GetEnergy(),clusterSide0It->GetEnergy());
					}
					else if(equalEnergyRange == implantEnergyDifference){
						highEnergyExEy[dssd]->Fill(clusterSide1It->GetEnergy(),clusterSide0It->GetEnergy());
					}
				#endif
				if(abs(clusterSide0It->GetEnergy()-clusterSide1It->GetEnergy()) <= equalEnergyRange){
					//Is the difference between the two clusters less than the equal energy cuts
					if(equalEnergyRange == implantEnergyDifference){
						implantEnergyMatchCount++;
					}

					if((clusterSide0It->GetTimestampDifference(clusterSide1It->GetTimestampMin())<4000) ||
						 (clusterSide0It->GetTimestampDifference(clusterSide1It->GetTimestampMax())<4000) ){
						if(equalEnergyRange == implantEnergyDifference){
							implantTimeMatchCounter++;
						}

						//Set the x and y multiplicity of the clusters
						clusterSide0It->SetMultiplicity(dssdSideMultiplicity[dssd][0]);
						clusterSide1It->SetMultiplicity(dssdSideMultiplicity[dssd][1]);
						//Clusters paired on both time and energy
						#ifdef OLD_OUTPUT
							MergerOutputOld pairedCluster(*clusterSide0It, *clusterSide1It);
							outputEvents.emplace(pairedCluster.GetTimestamp(),pairedCluster);
						#endif
						#ifdef NEW_OUTPUT
							MergerOutputNewTrial pairedCluster(*clusterSide0It, *clusterSide1It);
							outputEvents.emplace(pairedCluster.GetTimestamp(),pairedCluster);
						#endif
						MergerOutput pairedCluster;
						if(dssdPNisX[dssd]){
							pairedCluster.BuildItem(*clusterSide0It, *clusterSide1It);
						}
						else{
							pairedCluster.BuildItem(*clusterSide1It, *clusterSide0It);
						}
						outputEvents.emplace(pairedCluster.GetTimestamp(),pairedCluster);
						

						#ifdef HISTOGRAMMING
							if(equalEnergyRange == decayEnergyDifference){
								if(dssdPNisX[dssd]){
									lowEnergyExEyPair[dssd]->Fill(clusterSide1It->GetEnergy(),clusterSide0It->GetEnergy());
								}
								else{
									lowEnergyExEyPair[dssd]->Fill(clusterSide0It->GetEnergy(),clusterSide1It->GetEnergy());
								}
								lowEnergyXY[dssd]->Fill(pairedCluster.GetX(),pairedCluster.GetY());
                                //Have a paired CARME event add it to the CARME histograms
                                if(pairedCluster.GetTimestamp() > currentTimestamp){
                                    if(currentTimestamp == 0){
                                        currentTimestamp = pairedCluster.GetTimestamp()+1000000000;
                                    }
                                    else{
                                        currentTimestamp+=1000000000;
                                        for(int z = 0; z < Common::noDSSD; z++) {
                                            for (int x = 0; x < 128; x++) {
                                                for (int y = 0; y < 128; y++) {
                                                    lowEnergyXYRate[z]->SetBinContent(x + 1, y + 1, xyEvents[z * 128 * 128 + y * 128 + x]);
                                                }
                                            }
											/*
											double AvgBeamPosition=0;
											if(xyEvents[127*128+9]>0) {
												for(int y = 110; y < 127; y++){
													if(xyEvents[y*128+9]>0){
														double pixelsRatio = xyEvents[y*128+9]/xyEvents[127*128+9];
														std::clog<<xyEvents[127*128+9]<<std::endl;
														double pixelPosRatio = pow(pixelsRatio, -0.25);
														double strip_difference = 0.78125 * (129-y); 

														double pixel_position = (strip_difference / (1. - (1/pixelPosRatio))) - (0.78125/2);
														double beam_position = pixel_position - strip_difference;
														AvgBeamPosition += beam_position;
													}
												}
											AvgBeamPosition*=1./16;
											std::clog<<AvgBeamPosition<<std::endl;
											}
											*/
                                        }
                                        std::fill(std::begin(xyEvents), std::end(xyEvents), 0);

                                        //for(auto & xVsExEvent : xVsExEvents){
                                            //Sort out the dssd allocation
                                            //lowEnergyExXRate[std::get<0>(xVsExEvent)]->Fill(std::get<1>(xVsExEvent), std::get<2>(xVsExEvent));
                                        //}
                                        xVsExEvents.clear();


										//----------------------------------------------------------------------
										//Clear events more than 20s old
										//----------------------------------------------------------------------
										for(int i = 0; i< Common::noDSSD; i++){
											lowEnergyExXRate[i]->Reset();
											lowEnergyEyYRate[i]->Reset();
											lowEnergyExRate[i]->Reset();
											lowEnergyEyRate[i]->Reset();
										}
										xVsExMapLowIt = xVsExMap.lower_bound(currentTimestamp-20000000000);
										if(xVsExMapLowIt != xVsExMap.begin()){
											xVsExMap.erase(xVsExMap.begin(), std::prev(xVsExMapLowIt));
										}
										for(xVsExMapIt = xVsExMap.begin(); xVsExMapIt != xVsExMap.end(); xVsExMapIt++){
											lowEnergyExXRate[std::get<0>(xVsExMapIt->second)]->Fill(std::get<1>(xVsExMapIt->second), std::get<2>(xVsExMapIt->second));
											lowEnergyExRate[std::get<0>(xVsExMapIt->second)]->Fill(std::get<2>(xVsExMapIt->second));
										}

										//----------------------------------------------------------------------
										//Clear events more than 20s old
										//----------------------------------------------------------------------
										yVsEyMapLowIt = yVsEyMap.lower_bound(currentTimestamp-20000000000);
										if(yVsEyMapLowIt != yVsEyMap.begin()){
											yVsEyMap.erase(yVsEyMap.begin(), std::prev(yVsEyMapLowIt));
										}
										rate1s = 0;
										rate20s = 0;
										for(yVsEyMapIt = yVsEyMap.begin(); yVsEyMapIt != yVsEyMap.end(); yVsEyMapIt++){
											lowEnergyEyYRate[std::get<0>(yVsEyMapIt->second)]->Fill(std::get<1>(yVsEyMapIt->second), std::get<2>(yVsEyMapIt->second));
											lowEnergyEyRate[std::get<0>(yVsEyMapIt->second)]->Fill(std::get<2>(yVsEyMapIt->second));

											if(std::get<2>(yVsEyMapIt->second) >= lowRutherford && std::get<2>(yVsEyMapIt->second) <= highRutherford){
												rate20s++;
												if (yVsEyMapIt->first > (currentTimestamp - 1000000000)){
													rate1s++;
												}
											}
										}
										if(onlineMonitor){
											outFile << rate1s << "\t" << rate20s << "\n";
										}

                                        //for(auto & yVsEyEvent : yVsEyEvents){
                                            //lowEnergyEyYRate[std::get<0>(yVsEyEvent)]->Fill(std::get<1>(yVsEyEvent), std::get<2>(yVsEyEvent));
                                        //}
                                        yVsEyEvents.clear();
                                    }
                                gSystem->ProcessEvents();
                                }
                                gSystem->ProcessEvents();
                                lowEnergyXYTotal[dssd]->Fill(pairedCluster.x, pairedCluster.y);
                                xyEvents[dssd * 128 * 128 + (int)pairedCluster.y * 128 + (int)pairedCluster.x]++;

                                lowEnergyExXTotal[dssd]->Fill(pairedCluster.x, pairedCluster.Ex);
                                //xVsExEvents.emplace_back(std::make_tuple((int)pairedCluster.z,pairedCluster.x, pairedCluster.Ex));
								xVsExMap.emplace(pairedCluster.GetTimestamp(),std::make_tuple((int)pairedCluster.z,pairedCluster.x, pairedCluster.Ex));

                                lowEnergyEyYTotal[dssd]->Fill(pairedCluster.y, pairedCluster.Ey);
                                //yVsEyEvents.emplace_back(std::make_tuple((int)pairedCluster.z,pairedCluster.y, pairedCluster.Ey));
								yVsEyMap.emplace(pairedCluster.GetTimestamp(),std::make_tuple((int)pairedCluster.z,pairedCluster.y, pairedCluster.Ey));

								lowEnergyEyTotal[dssd]->Fill(pairedCluster.Ey);
								lowEnergyExTotal[dssd]->Fill(pairedCluster.Ex);
							}
							else if(equalEnergyRange == implantEnergyDifference){
								if(dssdPNisX[dssd]){
									highEnergyExEyPair[dssd]->Fill(clusterSide1It->GetEnergy(),clusterSide0It->GetEnergy());
								}
								else{
									highEnergyExEyPair[dssd]->Fill(clusterSide0It->GetEnergy(),clusterSide1It->GetEnergy());
								}
								highEnergyXY[dssd]->Fill(pairedCluster.GetX(),pairedCluster.GetY());
							}
						#endif
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
					#endif
					}
					#ifdef DEB_CLUSTER_PAIR
					else{
						if(!clusterPairE){
							pairedEnergy++;
							clusterPairE = true;
						}
						std::cout << "Percentage of paired event making E but not T" << (double)pairedEnergy/clusterTotal<< std::endl;
						std::cout << "Cluster 0 Timsestamps: " << clusterSide0It->GetTimestampMin() << " " << clusterSide0It->GetTimestampMax() <<std::endl;
						std::cout << "Cluster 0 Timsestamps: " << clusterSide1It->GetTimestampMin() << " " << clusterSide1It->GetTimestampMax() <<std::endl;
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
	std::cout << "Number of implant event windows processed: " << implantWindowCounter << std::endl;
	std::cout << "Number of stopping layers calculated: " << implantStoppingCounter << std::endl;
	std::cout << "Number of implant events with stopping layer making energy cut: " << implantEnergyMatchCount << std::endl;
	std::cout << "Number of implant events making energy cut also making time cut: " << implantTimeMatchCounter << std::endl;
	std::cout << "Percentage of implant event windows being written to file: " << (double)implantTimeMatchCounter/((double)implantWindowCounter) << std::endl;


	#ifdef HISTOGRAMMING
		for(int i =0; i<Common::noDSSD;i++){
			lowEnergyExEy[i]->Write();
		}
		for(int i =0; i<Common::noDSSD;i++){
			highEnergyExEy[i]->Write();
		}
		for(int i =0; i<Common::noDSSD;i++){
			lowEnergyExEyPair[i]->Write();
		}
		for(int i =0; i<Common::noDSSD;i++){
			highEnergyExEyPair[i]->Write();
		}
		for(int i =0; i<Common::noDSSD;i++){
			lowEnergyXY[i]->Write();
		}
		for(int i =0; i<Common::noDSSD;i++){
			highEnergyXY[i]->Write();
		}
		for(int i =0; i<Common::noDSSD;i++){
			xyMultiplicity[i]->Write();
		}
        for(int i = 0; i < Common::noDSSD;i++) {
            lowEnergyXYRate[i]->Write();
        }
        for(int i = 0; i < Common::noDSSD;i++) {
            lowEnergyXYTotal[i]->Write();
        }
        for(int i = 0; i < Common::noDSSD;i++) {
            lowEnergyExXRate[i]->Write();
        }
        for(int i = 0; i < Common::noDSSD;i++) {
            lowEnergyEyYRate[i]->Write();
        }
        for(int i = 0; i < Common::noDSSD;i++) {
            lowEnergyExXTotal[i]->Write();
        }
        for(int i = 0; i < Common::noDSSD;i++) {
            lowEnergyEyYTotal[i]->Write();
        }
		for(int i = 0; i < Common::noDSSD;i++) {
            lowEnergyEyTotal[i]->Write();
        }
		for(int i = 0; i < Common::noDSSD;i++) {
            lowEnergyExTotal[i]->Write();
        }
		for(int i = 0; i < Common::noDSSD;i++) {
            lowEnergyEyRate[i]->Write();
        }
		for(int i = 0; i < Common::noDSSD;i++) {
            lowEnergyExRate[i]->Write();
        }
	#endif

	#ifdef OLD_OUTPUT
		outputTree->Write();
	#endif
	#ifdef NEW_OUTPUT
		outputTree->Write();
	#endif
	#ifdef MERGER_OUTPUT
		outputTree->Write();
	#endif
}
void EventClustering::WriteToFile(){
	//Function to deal with writing to root output file

	#ifdef OLD_OUTPUT
		for(eventsIt = outputEvents.begin(); eventsIt != outputEvents.end(); eventsIt++){
			oldOutput = eventsIt->second;
			outputTree->Fill();
		}
	#endif
	#ifdef NEW_OUTPUT
		for(eventsIt = outputEvents.begin(); eventsIt != outputEvents.end(); eventsIt++){
			newOutput = eventsIt->second;
			outputTree->Fill();
		}
	#endif
	#ifdef MERGER_OUTPUT
		for(eventsIt = outputEvents.begin(); eventsIt != outputEvents.end(); eventsIt++){
			mergerOutput = eventsIt->second;
			outputTree->Fill();
		}
	#endif
}
int EventClustering::InitialisePNArray(bool pnArray[Common::noDSSD]){
	for(int i = 0; i < Common::noDSSD; i++){
		dssdPNisX[i] = pnArray[i];
	}
	return 0;
}