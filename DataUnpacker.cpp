#include "DataUnpacker.hpp"

DataUnpacker::DataUnpacker(){};

EventBuilder * DataUnpacker::InitialiseDataUnpacker(std::string syncOffsetFile){
  
  //Initialise event builder class
  dataCheck = true;
  EventBuilder *myEventBuilderPoint;
  myEventBuilderPoint = &myEventBuilder;
    
  //Initialise all the values that will be used in the unpacker process
  for (int i = 0; i < Common::noFEE64; i++){
    pauseItemCounter[i] = 0;
    resumeItemCounter[i] = 0;
    sync100Counter[i] = 0;
    timestampMSBStatus[i] = false;
  }

  correlationScalerData0 = 0;
  correlationScalerData1 = 0;
  totalSYNC100 = 0;
  totalPauseItem = 0;
  totalResumeItem = 0;
  totalDataWords = 0;
  totalDecayWords = 0;
  totalImplantWords = 0;
  totalScalerItems = 0;
  correlationScalerChangeCounter = 0;
  
  firstTimestamp = 0;
  
  correlationScalerStatus = false;

  scalerTree = new TTree("scalerTree","scalerTree");
  scalerTree->Branch("scalers",&scalerOut,"T/l:scaler/l:infoCode/B:fee/B");
  
#ifndef NOSYNCOFFSETS
  std::cout << "Setting FEE64 sync offsets... " << std::endl;
  ReadSyncOffsets(syncOffsetFile);
  for(int i=0; i< Common::noFEE64; i++){
    std::cout << "\tFEE " << i+1 << ": " << Common::offsetMSBFEE64[i] << std::endl;
  }
#endif

#ifdef HISTOGRAMMING
  lowEnergyChannelADC = new TH2D("lowEnergyChannelADC","",1536,0,1536,5e2,0,65536);
  highEnergyChannelADC = new TH2D("highEnergyChannelADC","",1536,0,1536,5e2,0,65536);
  deltaCorrelationScaler = new TH1D("deltaCorrelationScaler","",10000,-5000,5000);
  deltaCorrScalerTimestamp = new TH2D("deltaSYNC100Timestamp",";FEE-1;Time between consecutive SYNC100 data items [ns]",24,0,24,25E3,0,25E6);
  syncOffsets = new TH2D("syncOffsets",";FEE-1;Timestamp [1.31072ms]",Common::noFEE64,0,Common::noFEE64,5000,0,65536E4);
  infoCodes = new TH1D("infoCodes",";Info code;Counts",16,0,16);
#endif
  
  return myEventBuilderPoint;
}

void DataUnpacker::BeginDataUnpacker(DataReader & dataReader){
  
  while(dataCheck){
    dataWordsList.clear();
    dataWordsList = dataReader.ReadFromBuffer();
    
    for(dataWordsListIt = dataWordsList.begin(); dataWordsListIt != dataWordsList.end(); dataWordsListIt++){
      
      dataCheck = UnpackWords(*dataWordsListIt);
    }
  }
  
  myEventBuilder.UnpackerFinished();
  
  return;
}

void DataUnpacker::ReadSyncOffsets(std::string syncOffsetFile){
  int fee64, offset;

  std::string line;

  std::ifstream syncOffsets(syncOffsetFile.data());
  if(syncOffsets.is_open()){
    while(syncOffsets.good()){
      getline(syncOffsets,line);
      auto commentLine = line.find("#");
      std::string dummyVar;
      auto newLine = line.substr(0,commentLine);
      if(newLine.size()>0){
	std::istringstream iss(line,std::istringstream::in);
	iss >> dummyVar;
	if(dummyVar == "syncOffset"){
	  iss >> fee64;
	  iss >> offset;
	  
	  if(fee64>0 && fee64<=Common::noFEE64){
	    Common::offsetMSBFEE64[fee64-1] = offset;
	  }
	}
      }
    }
  }
  else if(!syncOffsets.is_open()){
    std::cout << "WARNING! Unable to open FEE64 sync offsets file " << syncOffsetFile << ". Offsets will all default to zero. " << std::endl;
  }
  syncOffsets.close();
}

