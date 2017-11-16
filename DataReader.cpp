#include "DataReader.hpp"

DataReader::DataReader(){};

void DataReader::InitialiseReader(std::list <std::string> inputFileList){

	//Check whether input file list is filled
	if (inputFileList.size() <= 0){
		std::cout << "No input files given, program ending" << std::endl;
		return;
	}
	else{
		std::cout << "AIDASort sorting offline data" << std::endl;
		SetInputFileList(inputFileList);
		BeginReader();
	}
}

void DataReader::SetInputFileList(std::list <std::string> fileList){
	AIDAFileList = fileList;
	return;
}

void DataReader::BeginReader(){
	while(AIDAFileList.size()>0){//Loops over all AIDA files
		
		OpenInputFile();//Opens the first AIDA file in the list
		CalculateFileSize();

		if (numBlocks > 0){//If file contains data blocks
			for (int currentBlock = 0; currentBlock <= numBlocks; currentBlock++){//Loop over data blocks

				ReadBlock();

				for(int itrData = 0; itrData < dataLength; itrData += 8){//Loop over data words in file

					//Assemble data words from block data
					//All data comes split between two words
					word0 = (blockData[itrData] & 0xFF) | (blockData[itrData+1] & 0xFF << 8) |
							(blockData[itrData+2] & 0xFF) << 16 | (blockData[itrData+3] & 0xFF << 24);

					word1 = (blockData[itrData+4] & 0xFF) | (blockData[itrData+5] & 0xFF << 8) |
							(blockData[itrData+6] & 0xFF) << 16 | (blockData[itrData+7] & 0xFF << 24);

					#ifdef DEB
						if(itrData < 17){
							std::cout << "itrData - " << itrData << " Word0 - " << word0 << std::endl;
							std::cout << "Word1 - " << word1 << std::endl;
						}
					#endif

					//If one of the words == 0xFFFFFFFF we reject all the data
					if (word0 == 0xFFFFFFFF || word1 == 0xFFFFFFFF){
						#ifdef DEB
							std::cout << "Reached end of data block. Data ended with word 0xFFFFFFFF" << std::endl;
						#endif

						continue;
					}
					else{
						//Pass on data words to be unpacked

					}

				}//End loop over data words
				
			}//End loop over data blocks
		}//End if on if data file has block

		//Should have reached end of file. Close input file.
		CloseInputFile();

	}
}

void DataReader::OpenInputFile(){
	currentInputFile = AIDAFileList.front(); 
	AIDAFileList.pop_front();
	inputFile.open(currentInputFile.data());
	dataLength = 0;
	endLocation = 0;
	startLocation = 0;
	fileSize = 0;
	numBlocks = 0;

	if(!inputFile.is_open()){
		std::cout << "Problem opening: " << currentInputFile << std::endl;
		return;
	}
	else if(inputFile.is_open()){
		std::cout << "Input file: " << currentInputFile << " opened" << std::endl;
		return;
	}
	else{
		std::cout << "Something strange happening in DataReader::OpenInputFile" << std::endl;
		return;
	}

}

void DataReader::CalculateFileSize(){
	inputFile.seekg(0, inputFile.end);
	endLocation = inputFile.tellg();
	inputFile.seekg(0, inputFile.beg);
	startLocation = inputFile.tellg();

	fileSize = endLocation - startLocation;
	numBlocks = fileSize/BLOCK_SIZE;

	if ((fileSize%BLOCK_SIZE) != 0){
		std::cout << "Warning in DataReader::CalculateFileSize: Input File is not an integer number of BLOCK_SIZE" <<std::endl;
	}
	if ( fileSize>0 && numBlocks >0){ return;}
	else {
		std::cout << "Problem calculating file size" << std::endl;
		return;
	}

}

void DataReader::ReadBlock(){

	inputFile.read((char*)&blockHeader,sizeof(blockHeader));
	inputFile.read((char*)&blockData,sizeof(blockData));

	dataLength = (blockHeader[20] & 0xFF) | (blockHeader[21] & 0xFF) << 8 |
				 (blockHeader[22] * 0xFF) << 16 | (blockHeader[23] << 24);

	#ifdef DEB
		unsigned char headerID[8];
		headerID[0] = blockHeader[0];
		headerID[1] = blockHeader[1];
		headerID[2] = blockHeader[2];
		headerID[3] = blockHeader[3];
		headerID[4] = blockHeader[4];
		headerID[5] = blockHeader[5];
		headerID[6] = blockHeader[6];
		headerID[7] = blockHeader[7];

		unsigned int headerSequence = (blockHeader[8] & 0xFF) << 24 | (blockHeader[9] & 0xFF) << 16 |
									  (blockHeader[10] & 0xFF) << 8 | (blockHeader[11] & 0xFF);

		unsigned short int headerMyEndian; // 2 byte. Should be 256 for correct endianess
		headerMyEndian = (blockHeader[16] & 0xFF) << 8 | (blockHeader[17] & 0xFF);

		unsigned short int headerDataEndian; //2 byte again
		headerDataEndian = (blockHeader[18] & 0xFF) << 8 | (blockHeader[19] & 0xFF);



		std::cout << "Header ID test: " << headerID <<std::endl;
		std::cout << "Header sequence: 0x" << std::hex << headerSequence << std::dec << std::endl;
		std::cout << "Header MyEndian - DataEndian Should be 256 - " << headerMyEndian << " - " << headerDataEndian << std::endl;

	#endif

	return;

}

void DataReader::CloseInputFile(){

	if(inputFile.is_open()){
		std::cout << "Closing input file. ";
		inputFile.close();
		if(!inputFile.is_open()){
			std::cout << "Input file closed" << std::endl;
			return;
		}
	}
	else{
		std::cout << "Something went wrong with closing file. No file open to be closed." << std::endl;
		return;
	}
}