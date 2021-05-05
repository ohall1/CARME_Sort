#ifndef _EVENTCLUSTERING_HPP
#define _EVENTCLUSTERING_HPP

#include "Common.hpp"

#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH2I.h"

class EventClustering{
	private:
		std::multimap<CalibratedADCDataItem,int> decayMap;			//Map to store the calibrated decay events
		std::multimap<CalibratedADCDataItem,int> implantMap;		//Map to store the calibrated implant events

		std::multimap<CalibratedADCDataItem,int>::iterator clusterIt; 	//Iterator to loop through the maps to be clustered

		std::deque<Cluster> dssdDecayLists[Common::noDSSD][2];						//Array of lists to store decay clusters in
		std::deque<Cluster> dssdImplantLists[Common::noDSSD][2];						//Array of lists to store implant clusters in
		std::deque<Cluster>::iterator clusterSide0It;					//Iterator for side 0
		std::deque<Cluster>::iterator clusterSide1It;					//Iterator for clusters on side 1

		short dssdSideMultiplicity[Common::noDSSD][2];					//Stores the multiplicity of each side of the detector

		const double decayEnergyDifference = 450.0;						//Cluster energy difference in keV for decay clusters
		const double implantEnergyDifference = 1000.0;					//Cluster energy difference in MeV for implant clusters
		const double energyThreshold = 0.0;								//Thresholds used for checking decays

		int implantStoppingCounter;
		int implantPairCounter;
		int implantWindowCounter;
		int implantEnergyMatchCount;
		int implantTimeMatchCounter;

		bool decayMapCurrent;													//Bool to keep track of whether current map is a decay

		short implantStoppingLayer;										//DSSD that an implant stops in

		#ifdef OLD_OUTPUT
		//OLD_OUPUT will use the original version of the merger where a search square is drawn around the implant
			TTree * outputTree;										//Tree to store output

			MergerOutputOld oldOutput;									//Variable to be used as the old output
			std::multimap<ULong_t, MergerOutputOld> outputEvents;		//Map to time order paired clusters in each event before being written to map
			std::multimap<ULong_t, MergerOutputOld>::iterator eventsIt;	//Iterator for output event map
		#endif
		#ifdef NEW_OUTPUT
			TTree * outputTree;										//Tree to store output

			MergerOutputNewTrial newOutput;									//Variable to be used as the old output
			std::multimap<ULong_t, MergerOutputNewTrial> outputEvents;		//Map to time order paired clusters in each event before being written to map
			std::multimap<ULong_t, MergerOutputNewTrial>::iterator eventsIt;	//Iterator for output event map
		#endif
		#ifdef MERGER_OUTPUT
			TTree * outputTree;										//Tree to store output

			MergerOutput mergerOutput;									//Variable to be used as the old output
			std::multimap<ULong_t, MergerOutput> outputEvents;		//Map to time order paired clusters in each event before being written to map
			std::multimap<ULong_t, MergerOutput>::iterator eventsIt;	//Iterator for output event map


		#endif

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
			TH2D * lowEnergyXY[Common::noDSSD];
			TH2D * highEnergyXY[Common::noDSSD];
			TH2I * xyMultiplicity[Common::noDSSD];
		#endif

		void ClusterMap(std::multimap<CalibratedADCDataItem,int> & eventMap);	//Cluster the maps once ready
		void CloseCluster(Cluster & decayCluster);								//Close, store and reset cluster when no more events to add
		short ImplantStoppingLayer();											//Find the stopping layer for an implant event
		void PairClusters(int dssd, double equalEnergyRange,std::deque<Cluster>  clusterLists[][2]);
		void WriteToFile();

	public:
		EventClustering();
		~EventClustering(){};

		void InitialiseClustering();
		void AddEventToMap(CalibratedADCDataItem & dataItem);		//Add event to the applicable map to the event type
		void ProcessMaps();											//Reached the end of the event process the maps
		void CloseClustering();										//At end of program close clustering

};

#endif