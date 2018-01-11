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

		void BuildTimestamp(unsigned long MSB);

		void SetTimestamp(unsigned long newTimestamp);


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
		short GetADCRange() const;
		double GetEnergy() const;
		unsigned long GetTimestampMin() const;
		unsigned long GetTimestampMax() const;
		unsigned long GetTimestampDifference(unsigned long timestampIn) const;

};
#endif