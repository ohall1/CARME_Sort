#include "EventClustering.hpp"

void EventClustering::AddEventToMap(CalibratedADCDataItem &dataItem){

	if(dataItem.GetADCRange == 0){
		decayMap.emplace(dataItem);
	}
	else if(decayItem.GetADCRange == 1){
		implantMap.emplace(dataItem);
	}
}
void EventClustering::ProcessMaps(){

}