#ifndef _DataReader_HPP
#define _DataReader_HPP
#include "Common.hpp"

#include <fstream>
#include <list>
#include <utility>
#include <thread>
#include <mutex>
#include <thread>
#include <condition_variable>

class DataReader{
	private:

		//Constants that define the format of the data that will be read in
		static const int HEADER_SIZE = 24; // Number of bytes in the header block
		static const int BLOCK_SIZE = 0x10000; //Max block size is 64kb
		static const int MAIN_SIZE = BLOCK_SIZE - HEADER_SIZE; // Header is included in the block size

		//Stream for reading in data file
		std::ifstream inputFile;
		std::list <std::string> AIDAFileList; //List of the files to be sorted

		//Variables used to output the data words
		std::list <std::pair<unsigned int, unsigned int>> dataWordList;					//List to store sorted data blocks
		std::list <std::list <std::pair<unsigned int, unsigned int>>> dataWordBuffer;	//List to be used as a buffer between reader and unpacker
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

		void OpenInputFile();

		void CalculateFileSize();
		void ReadBlock();
		void CloseInputFile();
		void SetInputFileList(std::list <std::string> fileList);
		void AddToBuffer(std::list<std::pair<unsigned int, unsigned int>> dataIn);

	public:

		//Constructor and destructor for class
		DataReader();
		~DataReader(){};

		void InitialiseReader(std::list <std::string> inputFileList);
		void BeginReader();
		std::list<std::pair<unsigned int, unsigned int>> ReadFromBuffer();

};
#endif