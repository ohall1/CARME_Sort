#ifndef _DATAITEMS_HPP
#define _DATAITEMS_HPP

#include "Common.hpp"

class ADCDataItem{

	private:
		unsigned long timestamp;    //Internal AIDA time stamp
		unsigned long timestampLSB; // Least significant bits of timestamp

		unsigned int adcData;       //ADC value
		unsigned int sampleLength;

		unsigned int dataType;     // Data type: 0 = Sample waveform, 1 = Sample Length, 2 = Info Data, 3 = ADC Data
		int fee64ID;      //FEE64 ID
		unsigned int channelID;	// Channel ID
		unsigned int adcRange;		// ADC range: 0 = low energy, 1 = high energy

	public:
		ADCDataItem();
		~ADCDataItem(){};
		ADCDataItem(std::pair < unsigned int, unsigned int> inData);

		void BuildItem(std::pair < unsigned int, unsigned int> inData);

		void BuildTimestamp(unsigned long MSB);

		void SetTimestamp(unsigned long newTimestamp);
		void SetADCRange(short range);


		unsigned long GetTimestamp();
		int GetFEE64ID();
		unsigned int GetChannelID();
		unsigned int GetADCRange();
		unsigned int GetADCData();
};

class InformationDataItem{

	private:

		unsigned long timestamp;    		//Internal AIDA time stamp
		unsigned long timestampLSB; 		// Least significant bits of timestamp
		unsigned long timestampMSB;			// Most significant bits of timestamp
		unsigned long infoField;    		//Iformation field of AIDA data word

		unsigned int sampleLength;

		unsigned int dataType;     		// Data type: 0 = Sample waveform, 1 = Sample Length, 2 = Info Data, 3 = ADC Data
		unsigned int fee64ID;      		//FEE64 ID
		unsigned int infoCode;				//Info code of AIDA "information" word
		unsigned char corrScalerIndex;		//Index of correlation scaler
		unsigned long corrScalerTimestamp;	//Timestamp of correlation scaler

	public:
		InformationDataItem();
		~InformationDataItem(){};
		InformationDataItem(std::pair < unsigned int, unsigned int> inData);
		void BuildItem(std::pair < unsigned int, unsigned int> inData);
		unsigned int GetInfoCode();
		unsigned long GetTimestampMSB();
		unsigned int GetCorrScalerIndex();
		unsigned long GetCorrScalerTimestamp();
		unsigned int GetFEE64ID();
		unsigned long GetTimestamp();
		unsigned long GetTimestampLSB();

		void SetTimestamp(unsigned long MSB);
};

class CalibratedADCDataItem{

	private:
		short dssd;
		short side;
		short strip;
		short adcRange;
		double energy;
		unsigned long timestamp;

	public:

		CalibratedADCDataItem();
		~CalibratedADCDataItem(){};
		CalibratedADCDataItem(ADCDataItem &adcDataItem);

		void BuildItem(ADCDataItem &adcDataItem);
		void SetDSSD(short dssdIn);
		void SetSide(short sideIn);
		void SetStrip(short stripIn);
		void SetADCRange(short adcRangeIn);
		void SetEnergy(double energyIn);
		void SetTimestamp(unsigned long timestampIn);

		short GetDSSD() const;
		short GetSide() const;
		short GetStrip() const;
		short GetADCRange() const;
		double GetEnergy() const;
		unsigned long GetTimestamp() const;

		//Comparrison operators
		bool operator<( const CalibratedADCDataItem &dataItem) const;
};

class Cluster{

	private:

		short dssd;
		short side;
		short stripMin;
		short stripMax;
		short adcRange;
		double Energy;
		unsigned long timestampMin;
		unsigned long timestampMax;
		short clusterMultiplicity;
		short eventMultiplicity;

	public:

		Cluster();
		Cluster(CalibratedADCDataItem & dataItem);
		~Cluster(){};

		void AddEventToCluster(CalibratedADCDataItem dataItem);
		void ResetCluster();
		short GetDSSD() const;
		short GetSide() const;
		short GetStrip() const;
		short GetStripMin() const;
		short GetADCRange() const;
		double GetEnergy() const;
		short GetSize() const;
		short GetMultiplicity() const;
		unsigned long GetTimestampMin() const;
		unsigned long GetTimestampMax() const;
		unsigned long GetTimestampDifference(unsigned long timestampIn) const;

		void SetMultiplicity(short multiplicity);

};
class MergerOutputOld{
	//Pretty much a struct but calling it a class as it has functions associated with it.
	//All variables are unfortunately public
	public:

		ULong_t T;
		ULong_t Tfast;
  		Double_t E;
  		Double_t Ex;
 		Double_t Ey;
 		Double_t x;
 		Double_t y;
		Double_t z;
 		Int_t nx;
 		Int_t ny;
  		Int_t nz;   //Pointless variable required by BRIKEN merger
		UChar_t ID;

		MergerOutputOld(){};
		MergerOutputOld(Cluster & clusterX, Cluster & clusterY);

		ULong_t GetTimestamp() const;

};
class MergerOutputNewTrial{
	//Pretty much a struct but calling it a class as it has functions associated with it.
	//All variables are unfortunately public
	public:

		ULong_t T;
		ULong_t Tfast;
  		Double_t E;
  		Double_t Ex;
 		Double_t Ey;
 		Int_t xMin;
 		Int_t yMin;
 		Int_t xMax;
 		Int_t yMax;

		Double_t z;
 		Int_t nx;
 		Int_t ny;
  		Int_t nz;   //Pointless variable required by BRIKEN merger
		UChar_t ID;

		MergerOutputNewTrial(){};
		MergerOutputNewTrial(Cluster & clusterX, Cluster & clusterY);

		ULong_t GetTimestamp() const;

};

class MergerOutput{
	//Based on the old version of the AIDA tree
	//Key change is Tfast is used to hold dx and dy
	//x and y are set to be the geometric centre of the decay or implant

		//Pretty much a struct but calling it a class as it has functions associated with it.
	//All variables are unfortunately public
	public:

		ULong_t T;
		ULong_t Tfast;
  		Double_t E;
  		Double_t Ex;
 		Double_t Ey;
 		Double_t x;
 		Double_t y;
		Double_t z;
 		Int_t nx;
 		Int_t ny;
  		Int_t nz;   //Pointless variable required by BRIKEN merger
		UChar_t ID;

		MergerOutput(){};
		MergerOutput(Cluster & clusterX, Cluster & clusterY);

		ULong_t GetTimestamp() const;
		double GetX() const;
		double GetY() const;
};
#endif