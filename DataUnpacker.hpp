#ifndef _DATAUNPACKER_HPP
#define _DATAUNPACKER_HPP
#include "DataReader.hpp"
#include "DataItems.hpp"
#include "EventBuilder.hpp"
#include "Common.hpp"

//C++ INCLUDES
#include <utility>
#include <thread>

//ROOT INCLUDES
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"

//class DataReader; //Let DataUnpacker know about the existence of DataReader so it can access it

class DataUnpacker{
  
private:
  
  EventBuilder myEventBuilder;
  ADCDataItem adcDataItem;
  InformationDataItem informationDataItem;
  bool dataCheck;
  
  std::list <std::pair<unsigned int, unsigned int>> dataWordsList;
  std::list <std::pair<unsigned int, unsigned int>>::iterator dataWordsListIt;
  std::pair < unsigned int, unsigned int> dataWords;
  unsigned int word0, word1;

  std::multimap <unsigned long, InformationDataItem> scalerMap;
  std::multimap <unsigned long, InformationDataItem>::iterator scalerMapItStart;
  std::multimap <unsigned long, InformationDataItem>::iterator scalerMapItEnd;

  TTree *scalerTree;                            //Output tree for correlation scaler items
  ScalerOutput scalerOut;                       //Scaler data item for writing
  
  unsigned long correlationScalerData0 = 0;	//Used in calculating the correlation scaler
  unsigned long correlationScalerData1 = 0;	//
  unsigned long correlationScaler;		//Correlation scaler between AIDA and other DAQs
  long int correlationScalerOffset;		//Offset between correlation scaler and AIDA timstamp
  bool correlationScalerStatus;			//Status off correlation scaler
  unsigned long timestampMSB;			//Most significant bit of the timestamp
  int MSBsyncOffset[Common::noFEE64];           //Offsets between FEE64 modules in units of 2.62144ms

  unsigned int pauseItemCounter[Common::noFEE64];
  unsigned int resumeItemCounter[Common::noFEE64];
  unsigned int sync100Counter[Common::noFEE64];
  unsigned int correlationScalerChangeCounter;
  unsigned long prevTimestamp[Common::noFEE64]; //Keep track of previous corrScaler tiemstamps
  unsigned long firstTimestamp;
    
  unsigned int totalPauseItem;
  unsigned int totalResumeItem;
  unsigned int totalSYNC100;
  unsigned long totalDataWords;
  unsigned long totalImplantWords;       
  unsigned long totalDecayWords;
  unsigned long totalScalerItems;
  unsigned int dataType;		        //Data type of data words
  
  bool correlationStatus;		        //Bool to keep track of whether correlation scaler has been measured
  bool timestampMSBStatus[Common::noFEE64];	//Bool to keep track of whether timeStampMSB has been set
  
  

  //Histrogramming declarations
#ifdef HISTOGRAMMING
  
  TH1D * deltaCorrelationScaler;
  TH1D * timestampADCData;
  TH1D * timestampLowEnergy;
  TH1D * timestampHighEnergy;
  TH2D * lowEnergyChannelADC;
  TH2D * highEnergyChannelADC;
  TH2D * deltaCorrScalerTimestamp;
  TH2D * syncOffsets;
  TH1D * infoCodes;
  
#endif
  
  bool UnpackWords(std::pair < unsigned int, unsigned int> wordsIn);
  
public:
  DataUnpacker();
  ~DataUnpacker(){};
  void BeginDataUnpacker(DataReader & dataReader);
  EventBuilder * InitialiseDataUnpacker(std::string syncOffsetFile);
  void ReadSyncOffsets(std::string syncOffsetFile);
  void AddScalerEvent(InformationDataItem & infoItem);
  void ApplyCorrelationScalerOffset(InformationDataItem & informationDataItem);
  void CloseUnpacker();
};


#endif
