#include "DataUnpacker.hpp"

DataUnpacker::DataUnpacker(){};

EventBuilder * DataUnpacker::InitialiseDataUnpacker(){

	//Initialise event builder class
	dataCheck = true;
	EventBuilder *myEventBuilderPoint;
	myEventBuilderPoint = &myEventBuilder;


	//Initialise all the values that will be used in the unpacker process
	for (int i = 0; i < Common::noFEE64; i++){
		pauseItemCounter[i] = 0;
		resumeItemCounter[i] = 0;
		sync100Counter[i] = 0;
	}
	correlationScalerData0 = 0;
	correlationScalerData1 = 0;
	totalSYNC100 = 0;
	totalPauseItem = 0;
	totalResumeItem = 0;
	totalDataWords = 0;
	totalDecayWords = 0;
	totalImplantWords = 0;
	correlationScalerChangeCounter = 0;

	correlationScalerStatus = false;
	timestampWR48Status = false;
	timestampWR64Status = false;

	#ifdef HISTOGRAMMING
		lowEnergyChannelADC = new TH2D("lowEnergyChannelADC","",Common::noFEE64*Common::noChannel,0,Common::noFEE64*Common::noChannel,5e2,0,65536);
		highEnergyChannelADC = new TH2D("highEnergyChannelADC","",Common::noFEE64*Common::noChannel,0,Common::noFEE64*Common::noChannel,5e2,0,65536);
		deltaCorrelationScaler = new TH1D("deltaCorrelationScaler","",10000,-5000,5000);
		lowEnergyHitPattern = new TH1D("lowEnergyHitPattern","",Common::noFEE64*Common::noChannel,0,Common::noFEE64*Common::noChannel);
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

bool DataUnpacker::UnpackWords(std::pair < unsigned int, unsigned int> wordsIn){
	//Takes the data Words that have been read in and determines the data type and
	//unpacks the data accordingly.
	//Determines the data type of the two words. If 3 adc data and 2 is information data
	dataType = ( (wordsIn.first >>30) & 0x3);
	if (dataType ==3 ){
		//ADC data item - Unpack into ADCDataItem format
		adcDataItem.BuildItem(wordsIn);

		if (timestampWR48Status && timestampWR64Status){//If both upper parts of upper WR obtained continue

		#ifdef OFFSETS
			adcDataItem.BuildTimestamp(timestampWR48, timestampWR64);

			//If histogramming turned on add event information to histograms
			#ifdef HISTOGRAMMING
				if(adcDataItem.GetADCRange() == 0){
					lowEnergyChannelADC->Fill((((adcDataItem.GetFEE64ID()-1)*Common::noChannel)+adcDataItem.GetChannelID()),adcDataItem.GetADCData());
					lowEnergyHitPattern->Fill(((adcDataItem.GetFEE64ID()-1)*Common::noChannel)+adcDataItem.GetChannelID());
				}
				else if(adcDataItem.GetADCRange() == 1){
					highEnergyChannelADC->Fill((((adcDataItem.GetFEE64ID()-1)*Common::noChannel)+adcDataItem.GetChannelID()),adcDataItem.GetADCData());
				}
			#endif

			//Send ADC item to the event builder to be built
			myEventBuilder.AddADCEvent(adcDataItem);
			totalDataWords++;
		}
		else if(timestampWR48Status && timestampWR64Status){
		#endif
			adcDataItem.BuildTimestamp(timestampWR48, timestampWR64);
			if(adcDataItem.GetFEE64ID() < 0){
				return true;
			}
			else if(adcDataItem.GetFEE64ID() > 8){
				return true;
			}

			//If histogramming turned on add event information to histograms
			#ifdef HISTOGRAMMING
				if(adcDataItem.GetADCRange() == 0){
					lowEnergyChannelADC->Fill((((adcDataItem.GetFEE64ID()-1)*Common::noChannel)+adcDataItem.GetChannelID()),adcDataItem.GetADCData());
					lowEnergyHitPattern->Fill(((adcDataItem.GetFEE64ID()-1)*Common::noChannel)+adcDataItem.GetChannelID());
				}
				else if(adcDataItem.GetADCRange() == 1){
					highEnergyChannelADC->Fill((((adcDataItem.GetFEE64ID()-1)*Common::noChannel)+adcDataItem.GetChannelID()),adcDataItem.GetADCData());
				}
			#endif

			//Send ADC item to the event builder to be built
			myEventBuilder.AddADCEvent(adcDataItem);
			totalDataWords++;
		}

		return true;
	}
	else if (dataType == 2){
		//Information data item

		informationDataItem.BuildItem(wordsIn);

		if(informationDataItem.GetInfoCode() == 2){				//Pause information item
			//timestampWR48 = informationDataItem.GetTimestampWRUpper();
			//timestampWR48Status = true;
			pauseItemCounter[informationDataItem.GetFEE64ID()-1] += 1;

			#ifdef DEB_UNPACKER
				//std::cout << "\nTimestamp MSB Updated - " << timestampMSB << " PAUSE information item\n" << std::endl;
			#endif
		}
		else if(informationDataItem.GetInfoCode() == 3){		//Resume information item
			//timestampWR48 = informationDataItem.GetTimestampWRUpper();
			//timestampWR48Status = true;
			resumeItemCounter[informationDataItem.GetFEE64ID()-1] += 1;

			#ifdef DEB_UNPACKER
				//std::cout << "\nTimestamp MSB Updated - " << timestampMSB << " RESUME information item\n" << std::endl;
			#endif
		}
		else if(informationDataItem.GetInfoCode() == 4){		//SYNC100 information item WR 47:28
			timestampWR48 = informationDataItem.GetTimestampWRUpper();
			timestampWR48Status = true;
			sync100Counter[informationDataItem.GetFEE64ID()-1] += 1;

			#ifdef DEB_UNPACKER
				std::cout << "\nTimestampWR48  Updated - " << timestampWR48 << " SYNC100 information item. FEE#" << informationDataItem.GetFEE64ID() << "\n" <<std::endl;
			#endif
		}
		else if(informationDataItem.GetInfoCode() == 5){			//SYNC100 information item WR 63:48
			timestampWR64 = informationDataItem.GetTimestampWRUpper();
			timestampWR64Status = true;
			sync100Counter[informationDataItem.GetFEE64ID()-1]+= 1;
			#ifdef DEB_UNPACKER
				std::cout << "\nTimestampWR64  Updated - " << timestampWR64 << " SYNC100 information item. FEE#" << informationDataItem.GetFEE64ID() << "\n" <<std::endl;
			#endif
		}
		else if(informationDataItem.GetInfoCode() == 8){		//Correlation scaler data item

			//Scaler item input			
			if (informationDataItem.GetFEE64ID() == 1){
				informationDataItem.SetTimestamp(timestampWR48, timestampWR64);
				ADCDataItem correlationItem(informationDataItem.GetTimestamp(), informationDataItem.GetFEE64ID(), 3);
				myEventBuilder.AddADCEvent(correlationItem);
		  	}
			//std::cout << "Scaler item FEE: " << informationDataItem.GetFEE64ID() << " Timestamp: " << informationDataItem.GetTimestamp() << std::endl;
			
			/*if(timestampWR48Status && timestampWR64Status){//No MBS information in scaler so can't set timestamp in constructor
				informationDataItem.SetTimestamp(timestampWR48, timestampWR64);

				if(informationDataItem.GetCorrScalerIndex() == 0){ //Scaler is split across three data word pairs need to combine the three to get the scaler
					correlationScalerData0 = informationDataItem.GetCorrScalerTimestamp();
				}
				else if(informationDataItem.GetCorrScalerIndex() == 1){
					correlationScalerData1 = informationDataItem.GetCorrScalerTimestamp();
				}
				else if(informationDataItem.GetCorrScalerIndex() == 2){
					correlationScaler = (informationDataItem.GetCorrScalerTimestamp() << 32) | (correlationScalerData1 << 16)
										| (correlationScalerData0);	

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
						std::cout << "Correlation scaler " << correlationScaler << " Offset = " << correlationScalerOffset << std::endl;
					#endif
				}
			}*/ 

		}
		return true;

	}
	else if (dataType == 0){
		//Have reached the end of the data file break out of the while loop
		std::cout << "Reached the end of the files" << std::endl;		
		return false;
	}
}
void DataUnpacker::CloseUnpacker(){
	//Reached the end of reading in data.
	//Calculate the total of data items
	for(int i = 0; i < Common::noFEE64; i++){
		totalPauseItem += pauseItemCounter[i];
		totalResumeItem += resumeItemCounter[i];
		totalSYNC100 += sync100Counter[i];
	}

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
	std::cout << "Changes in the correlation scaler - " << correlationScalerChangeCounter << std::endl; 

	#ifdef HISTOGRAMMING
		lowEnergyHitPattern->Write();
		lowEnergyChannelADC->Write();
		highEnergyChannelADC->Write();
		deltaCorrelationScaler->Write();
	#endif

	return;
}
