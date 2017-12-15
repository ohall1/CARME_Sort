#include "DataItems.hpp"
ADCDataItem::ADCDataItem(){};
ADCDataItem::ADCDataItem(std::pair < unsigned int, unsigned int> inData){

	dataType = 3;
	unsigned int chIdentity = (inData.first >> 16) & 0xFFF; //Word 0, bits 16:27
	fee64ID = (chIdentity >> 6) & 0x003F;  					//Top 6 bit of chIdentity (22:27) are FEE64 number
	channelID = chIdentity & 0x003f;						//Bottom 6 bits of chIdentity (16:21) are channel number
	adcRange = (inData.first >> 28 ) & 0x0001;				//Bit 28 - Veto bit used as ADC range
	adcData = (inData.first & 0xFFFF); 						//Bits 0:15 of Word 0 is ADC data
	timestampLSB = (inData.second & 0x0FFFFFFF);				//Word 1, bits 0:27 - Timestamp LSB

}
void ADCDataItem::BuildTimestamp(unsigned long MSB){
	timestamp = (MSB << 28) | timestampLSB;
}
void ADCDataItem::SetTimestamp(unsigned long newTimestamp){
	timestamp = newTimestamp;
}
unsigned long ADCDataItem::GetTimestamp(){
	return timestamp;
}
int ADCDataItem::GetFEE64ID(){
	return fee64ID;
}
unsigned int ADCDataItem::GetADCRange(){
	return adcRange;
}
unsigned int ADCDataItem::GetChannelID(){
	return channelID;
}
unsigned int ADCDataItem::GetADCData(){
	return adcData;
}

InformationDataItem::InformationDataItem(std::pair < unsigned int, unsigned int> inData){

	dataType = 2;

	infoField = ( inData.first & 0x000FFFFF); 				//Word 0, bits 0:19 is the information field
	infoCode = (inData.first >> 20) & 0x000F;				//Word 0, bits 20:23 - Information code
	fee64ID = (inData.first >> 24) & 0x003F;				//Word 0, bits 24:29 - FEE64 module number
	timestampLSB = (inData.second & 0x0FFFFFFF);			//Word 1, bits 0:27 - Timestamp LSB

	if ( infoCode == 2 || infoCode == 3 || infoCode == 4){
		timestampMSB = infoField;
		timestamp = (timestampMSB << 28) | timestampLSB;
	}

	if ( infoCode == 8){

		corrScalerIndex = (infoField & 0x000F0000) >> 16;	//Index of correlation scaler
		corrScalerTimestamp = (infoField & 0x0000FFFF);		//Bits with timestamp

	}


}
void InformationDataItem::SetTimestamp(unsigned long MSB){
	timestamp = ((MSB <<28 ) | timestampLSB);
}
unsigned long InformationDataItem::GetTimestampMSB(){
	return timestampMSB;
}
unsigned int InformationDataItem::GetInfoCode(){
	return infoCode;
}

unsigned int InformationDataItem::GetCorrScalerIndex(){
	return corrScalerIndex;
}
unsigned long InformationDataItem::GetCorrScalerTimestamp(){
	return corrScalerTimestamp;
}
unsigned int InformationDataItem::GetFEE64ID(){
	return fee64ID;
}
unsigned long InformationDataItem::GetTimestamp(){
	return timestamp;
}
unsigned long InformationDataItem::GetTimestampLSB(){
	return timestampLSB;
}

CalibratedADCDataItem::CalibratedADCDataItem(){};
CalibratedADCDataItem::CalibratedADCDataItem(ADCDataItem &adcDataItem){
	energy = adcDataItem.GetADCData();
	timestamp = adcDataItem.GetTimestamp();
	#ifdef DEB_CALIBRATOR
		//std::cout << "\nadcDataItem " << adcDataItem.GetADCData() << " - Non-calibratedItem " << energy << std::endl;
		//std::cout << "\nadcDataItem " << adcDataItem.GetTimestamp() << " - Non-calibratedItem " << timestamp << std::endl;
		#endif
};

void CalibratedADCDataItem::SetDSSD(short dssdIn){
	dssd = dssdIn;
	return;
}
void CalibratedADCDataItem::SetSide(short sideIn){
	side = sideIn;
	return;
}
void CalibratedADCDataItem::SetStrip(short stripIn){
	dssd = stripIn;
	return;
}
void CalibratedADCDataItem::SetADCRange(short adcRangeIn){
	adcRange = adcRangeIn;
	return;
}
void CalibratedADCDataItem::SetEnergy(int EnergyIn){
	energy = EnergyIn;
	return;
}
void CalibratedADCDataItem::SetTimestamp(unsigned long timestampIn){
	timestamp = timestampIn;
	return;
}
short CalibratedADCDataItem::GetDSSD() const{
	return dssd;
}
short CalibratedADCDataItem::GetSide() const{
	return side;
}
short CalibratedADCDataItem::GetStrip() const{
	return strip;
}
short CalibratedADCDataItem::GetADCRange() const{
	return adcRange;
}
int CalibratedADCDataItem::GetEnergy() const{
	return energy;
}
unsigned long CalibratedADCDataItem::GetTimestamp() const{
	return timestamp;
}

bool CalibratedADCDataItem::operator<(const CalibratedADCDataItem &dataItem) const{
	return dssd < dataItem.GetDSSD() || (dssd == dataItem.GetDSSD() && side < dataItem.GetSide())
			|| (dssd == dataItem.GetDSSD() && side == dataItem.GetSide() && strip < dataItem.GetStrip());
}