bool DataUnpacker::UnpackWords(std::pair < unsigned int, unsigned int> wordsIn){
  //Takes the data Words that have been read in and determines the data type and
  //unpacks the data accordingly.
  //Determines the data type of the two words. If 3 adc data and 2 is information data
  dataType = ( (wordsIn.first >>30) & 0x3);

  if (dataType ==3 ){
    //ADC data item - Unpack into ADCDataItem format
    adcDataItem.BuildItem(wordsIn);

    if (timestampMSBStatus[adcDataItem.GetFEE64ID()-1] && correlationScalerStatus){   //If timestampMSB has been obtained from inforation data set the timestamp of the adc data

#ifdef OFFSETS
      adcDataItem.BuildTimestamp(timestampMSB);

      //If histogramming turned on add event information to histograms
#ifdef HISTOGRAMMING
      if(adcDataItem.GetADCRange() == 0){
	lowEnergyChannelADC->Fill((((adcDataItem.GetFEE64ID()-1)*Common::noChannel)+adcDataItem.GetChannelID()),adcDataItem.GetADCData());
      }
      else if(adcDataItem.GetADCRange() == 1){
	highEnergyChannelADC->Fill((((adcDataItem.GetFEE64ID()-1)*Common::noChannel)+adcDataItem.GetChannelID()),adcDataItem.GetADCData());
					
      }
#endif

      //Send ADC item to the event builder to be built
      myEventBuilder.AddADCEvent(adcDataItem);
      totalDataWords++;
    }
    else if(timestampMSBStatus[adcDataItem.GetFEE64ID()-1]){
#endif
      adcDataItem.BuildTimestamp(timestampMSB);

      //If histogramming turned on add event information to histograms
#ifdef HISTOGRAMMING
      if(adcDataItem.GetADCRange() == 0){
	lowEnergyChannelADC->Fill((((adcDataItem.GetFEE64ID()-1)*Common::noChannel)+adcDataItem.GetChannelID()),adcDataItem.GetADCData());
      }
      else if(adcDataItem.GetADCRange() == 1){
	highEnergyChannelADC->Fill((((adcDataItem.GetFEE64ID()-1)*Common::noChannel)+adcDataItem.GetChannelID()),adcDataItem.GetADCData());
      }
#endif

      //Send ADC item to the event builder to be built
      myEventBuilder.AddADCEvent(adcDataItem);
      totalDataWords++;
    }

    if(firstTimestamp==0) {firstTimestamp = adcDataItem.GetTimestamp();}
    else if(firstTimestamp > 0 && adcDataItem.GetTimestamp()-firstTimestamp < 20E8) {syncOffsets->Fill(adcDataItem.GetFEE64ID()-1,adcDataItem.GetTimestamp()-firstTimestamp);}

    return true;
  }
  else if (dataType == 2){
    //Information data item

    informationDataItem.BuildItem(wordsIn);

    infoCodes->Fill(informationDataItem.GetInfoCode());

    if(informationDataItem.GetInfoCode() == 2){				//Pause information item
      timestampMSB = informationDataItem.GetTimestampMSB();
      timestampMSBStatus[informationDataItem.GetFEE64ID()-1] = false;
      pauseItemCounter[informationDataItem.GetFEE64ID()-1] += 1;

#ifdef DEB_UNPACKER
      std::cout << "\nTimestamp MSB Updated - " << std::dec << timestampMSB << " PAUSE information item\n" << std::endl;
#endif
    }
    else if(informationDataItem.GetInfoCode() == 3){		//Resume information item
      timestampMSB = informationDataItem.GetTimestampMSB();
      timestampMSBStatus[informationDataItem.GetFEE64ID()-1] = true;

      if(timestampMSB != informationDataItem.GetTimestampMSB() && informationDataItem.GetTimestampLSB() != 160){
	std::cout << "Timestamp MSB updated to: " << timestampMSB << " using RESUME information item from FEE " << informationDataItem.GetFEE64ID() << ". LSB: " << informationDataItem.GetTimestampLSB() << std::endl;
      }

      resumeItemCounter[informationDataItem.GetFEE64ID()-1] += 1;

#ifdef DEB_UNPACKER
      //std::cout << "\nTimestamp MSB Updated - " << timestampMSB << " RESUME information item\n" << std::endl;
#endif
    }
    else if(informationDataItem.GetInfoCode() == 4){		//SYNC100 information item
      if(timestampMSB != informationDataItem.GetTimestampMSB() && informationDataItem.GetTimestampLSB() != 160){
	std::cout << "Timestamp MSB updated to: " << timestampMSB << " using SYNC100 information item from FEE " << informationDataItem.GetFEE64ID() << " but LSB not 0xA0. LSB: " << informationDataItem.GetTimestampLSB() << std::endl;
      }
      timestampMSB = informationDataItem.GetTimestampMSB();
      timestampMSBStatus[informationDataItem.GetFEE64ID()-1] = true;
      sync100Counter[informationDataItem.GetFEE64ID()-1] += 1;
#ifdef HISTOGRAMMING
      if(prevTimestamp[informationDataItem.GetFEE64ID()-1] > 0){ deltaCorrScalerTimestamp->Fill(informationDataItem.GetFEE64ID()-1,informationDataItem.GetTimestamp()-prevTimestamp[informationDataItem.GetFEE64ID()-1]);}
#endif
      prevTimestamp[informationDataItem.GetFEE64ID()-1] = informationDataItem.GetTimestamp();	


#ifdef DEB_UNPACKER
      std::cout << "\nTimestamp MSB Updated - " << timestampMSB << " SYNC100 information item. FEE64:" << informationDataItem.GetFEE64ID() << " LSB:" << informationDataItem.GetTimestampLSB() << "\n" << std::endl;
#endif
    }
    else if(informationDataItem.GetInfoCode() == 8/* && informationDataItem.GetFEE64ID() == Common::masterFEE64*/){		//Correlation scaler data item

      if(timestampMSBStatus[informationDataItem.GetFEE64ID()-1]){//No MSB information in scaler so can't set timestamp in constructor
	informationDataItem.SetTimestamp(timestampMSB);

	if(informationDataItem.GetCorrScalerIndex() == 0){ //Scaler is split across three data word pairs need to combine the three to get the scaler
	  correlationScalerData0 = informationDataItem.GetCorrScalerTimestamp();
	}
	else if(informationDataItem.GetCorrScalerIndex() == 1){
	  correlationScalerData1 = informationDataItem.GetCorrScalerTimestamp();
	}
	else if(informationDataItem.GetCorrScalerIndex() == 2 && correlationScalerData0 != 0 && correlationScalerData1 != 0){
	  correlationScaler = (informationDataItem.GetCorrScalerTimestamp() << 32) | (correlationScalerData1 << 16)
	    | (correlationScalerData0);	

	  //std::cout << "Correlation scaler event! Scaler: " << correlationScaler << "\t AIDA timestamp: " << informationDataItem.GetTimestamp() << std::endl;

	  correlationScalerOffset = (4 * correlationScaler) - informationDataItem.GetTimestamp();
    
	  if(myEventBuilder.GetCorrelationScalerOffset()==0){
	    //Set the correlation scaler offset in the event builder
	    myEventBuilder.SetCorrelationScaler(correlationScalerOffset);

	    std::cout << "Setting the correlation scaler offset to: " << correlationScalerOffset << std::endl;
	    correlationScalerStatus = true;
#ifdef HISTOGRAMMING
	    deltaCorrelationScaler->Fill(correlationScalerOffset-myEventBuilder.GetCorrelationScalerOffset());
#endif
	  }
	  else if (myEventBuilder.GetCorrelationScalerOffset() != correlationScalerOffset){
	    std::cout << "Warning correlation scaler offset changed mid run. Old Offset = " << myEventBuilder.GetCorrelationScalerOffset()
		      << " - New offset = " << correlationScalerOffset << ". Was a sync pulse sent in the middle of a run?" <<std::endl;
#ifdef HISTOGRAMMING
	    deltaCorrelationScaler->Fill(correlationScalerOffset-myEventBuilder.GetCorrelationScalerOffset());
#endif
	    correlationScalerChangeCounter++;
	  }

#ifdef DEB_CORRELATION
	  std::cout << "Correlation scaler " << correlationScaler << " Offset = " << correlationScalerOffset << "\tat timestamp: " << informationDataItem.GetTimestamp() << std::endl;
#endif

	  //Add to scaler time-ordering multimap
	  informationDataItem.SetCorrScaler(correlationScaler);
	  AddScalerEvent(informationDataItem);
	  totalScalerItems++;

	}  
      } 

    }
    return true;

  }
  else if (dataType == 0){
    //Have reached the end of the data file break out of the while loop
    std::cout << "Reached the end of reading in the data" << std::endl;
    return false;
  }
}

