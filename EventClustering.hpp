#ifndef _EVENTCLUSTERING_HPP
#define _EVENTCLUSTERING_HPP

#include "Common.hpp"

#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"

class EventClustering{
	private:
		std::multimap<CalibratedADCDataItem,int> decayMap;			//Map to store the calibrated decay events
		std::multimap<CalibratedADCDataItem,int> implantMap;		//Map to store the calibrated implant events

		std::multimap<CalibratedADCDataItem,int>::iterator clusterIt; 	//Iterator to loop through the maps to be clustered

		std::list<Cluster> dssdDecayLists[Common::noDSSD][2];						//Array of lists to store decay clusters in
		std::list<Cluster> dssdImplantLists[Common::noDSSD][2];						//Array of lists to store implant clusters in
		std::list<Cluster>::iterator clusterSide0It;					//Iterator for side 0
		std::list<Cluster>::iterator clusterSide1It;					//Iterator for clusters on side 1

		const double decayEnergyDifference = 300;						//Cluster energy difference in keV for decay clusters
		const double implantEnergyDifference = 1000;						//Cluster energy difference in MeV for implant clusters

		short implantStoppingLayer;										//DSSD that an implant stops in

		#ifdef DEB_IMPLANT_STOPPING
			int positiveStopping = 0;
			int negativeStopping = 0;
		#endif
		#ifdef DEB_CLUSTER_PAIR
			int pairedEnergy = 0;
			int pairedTime = 0;
			int clusterTotal = 0;
		#endif

		//Variables used if HISTOGRAMMING is enabled
		#ifdef HISTOGRAMMING
			TH2D * lowEnergyExEy[Common::noDSSD];
			TH2D * highEnergyExEy[Common::noDSSD];
			TH2D * lowEnergyExEyPair[Common::noDSSD];
			TH2D * highEnergyExEyPair[Common::noDSSD];
		#endif

		void ClusterMap(std::multimap<CalibratedADCDataItem,int> & eventMap);	//Cluster the maps once ready
		void CloseCluster(Cluster & decayCluster);								//Close, store and reset cluster when no more events to add
		short ImplantStoppingLayer();											//Find the stopping layer for an implant event
		void PairClusters(int dssd, double equalEnergyRange,std::list<Cluster>  clusterLists[][2]);

	public:
		EventClustering();
		~EventClustering(){};

		void InitialiseClustering();
		void AddEventToMap(CalibratedADCDataItem & dataItem);		//Add event to the applicable map to the event type
		void ProcessMaps();											//Reached the end of the event process the maps
		void CloseClustering();										//At end of program close clustering

};

#endif