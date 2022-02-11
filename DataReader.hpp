#ifndef _DataReader_HPP
#define _DataReader_HPP
#include "Common.hpp"

#include <fstream>
#include <list>
#include <deque>
#include <utility>
#include <thread>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "TSystem.h"

extern int dataSpyOpen   (int id);
extern int dataSpyClose  (int id);
extern int dataSpyRead   (int id, char *data, int length);

class DataReader{
	private:

		//Constants that define the format of the data that will be read in
		static const int HEADER_SIZE = 24; // Number of bytes in the header block
		static const int BLOCK_SIZE = 0x10000; //Max block size is 64kb
		static const int MAIN_SIZE = BLOCK_SIZE - HEADER_SIZE; // Header is included in the block size

        // Properties for dataspy
        int id;                                               // ID of the dataSpy stream
        bool isDataSpy;                                       // Bool for whether the source of the data will be from a dataspy source
        int dataSpyI;                                         // Stores value returned by dataspyopen/close
        int dataSpyLength;                                    // Stores value returned by dataspyread

        long currentBlockID;
        long lastBlockID;
        long blocksSeen;

		//Stream for reading in data file
		std::ifstream inputFile;
		std::list <std::string> AIDAFileList; //List of the files to be sorted

		//Variables used to output the data words
		std::deque <std::pair<unsigned int, unsigned int>> dataWordList;					//List to store sorted data blocks
		std::queue <std::deque <std::pair<unsigned int, unsigned int>>> dataWordBuffer;	//List to be used as a buffer between reader and unpacker
		std::pair <unsigned int, unsigned int> dataWords;

		//Variables used for synchronising data threads
		std::mutex bufProtect;

		std::condition_variable bufferFull;
		std::condition_variable bufferEmpty;

		bool bufferFullCheck;
		bool bufferEmptyCheck;
		bool dataFinishedCheck = false;

		//Paramerters describing size of data file
		long int fileSize = 0;
		long int numBlocks = 0;
		long int endLocation = 0;
		long int startLocation = 0;
		long int dataLength = 0;

		unsigned int word0;
		unsigned int word1;

		std::string currentInputFile;

		//Char arrays that read in the block data
		char blockHeader[HEADER_SIZE];
		char blockData[MAIN_SIZE];
        char dataSpyData[BLOCK_SIZE];

		void OpenInputFile();

		void CalculateFileSize();
		int ReadBlock();
		void CloseInputFile();
		void SetInputFileList(std::list <std::string> fileList);
		void AddToBuffer(std::deque<std::pair<unsigned int, unsigned int>> dataIn);

	public:

		//Constructor and destructor for class
		DataReader();
		~DataReader(){};

		void InitialiseReader(std::list <std::string> inputFileList, bool bDataSpy);
		void BeginReader();
        void UpdateDataSpy(bool value);
		std::deque<std::pair<unsigned int, unsigned int>> ReadFromBuffer();

};
#endif