void DataUnpacker::AddScalerEvent(InformationDataItem & infoItem){

  scalerMap.emplace(infoItem.GetTimestamp(),infoItem);
  InformationDataItem mergedItem;

  scalerMapItEnd = scalerMap.end();
  scalerMapItEnd--;
  scalerMapItStart = scalerMap.begin();

  if( scalerMapItEnd->second.GetTimestamp() - scalerMapItStart->first > 1e8 && scalerMap.size()>1){
    while(scalerMapItEnd->first - scalerMapItStart->first > 1e8){
      
      mergedItem = scalerMapItStart->second;

      ApplyCorrelationScalerOffset(mergedItem);	

      scalerOut = ScalerOutput(mergedItem);
      
      scalerTree->Fill();
      
      scalerMap.erase(scalerMapItStart);
      scalerMapItStart = scalerMap.begin();
    }
  }
      
  return;
}

void DataUnpacker::ApplyCorrelationScalerOffset(InformationDataItem & informationDataItem){

  //Applys the correlation scaler offset to the timestamp
  //This allows the timestamp to be synced with BRIKEN
  informationDataItem.SetTimestampFull(informationDataItem.GetTimestamp()+correlationScalerOffset);
}

void DataUnpacker::CloseUnpacker(){
  //Reached the end of reading in data.
  //Calculate the total of data items
  for(int i = 0; i < Common::noFEE64; i++){
    totalPauseItem += pauseItemCounter[i];
    totalResumeItem += resumeItemCounter[i];
    totalSYNC100 += sync100Counter[i];
  }

  if(scalerMap.size()>0){
    while(!scalerMap.empty()){
      scalerMapItStart = scalerMap.begin();
      scalerOut = ScalerOutput(scalerMapItStart->second);
      scalerTree->Fill();
      scalerMap.erase(scalerMapItStart);
    }
  }

  scalerTree->Write();
  
  totalDataWords = myEventBuilder.GetImplantWords() + myEventBuilder.GetDecayWords();

  std::cout << "Unpacker stage finished." << std::endl;
  std::cout << "Total number of data words unpacked - " << totalDataWords <<std::endl;
  std::cout << "Total number of implant words unpacked - " << myEventBuilder.GetImplantWords() <<std::endl;
  std::cout << "Which were built into " << myEventBuilder.GetImplantEvents() << " events." << std::endl;
  std::cout << "Total number of decay words unpacked - " << myEventBuilder.GetDecayWords() << std::endl;
  std::cout << "Of which " << myEventBuilder.GetPulserWords() << " were pulser items in " 
	    << myEventBuilder.GetPulserEvents() << " pulser events." <<std::endl;
  std::cout << "With " << myEventBuilder.GetDecayWords()-myEventBuilder.GetPulserWords() << " words being identified as part low energy events." << std::endl;
  std::cout << "Which were built into " << myEventBuilder.GetDecayEvents() << " events." << std::endl;
  std::cout << "Total number of PUASE statements - " << totalPauseItem << std::endl;
  std::cout << "Total number of RESUME statements - " << totalResumeItem << std::endl;
  std::cout << "Total number of SYNC100 pulses - " << totalSYNC100 << std::endl;
  std::cout << "Total number of scaler items - " << totalScalerItems << std::endl;
  std::cout << "SYNC100 pulses per FEE - " << std::endl;
  for(int i=0; i< Common::noFEE64;i++) {std::cout << "\t" << i+1;}
  std::cout << "" << std::endl;
  for(int i=0; i< Common::noFEE64;i++) {std::cout << "\t" << sync100Counter[i];}
  std::cout << "" << std::endl;
  std::cout << "Changes in the correlation scaler - " << correlationScalerChangeCounter << std::endl; 

#ifdef HISTOGRAMMING
  lowEnergyChannelADC->Write();
  highEnergyChannelADC->Write();
  deltaCorrelationScaler->Write();
  deltaCorrScalerTimestamp->Write();
  syncOffsets->Write();
  infoCodes->Write();
#endif

  return;
}
