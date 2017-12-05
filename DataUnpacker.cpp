#include "DataUnpacker.hpp"
#include "DataReader.hpp"

DataUnpacker::DataUnpacker(){};

UnpackedDataItem::UnpackedDataItem(std::pair < unsigned int, unsigned int> inData){

	dataType = 3;
	unsigned int chIdentity = (inData.first >> 16) & 0xFFF; //Word 0, bits 16:27
	fee64ID = (chIdentity >> 6) & 0x003F;  					//Top 6 bit of chIdentity (22:27) are FEE64 number
	channelID = chIdentity & 0x003f;						//Bottom 6 bits of chIdentity (16:21) are channel number
	adcRange = (inData.first >> 28 ) & 0x0001;				//Bit 28 - Veto bit used as ADC range
	adcData = inData.first & 0xFFFF; 						//Bits 0:15 of Word 0 is ADC data
	timeStampLSB = inData.second & 0x0FFFFFFF;				//Word 1, bits 0:27 - Timestamp LSB

}

InformationDataItem::InformationDataItem(std::pair < unsigned int, unsigned int> inData){

	dataType = 2;

	infoField = ( inData.first & 0x000FFFFF); 				//Word 0, bits 0:19 is the information field
	infoCode = (inData.first >> 20) & 0x000F;				//Word 0, bits 20:23 - Information code
	fee64ID = (inData.first >> 24) & 0x003F;				//Word 0, bits 24:29 - FEE64 module number
	timeStampLSB = (inData.second) & 0x0FFFFFFF;			//Word 1, bits 0:27 - Timestamp LSB

	if ( infoCode == 2 || infoCode == 3 || infoCode == 4){
		timeStampMSB = infoField;
	}

	if ( infoCode == 8){

		corrScalerIndex = (infoFiled & 0x000F0000) >> 16;	//Index of correlation scaler
		corrScalerTimeStamp = (infoField & 0x0000FFFF);		//Bits with timestamp
	}


}

InformationDataItem::GetTimeStampMSB(){
	return TimeStampMSB;
}
InformationDataItem::GetInfoCode(){
	return infoCode;
}

InformationDataItem::GetCorrScalerIndex(){
	return corrScalerIndex;
}
InformationDataItem::GetCorrScalerTimeStamp(){
	return corrScalerTimeStamp;
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

	dataType = ( (wordsIn.first >>30) & 0x3);

	if (dataType ==3 ){
		//ADC data item - Unpack into ADCDataItem format
		adcDataItem ADCDataItem(wordsIn);

		
	}
	if (dataType == 2){

		informationDataItem InformationDataItem(wordsIn);

		if(infomationDataItem.GetInfoCode == 2){
			timeStampMSB = informationDataItem.GetTimeStampMSB();
			timeStampMSBStatus == true;

			#ifdef DEB_UNPACKER
				std::cout << "Timestamp MSB Updated - " << TimeStampMSB << " PAUSE information item"
			#endif
		}
		else if(infomationDataItem.GetInfoCode == 3){
			timeStampMSB = informationDataItem.GetTimeStampMSB();
			timeStampMSBStatus == true;

			#ifdef DEB_UNPACKER
				std::cout << "Timestamp MSB Updated - " << TimeStampMSB << " RESUME information item"
			#endif
		}
		else if(infomationDataItem.GetInfoCode == 4){
			timeStampMSB = informationDataItem.GetTimeStampMSB();
			timeStampMSBStatus == true;

			#ifdef DEB_UNPACKER
				std::cout << "Timestamp MSB Updated - " << TimeStampMSB << " SYNC100 information item"
			#endif
		}
		else if(informationDataItem.GetInfoCode() == 8){

		}

	}
}
