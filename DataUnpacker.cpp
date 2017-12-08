#include "DataUnpacker.hpp"

DataUnpacker::DataUnpacker(){};

void DataUnpacker::InitialiseDataUnpacker(){

	//Initialise event builder class
	EventBuilder myEventBuilder;

	//Initialise all the values that will be used in the unpacker process
	for (int i = 0; i < 24; i++){
		pauseItemCounter[i] = 0;
		resumeItemCounter[i] = 0;
		sync100Counter[i] = 0;
		correlationScalerData0[i] = 0;
		correlationScalerData1[i] = 0;
	}

	correlationScalerStatus = false;
}
void DataUnpacker::BeginDataUnpacker(DataReader & dataReader){

	for(;;){
	dataWords = dataReader.ReadFromBuffer();
	//std::cout << "first word " << dataWords.first << " second word " << dataWords.second << std::endl;

	UnpackWords(dataWords);

	}
}

void DataUnpacker::UnpackWords(std::pair < unsigned int, unsigned int> wordsIn){
	//Takes the data Words that have been read in and determines the data type and
	//unpacks the data accordingly.
	//Determines the data type of the two words. If 3 adc data and 2 is information data
	dataType = ( (wordsIn.first >>30) & 0x3);
	if (dataType ==3 ){
		//ADC data item - Unpack into ADCDataItem format
		ADCDataItem adcDataItem(wordsIn);

		if (timestampMSBStatus && correlationScalerStatus){//If timestampMSB has been obtained from inforation data set the timestamp of the adc data
			adcDataItem.BuildTimestamp(timestampMSB);

			//Send ADC item to the event builder to be built
			myEventBuilder.AddADCEvent(adcDataItem);
		}

		
	}
	else if (dataType == 2){
		//Information data item

		InformationDataItem informationDataItem(wordsIn);

		if(informationDataItem.GetInfoCode() == 2){				//Pause information item
			timestampMSB = informationDataItem.GetTimestampMSB();
			timestampMSBStatus = true;
			pauseItemCounter[informationDataItem.GetFEE64ID()-1] += 1;

			#ifdef DEB_UNPACKER
				std::cout << "\nTimestamp MSB Updated - " << timestampMSB << " PAUSE information item\n" << std::endl;
			#endif
		}
		else if(informationDataItem.GetInfoCode() == 3){		//Resume information item
			timestampMSB = informationDataItem.GetTimestampMSB();
			timestampMSBStatus = true;
			resumeItemCounter[informationDataItem.GetFEE64ID()-1] += 1;

			#ifdef DEB_UNPACKER
				std::cout << "\nTimestamp MSB Updated - " << timestampMSB << " RESUME information item\n" << std::endl;
			#endif
		}
		else if(informationDataItem.GetInfoCode() == 4){		//SYNC100 information item
			timestampMSB = informationDataItem.GetTimestampMSB();
			timestampMSBStatus = true;
			sync100Counter[informationDataItem.GetFEE64ID()-1] += 1;

			#ifdef DEB_UNPACKER
				std::cout << "\nTimestamp MSB Updated - " << timestampMSB << " SYNC100 information item\n" << std::endl;
			#endif
		}
		else if(informationDataItem.GetInfoCode() == 8){		//Correlation scaler data item

			if(timestampMSBStatus){//No MSB information in scaler so can't set timestamp in constructor
				informationDataItem.SetTimestamp(timestampMSB);

				if(informationDataItem.GetCorrScalerIndex() == 0){ //Scaler is split across three data word pairs need to combine the three to get the scaler
					correlationScalerData0[informationDataItem.GetFEE64ID()] = informationDataItem.GetCorrScalerTimestamp();
				}
				else if(informationDataItem.GetCorrScalerIndex() == 1){
					correlationScalerData1[informationDataItem.GetFEE64ID()] = informationDataItem.GetCorrScalerTimestamp();
				}
				else if(informationDataItem.GetCorrScalerIndex() == 2){
					correlationScaler = (informationDataItem.GetCorrScalerTimestamp() << 32) | (correlationScalerData1[informationDataItem.GetFEE64ID()] << 16)
										| (correlationScalerData0[informationDataItem.GetFEE64ID()]);	

					correlationScalerOffset = (4 * correlationScaler) - informationDataItem.GetTimestamp();

					if(myEventBuilder.GetCorrelationScalerOffset()==0){
						//Set the correlation scaler offset in the event builder
						myEventBuilder.SetCorrelationScaler(correlationScalerOffset);

						std::cout << "Setting the correlation scaler offset to: " << correlationScalerOffset << std::endl;
						correlationScalerStatus = true;
					}
					else if (myEventBuilder.GetCorrelationScalerOffset() != correlationScalerOffset){
						std::cout << "Warning correlation scaler offset changed mid run. Old Offset = " << myEventBuilder.GetCorrelationScalerOffset()
								  << " - New offset = " << correlationScalerOffset << ". Was a sync pulse sent in the middle of a run?" <<std::endl;
					}

					#ifdef DEB_CORRELATION
						std::cout << "Correlation scaler " << correlationScaler << " Offset = " << correlationScalerOffset << std::endl;
					#endif
				}
			} 

		}

	}
}
