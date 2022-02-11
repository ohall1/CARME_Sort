#include "DataItems.hpp"
ADCDataItem::ADCDataItem(){};
ADCDataItem::ADCDataItem(std::pair < unsigned int, unsigned int> inData){

	dataType = 3;
	unsigned int chIdentity = (inData.first >> 16) & 0xFFF; //Word 0, bits 16:27
	fee64ID = ((chIdentity >> 6) & 0x003F)+1;  					//Top 6 bit of chIdentity (22:27) are FEE64 number
	channelID = chIdentity & 0x003f;						//Bottom 6 bits of chIdentity (16:21) are channel number
	adcRange = (inData.first >> 28 ) & 0x0001;				//Bit 28 - Veto bit used as ADC range
	adcData = (inData.first & 0xFFFF); 						//Bits 0:15 of Word 0 is ADC data
	timestampWR24 = (inData.second & 0x0FFFFFFF);				//Word 1, bits 0:27 - Timestamp LSB

}
void ADCDataItem::BuildItem(std::pair < unsigned int, unsigned int> inData){
	dataType = 3;
	unsigned int chIdentity = (inData.first >> 16) & 0xFFF; //Word 0, bits 16:27
	fee64ID = ((chIdentity >> 6) & 0x003F)+1;  					//Top 6 bit of chIdentity (22:27) are FEE64 number
	channelID = chIdentity & 0x003f;						//Bottom 6 bits of chIdentity (16:21) are channel number
	adcRange = (inData.first >> 28 ) & 0x0001;				//Bit 28 - Veto bit used as ADC range
	adcData = (inData.first & 0xFFFF); 						//Bits 0:15 of Word 0 is ADC data
	timestampWR24 = (inData.second & 0x0FFFFFFF);				//Word 1, bits 0:27 - Timestamp LSB
}
void ADCDataItem::BuildTimestamp(unsigned long WR48, unsigned long WR64){
	if(timestampWR24 < 0x00000A0){
		timestamp = (WR64) << 48 | ((WR48+1) << 28) | timestampWR24;
	}
	else{
		timestamp = (WR64) << 48 | (WR48 << 28) | timestampWR24;
	}
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
void ADCDataItem::SetADCRange(short range){
	adcRange = range;
}

InformationDataItem::InformationDataItem(){};
InformationDataItem::InformationDataItem(std::pair < unsigned int, unsigned int> inData){

	dataType = 2;

	infoField = ( inData.first & 0x000FFFFF); 				//Word 0, bits 0:19 is the information field
	infoCode = (inData.first >> 20) & 0x000F;				//Word 0, bits 20:23 - Information code
	fee64ID = ((inData.first >> 24) & 0x003F)+1;			//Word 0, bits 24:29 - FEE64 module number
	timestampWR24 = (inData.second & 0x0FFFFFFF);			//Word 1, bits 0:27 - Timestamp LSB

	if ( infoCode == 2 || infoCode == 3 || infoCode == 4){
		timestampWRUpper = infoField;
	}
	if	(infoCode == 5){
		timestampWRUpper = (inData.first & 0x0000FFFF);
	}

	if ( infoCode == 8){

		corrScalerIndex = (infoField & 0x000F0000) >> 16;	//Index of correlation scaler
		corrScalerTimestamp = (infoField & 0x0000FFFF);		//Bits with timestamp

	}


}
void InformationDataItem::BuildItem(std::pair < unsigned int, unsigned int> inData){
	dataType = 2;

	infoField = ( inData.first & 0x000FFFFF); 				//Word 0, bits 0:19 is the information field
	infoCode = (inData.first >> 20) & 0x000F;				//Word 0, bits 20:23 - Information code
	fee64ID = ((inData.first >> 24) & 0x003F)+1;				//Word 0, bits 24:29 - FEE64 module number
	timestampWR24 = (inData.second & 0x0FFFFFFF);			//Word 1, bits 0:27 - Timestamp LSB

	if ( infoCode == 2 || infoCode == 3 || infoCode == 4){
		timestampWRUpper = infoField;
	}
	if	(infoCode == 5){
		timestampWRUpper = (inData.first & 0x0000FFFF);
	}

	if ( infoCode == 8){

		corrScalerIndex = (infoField & 0x000F0000) >> 16;	//Index of correlation scaler
		corrScalerTimestamp = (infoField & 0x0000FFFF);		//Bits with timestamp

	}
}
void InformationDataItem::SetTimestamp(unsigned long WR48, unsigned long WR64){
	if(timestampWR24 < 0x00000A0){
		timestamp = (WR64) << 48 | ((WR48+1) << 28) | timestampWR24;
	}
	else{
		timestamp = (WR64) << 48 | (WR48 << 28) | timestampWR24;
	}
}
unsigned long InformationDataItem::GetTimestampWRUpper(){
	return timestampWRUpper;
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
unsigned long InformationDataItem::GetTimestampWR24(){
	return timestampWR24;
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
void CalibratedADCDataItem::BuildItem(ADCDataItem &adcDataItem){
	energy = adcDataItem.GetADCData();
	timestamp = (adcDataItem.GetTimestamp());	//Convwerts the timestamp to nanoseconds
	#ifdef DEB_CALIBRATOR
		//std::cout << "\nadcDataItem " << adcDataItem.GetADCData() << " - Non-calibratedItem " << energy << std::endl;
		//std::cout << "\nadcDataItem " << adcDataItem.GetTimestamp() << " - Non-calibratedItem " << timestamp << std::endl;
		#endif	
}
void CalibratedADCDataItem::SetDSSD(short dssdIn){
	dssd = dssdIn;
	return;
}
void CalibratedADCDataItem::SetSide(short sideIn){
	side = sideIn;
	return;
}
void CalibratedADCDataItem::SetStrip(double stripIn){
	strip = stripIn;
	return;
}
void CalibratedADCDataItem::SetADCRange(short adcRangeIn){
	adcRange = adcRangeIn;
	return;
}
void CalibratedADCDataItem::SetEnergy(double EnergyIn){
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
double CalibratedADCDataItem::GetStrip() const{
	return strip;
}
short CalibratedADCDataItem::GetADCRange() const{
	return adcRange;
}
double CalibratedADCDataItem::GetEnergy() const{
	return energy;
}
unsigned long CalibratedADCDataItem::GetTimestamp() const{
	return timestamp;
}

bool CalibratedADCDataItem::operator<(const CalibratedADCDataItem &dataItem) const{
	return dssd < dataItem.GetDSSD() || (dssd == dataItem.GetDSSD() && side < dataItem.GetSide())
			|| (dssd == dataItem.GetDSSD() && side == dataItem.GetSide() && strip < dataItem.GetStrip());
}
Cluster::Cluster(){
	dssd = -5;
	side = -5;
	stripMin = -5;
	stripMax = -5;
	Energy = -5;
	timestampMin = 0;
	timestampMax = 0;
	clusterMultiplicity = -5;
	eventMultiplicity = -5;
};
Cluster::Cluster(CalibratedADCDataItem &dataItem){
	dssd = dataItem.GetDSSD();
	side = dataItem.GetSide();
	stripMin = dataItem.GetStrip();
	stripMax = stripMin;
	Energy = dataItem.GetEnergy();
	adcRange = dataItem.GetADCRange();
	timestampMin = dataItem.GetTimestamp();
	timestampMax = timestampMin;
	clusterMultiplicity = 1;
	eventMultiplicity = 0;
}
void Cluster::AddEventToCluster(CalibratedADCDataItem dataItem){

	unsigned long timestampIn = dataItem.GetTimestamp();
	short stripIn = dataItem.GetStrip();


	if(timestampMin == 0 || timestampMax == 0){
		timestampMin = timestampIn;
		timestampMax = timestampIn;
	}
	else if(timestampIn < timestampMin){
		timestampMin = timestampIn;
	}
	else if(timestampIn > timestampMax){
		timestampMax = timestampIn;
	}
	else if(timestampIn == timestampMin || timestampIn == timestampMax){}

	if(stripIn < stripMin){
		stripMin = stripIn;
	}
	else if (stripIn > stripMax && stripMax != -5){
		stripMax = stripIn;
	}
	else if (stripMin == -5 || stripMax == -5){
		stripMin = stripIn;
		stripMax = stripIn;
	}
	if(clusterMultiplicity == -5){

		dssd = dataItem.GetDSSD();
		side = dataItem.GetSide();
		Energy = 0;
		adcRange = dataItem.GetADCRange();
		clusterMultiplicity = 0;
	}
	Energy += dataItem.GetEnergy();
	clusterMultiplicity++;
}
void Cluster::ResetCluster(){
	dssd = -5;
	side = -5;
	stripMin = -5;
	stripMax = -5;
	Energy = -5;
	timestampMin = 0;
	timestampMax = 0;
	clusterMultiplicity = -5;
	eventMultiplicity = -5;
}
short Cluster::GetDSSD() const{
	return dssd;
}
short Cluster::GetSide() const{
	return side;
}
double Cluster::GetStrip() const{
	return stripMax;
}
double Cluster::GetStripMin() const{
	return stripMin;
}
short Cluster::GetADCRange() const{
	return adcRange;
}
double Cluster::GetEnergy() const{
	return Energy;
}
unsigned long Cluster::GetTimestampMin() const{
	return timestampMin;
}
unsigned long Cluster::GetTimestampMax() const{
	return timestampMax;
}
short Cluster::GetSize() const{
	return clusterMultiplicity;
}
short Cluster::GetMultiplicity() const{
	return eventMultiplicity;
}
unsigned long Cluster::GetTimestampDifference(unsigned long timestampIn) const{
	unsigned long timestampDifMin;
	unsigned long timestampDifMax;

	if(timestampIn <= timestampMin){
		timestampDifMin = timestampMin - timestampIn;
		timestampDifMax = timestampMax - timestampIn;
	}
	else if(timestampIn <= timestampMax){
		timestampDifMin = timestampIn - timestampMin;
		timestampDifMax = timestampMax - timestampIn;
	}
	else{
		timestampDifMax = timestampIn - timestampMax;
		timestampDifMin = timestampIn - timestampMin;
	}

	if(timestampDifMin < timestampDifMax){
		return timestampDifMin;
	}
	else if(timestampDifMax < timestampDifMin){
		return timestampDifMax;
	}
	else if( timestampDifMin == timestampDifMax && timestampMin != 0){
		return timestampDifMin;
	}
	else if( timestampMin == 0){
		return 0;
	} 
	else{
		return 0;
	}
}
void Cluster::SetMultiplicity(short multiplicity){
	eventMultiplicity = multiplicity;
}
MergerOutputOld::MergerOutputOld(Cluster & clusterX, Cluster & clusterY){

	if(clusterX.GetTimestampMin() < clusterY.GetTimestampMin()){
		T = clusterX.GetTimestampMin();
	}
	else{
		T = clusterY.GetTimestampMin();
	}

	Tfast = 0;

	Ex = clusterX.GetEnergy();
	Ey = clusterY.GetEnergy();

	E = (Ex + Ey)/2.0;

	x = clusterX.GetStripMin();
	y = clusterY.GetStripMin();
	z = clusterX.GetDSSD();

	nx = clusterX.GetMultiplicity();
	ny = clusterY.GetMultiplicity();
	nz = 0;

	if(clusterX.GetADCRange() == 0){
		ID = 5;
	}
	else if(clusterY.GetADCRange() == 1){
		ID = 4;
	}
	else{
		ID==4;
	}

}
ULong_t MergerOutputOld::GetTimestamp()const{
	return T;
}
MergerOutputNewTrial::MergerOutputNewTrial(Cluster & clusterX, Cluster & clusterY){

	if(clusterX.GetTimestampMin() < clusterY.GetTimestampMin()){
		T = clusterX.GetTimestampMin();
	}
	else{
		T = clusterY.GetTimestampMin();
	}

	Tfast = 0;

	Ex = clusterX.GetEnergy();
	Ey = clusterY.GetEnergy();

	E = (Ex + Ey)/2.0;

	xMin = clusterX.GetStripMin();
	yMin = clusterY.GetStripMin();

	xMax = clusterX.GetStrip();
	yMax = clusterY.GetStrip();
	z = clusterX.GetDSSD();

	nx = clusterX.GetMultiplicity();
	ny = clusterY.GetMultiplicity();
	nz = 0;

	if(clusterX.GetADCRange() == 0){
		ID = 5;
	}
	else if(clusterY.GetADCRange() == 1){
		ID = 4;
	}
	else{
		ID==4;
	}

}
ULong_t MergerOutputNewTrial::GetTimestamp()const{
	return T;
}
MergerOutput::MergerOutput(Cluster & clusterY, Cluster & clusterX){

	if(clusterX.GetTimestampMin() < clusterY.GetTimestampMin()){
		T = clusterX.GetTimestampMin();
	}
	else{
		T = clusterY.GetTimestampMin();
	}

	uint8_t dx = (clusterX.GetStrip()-clusterX.GetStripMin());
	uint8_t dy = (clusterY.GetStrip()-clusterY.GetStripMin());
	uint8_t dxCal;
	uint8_t dyCal;

	Tfast = dx + 0x100 * dy;

	Ex = clusterX.GetEnergy();
	Ey = clusterY.GetEnergy();

	E = (Ex + Ey)/2.0;

	x = ((clusterX.GetStripMin() + clusterX.GetStrip())/2.0);
	y = ((clusterY.GetStripMin() + clusterY.GetStrip())/2.0);
	z = clusterX.GetDSSD();

	nx = clusterX.GetMultiplicity();
	ny = clusterY.GetMultiplicity();
	nz = 0;

	if(clusterX.GetADCRange() == 0){
		ID = 5;
	}
	else if(clusterY.GetADCRange() == 1){
		ID = 4;
	}
	else{
		ID==4;
	}

}
ULong_t MergerOutput::GetTimestamp()const{
	return T;
}
double MergerOutput::GetX()const{
	return x;
}
double MergerOutput::GetY()const{
	return y;
}
