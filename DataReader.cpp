#include "DataReader.hpp"
#include "dataspy/dataspy.c"

DataReader::DataReader(){};

void DataReader::InitialiseReader(std::list <std::string> inputFileList, bool bDataSpy){


	//Check whether input file list is filled
    if (!bDataSpy) {
        if (inputFileList.size() <= 0) {
            std::cout << "No input files given, program ending" << std::endl;
            dataFinishedCheck = true;
            return;
        } else {
            std::cout << "AIDASort sorting offline data" << std::endl;
            SetInputFileList(inputFileList);
        }
        isDataSpy = false;
    }
    else{
        isDataSpy = true;
        id = 0;
        dataSpyI = dataSpyOpen(id);
    }
    currentBlockID = -1;
    blocksSeen = 0;
    lastBlockID = -1;
}

void DataReader::SetInputFileList(std::list <std::string> fileList){
	AIDAFileList = fileList;
	return;
}

void DataReader::BeginReader(){
	while(!AIDAFileList.empty() && !isDataSpy){//Loops over all AIDA files
		
		OpenInputFile();//Opens the first AIDA file in the list
		CalculateFileSize();

		if (numBlocks > 0){//If file contains data blocks
			for (int currentBlock = 0; currentBlock < numBlocks; currentBlock++){//Loop over data blocks

				dataWordList.clear();

				ReadBlock();

				for(int itrData = 0; itrData < dataLength; itrData += 8){//Loop over data words in file

					//Assemble data words from block data
					//All data comes split between two words
					word0 = (blockData[itrData] & 0xFF) | (blockData[itrData+1] & 0xFF) << 8 |
							(blockData[itrData+2] & 0xFF) << 16 | (blockData[itrData+3] & 0xFF) << 24;

					word1 = (blockData[itrData+4] & 0xFF) | (blockData[itrData+5] & 0xFF) << 8 |
							(blockData[itrData+6] & 0xFF) << 16 | (blockData[itrData+7] & 0xFF) << 24;

					#ifdef DEB
						if(itrData < 17){
							std::cout << "itrData - " << itrData << " Word0 - " << word0 << std::endl;
							std::cout << "Word1 - " << word1 << std::endl;
							std::cout << 1* (blockData[itrData] & 0xFF) << " block 1" <<std::endl;
							std::cout << 1* (blockData[itrData+1] & 0xFF) << " block 2" <<std::endl;
							std::cout << 1* (blockData[itrData+2] & 0xFF) << " block 3" <<std::endl;
							std::cout << 1* (blockData[itrData+3] & 0xFF) << " block 4" <<std::endl;
						}
					#endif

					//If one of the words == 0xFFFFFFFF we reject all the data
					if (word0 == 0xFFFFFFFF || word1 == 0xFFFFFFFF){
						#ifdef DEB
							//std::cout << "Reached end of data block. Data ended with word 0xFFFFFFFF" << std::endl;
						#endif

						continue;
					}
					else{
						//Pass on data words to the buffer
						dataWords.first = word0;
						dataWords.second = word1;

						dataWordList.push_back(dataWords);

						
					}


				}//End loop over data words

				AddToBuffer(dataWordList);
				
			}//End loop over data blocks
		}//End if on if data file has block

		//Should have reached end of file. Close input file.
		CloseInputFile();

	}
    if(isDataSpy){
        while(isDataSpy){
            //Infinite loop for now
            dataSpyLength = ReadBlock();
            if (dataSpyLength == 0){
                //No data to read. Sleep for 1ms
				// Print sleeping
				std::cout << "Sleeping" << std::endl;
                usleep(1000);
                continue;
            }
			std::cout << "Have data: " << dataSpyLength << std::endl;
            dataWordList.clear();
            for(int itrData = 24; itrData < dataLength+24; itrData += 8){
                word0 = (dataSpyData[itrData] & 0xFF) | (dataSpyData[itrData+1] & 0xFF) << 8 |
                        (dataSpyData[itrData+2] & 0xFF) << 16 | (dataSpyData[itrData+3] & 0xFF) << 24;

                word1 = (dataSpyData[itrData+4] & 0xFF) | (dataSpyData[itrData+5] & 0xFF) << 8 |
                        (dataSpyData[itrData+6] & 0xFF) << 16 | (dataSpyData[itrData+7] & 0xFF) << 24;
#ifdef DEB
                if(itrData < 17+24){
                std::cout << "itrData - " << itrData << " Word0 - " << word0 << std::endl;
                std::cout << "Word1 - " << word1 << std::endl;
                std::cout << 1* (dataSpyData[itrData] & 0xFF) << " block 1" <<std::endl;
                std::cout << 1* (dataSpyData[itrData+1] & 0xFF) << " block 2" <<std::endl;
                std::cout << 1* (dataSpyData[itrData+2] & 0xFF) << " block 3" <<std::endl;
                std::cout << 1* (dataSpyData[itrData+3] & 0xFF) << " block 4" <<std::endl;
                }
#endif
                if (word0 == 0xFFFFFFFF || word1 == 0xFFFFFFFF) continue;
                //Pass on data words to the buffer
                dataWords.first = word0;
                dataWords.second = word1;

                dataWordList.push_back(dataWords);

            }
            AddToBuffer(dataWordList);

        }
    }

	//All files should have been emptied and closed
	std::cout << "All data files read in." <<std::endl;
	dataFinishedCheck = true;

	dataWords.first = 0x0000;
	dataWords.second = 0x0000;
	dataWordList.clear();
	dataWordList.push_back(dataWords);
	AddToBuffer(dataWordList);

	if(bufferEmptyCheck){
		bufferEmpty.notify_all();
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

int DataReader::ReadBlock(){
    if(!isDataSpy) {
        inputFile.read((char *) &blockHeader, sizeof(blockHeader));
        inputFile.read((char *) &blockData, sizeof(blockData));

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

        return dataLength;
    }
    else{
        dataSpyLength = dataSpyRead(id, (char*)dataSpyData, sizeof(dataSpyData));
        currentBlockID = (dataSpyData[8] & 0xFF) << 24 | (dataSpyData[9] & 0xFF) << 16 |
                         (dataSpyData[10] & 0xFF) << 8 | (dataSpyData[11] & 0xFF);
        if(currentBlockID > lastBlockID){
            blocksSeen++;
            lastBlockID = currentBlockID;
            if(currentBlockID > 0 && currentBlockID % 10 == 0){
                //Can print valid fraction of that 2GB chunk
                std::cout << "Fraction of current 2GB chunk observed: " <<
                          (double)blocksSeen/(double)currentBlockID << std::endl;

            }
        }
        else if(currentBlockID == lastBlockID){
            return 0;
        }
        else if(currentBlockID < lastBlockID){
            lastBlockID = currentBlockID;
            blocksSeen = 1;
            std::cout << "New 2GB chunk of data being processed" << std::endl;
        }
        return dataSpyLength;
    }
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

void DataReader::AddToBuffer(std::deque<std::pair<unsigned int, unsigned int>> dataIn){

	//Aqquire bufProtect mutex lock to modify list
	std::unique_lock<std::mutex> addLock(bufProtect);
	#ifdef DEB_THREAD
		std::cout << "Mutex lock acquire for AddToBuffer" << std::endl;
	#endif

	//Wait if list size exceeds maximum buffer
	while(dataWordBuffer.size()>=100){
		bufferFullCheck = true;
		#ifdef DEB_THREAD
			std::cout << "Buffer list is fulle. BufferFulCheck = " << bufferFullCheck <<std::endl;
		#endif
		bufferFull.wait(addLock);
	}

	dataWordBuffer.push(dataIn);
	#ifdef DEB_THREAD
		std::cout << "Data pair added to back of buffer." << std::endl;
	#endif
	//	std::cout << "Item added buffer size " << dataWordBuffer.size() <<std::endl;


	#ifdef DEB_THREAD
		std::cout << "Checking bufferEmptyCheck = " << bufferEmptyCheck << std::endl;
	#endif

	if(bufferEmptyCheck){
		bufferEmptyCheck = false;
		#ifdef DEB_THREAD
			std::cout << "Buffer no longer empty. Notifying ReadFromBufferThread" << std::endl;
		#endif
		bufferEmpty.notify_all();
	}

	//Unlock the bufProtect mutex lock
	addLock.unlock();
	#ifdef DEB_THREAD
		std::cout << "Mutext lock on AddToBuffer released" <<std::endl;
		std::cout << " " << std::endl;
	#endif

	return;
}

std::deque<std::pair<unsigned int, unsigned int>> DataReader::ReadFromBuffer(){

	std::deque <std::pair<unsigned int, unsigned int>> bufferOut;
	bufferOut.clear();

	//Aqquire bufProtect mutex lock to modify list
	std::unique_lock<std::mutex> popLock(bufProtect);
	#ifdef DEB_THREAD
		std::cout << "Mutex lock acquired for ReadFromBuffer" <<std::endl;
	#endif

	//Check if the list size is empty
	while(dataWordBuffer.size()<1){
		bufferEmptyCheck = true;
		#ifdef DEB_THREAD
			std::cout << "Buffer list is empty, thread waiting. bufferEmptyCheck = " << bufferEmptyCheck << std::endl;
		#endif

		//	std::cout << "Waiting" << dataWordBuffer.size() <<std::endl;

		bufferEmpty.wait(popLock);
	}

	bufferOut = dataWordBuffer.front();
	dataWordBuffer.pop();
	//std::cout << "Data read buffer size. " << dataWordBuffer.size() <<std::endl;

	#ifdef DEB_THREAD
		//std::cout << "Buffer popped from front. First word = " << bufferOut.first << " Second word = " << bufferOut.second << std::endl;
		std::cout << "Current list buffer size " << dataWordBuffer.size() << " items." << std::endl;
	#endif


	#ifdef DEB_THREAD
		std::cout << "Checking bufferFullCheck = " << bufferFullCheck << std::endl;
	#endif

	if(bufferFullCheck){
		bufferFullCheck=false;
		#ifdef DEB_THREAD
			std::cout << "Buffer no longer full. Notifying AddToBufferThread" << std::endl;
		#endif
		bufferFull.notify_all();
	}

	popLock.unlock();

	#ifdef DEB_THREAD
		std::cout << "Releasing mutex lock on ReadFromBuffer" << std::endl;
		std::cout << " " <<std::endl;
	#endif

	return bufferOut;

}

void DataReader::UpdateDataSpy(bool value) {
    isDataSpy = value;
    //There shouldn't be an issue as isDataSpy is only read after it is first initialised
}