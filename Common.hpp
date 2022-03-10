#ifndef _COMMON_HPP
#define _COMMON_HPP

namespace Common{
	//Variables that will be used throughout the program by all threads

	const int noFEE64 = 5;			//Not just the number in use but the highest number ID that is used
	const int noDSSD = 1;			//Total number of DSSDs
	const int noChannel = 64;		//Will remain fixed. Number of channels per FEE
	const int masterFEE64 = 1;

	//Pointer used by ROOT to access output file
	TFile * fOutRoot;
}
#endif