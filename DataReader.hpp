#ifndef _DataReader_HPP
#define _DataReader_HPP

#include <fstream>
#include <list>

class DataReader{
	private:

		//Constants that define the format of the data that will be read in
		static const int HEADER_SIZE = 24; // Number of bytes in the header block
		static const int BLOCK_SIZE = 0x10000; //Max block size is 64kb
		static const int MAIN_SIZE = BLOCK_SIZE - HEADER_SIZE; // Header is included in the block size

		//Stream for reading in data file
		std::ifstream inputFile;
		std::list <std::string> AIDAFileList;

		//Paramerters describing size of data file
		int fileSize = 0;
		int numBlocks = 0;
		int endLocation = 0;
		int startLocation = 0;
		int dataLength = 0;

		unsigned int word0;
		unsigned int word1;

		std::string currentInputFile;

		//Char arrays that read in the block data
		char blockHeader[HEADER_SIZE];
		char blockData[MAIN_SIZE];

		void OpenInputFile();
		void BeginReader();
		void CalculateFileSize();
		void ReadBlock();
		void CloseInputFile();
		void SetInputFileList(std::list <std::string> fileList);

	public:

		//Constructor and destructor for class
		DataReader();
		~DataReader(){};

		void InitialiseReader(std::list <std::string> inputFileList);

};
#endif