#pragma once
#include "stdafx.h"
#include "FileManager.h"
#include "Record.h"

class DataManager
{
	FileManager fileManager;

	vector<Record**> memoryBuffers;

	// Default number of printed records
	const int defaultPrintedRecordsCount = 1000;

	// Default buffers parameters
	const int defaultBufferSize = DISK_PAGE_SIZE;
	const int defaultBuffersCount = 100;

	// Fields describing memoryBuffers
	int memoryBufferSize; // in bytes
	int memoryBuffersCount;
	int recordsPerBuffer;

	// Fields storing data for stats
	int recordsNumber;
	int diskOperations;

	//// Managing memory buffers
	// Generate memory buffers based on current parameters
	void generateMemoryBuffers(bool printDetails);
	// Deallocate memory buffers 
	void deallocateMemoryBuffers(bool printDetails);


	//// Loading memory buffers
	// Load given amount of records to memory and parse them to memoryBuffers as Records objects (returns number of loaded elements)
	bool fillMemoryBuffers(string filename, int& recordsNumber);
	// Load nth run to nth buffer
	bool loadRun(int runIndex, int bufferIndex);


	//// Saving memory buffers
	// Save memory buffers to file
	int saveMemoryBuffers(string targetName, bool append);
	// Save single buffer to file
	void saveMemoryBuffer(int index, string targetName, bool append);


	//// Algorithm to sort data stored in buffers
	// Quicksort
	void memorySort(int low, int high);
	// Quicksort partition
	int memorySortPartition(int low, int high);
	// Swap elements of two given indexes
	void memorySwap(int index1, int index2);


	//// Merging
	// Merge runs from range [minRun, maxRun]
	string merge(int minRun, int maxRun);
	// Check if buffer was finished
	bool ifBufferFinished(vector<int>& buffersIterators, int n);

	// Map continuous space of indexes to buffers
	Record** getRecordByIndex(int index);

	// Get value of record on given index
	double valueOf(int index);

	// Generate targetfile name
	string getTargetfileName(int n);

	// rename given file to newname
	void renameFile(string oldname, string newname);

	// delete given file
	void deleteFile(string filename);
public:
	DataManager();

	// Perform Sorting operation on current file and parameters
	void sort(bool printDetails, bool printFiles);

	// Randomize records
	void recordsRandomize(int number);

	// Input records
	void recordsInput(int number);

	// Read records
	int recordsRead(string filename, int number);

	// Set memoryBuffers parameters
	void setMemoryBuffersCount(int memoryBuffersCount);
	// Get memoryBuffers parameters
	int getMemoryBufferSize();
	int getMemoryBuffersCount();

	// Get number of stored records in file
	int getRecordsNumber();

	// Print first n records from file
	int print(string filename, int recordsNumber);

	// Clear all stored variables and data
	void reset();
};
