#pragma once
#include "stdafx.h"
#include "Communicator.h"

using namespace std;

class FileManager
{
	const string mainDirectory = "data";
	const string mainFilename = mainDirectory + "/data";

	map<string, long> filenameToLoadPosition;

	// Methods to manage storing file load positions
	long getFileLoadPosition(string filename);
	void insertFileLoadPosition(string filename, long pos);

	// Save buffer of doubles to given file
	int saveDoublesBuffer(FILE *file, double* buffer, int size);

	// Load buffer of doubles from given file
	int loadDoublesBuffer(FILE *file, double* buffer, int size);

public:
	FileManager();

	// Get filename of main file
	string getMainFilename();

	// Save multiple buffers to the file (deletes saved buffers)
	void saveBuffers(string filename, vector<double*>& buffers, int size);

	// Load given amount of doubles to buffer
	// * continuousLoadDoubles will automatically store file position,
	// so that next call with the same given filename will continue the load from previous position
	// * doublesNumber on entry is amount of doubles which will be loaded
	// on return it is number of loaded doubles
	// * returned value is false is file is not finished
	// and true if file was finished
	bool continuousLoadDoubles(string filename, double*& buffer, int& doublesNumber);

	// Load given amount of doubles to buffer
	// * staticLoadDoubles will always load doubles from the begging of the file
	// * doublesNumber on entry is amount of doubles which will be loaded
	// * returned value is number of loaded doubleNumbers
	int staticLoadDoubles(string filename, double*& buffer, int doublesNumber);

	// Save buffer to file of given directory (deletes saved buffer)
	void saveBuffer(string targetName, double* buffer, int size, bool append);

	// Read buffer of doubles from .txt file
	int loadTxtDoubles(string targetName, double*& buffer, int number);

	// Clear all stored data
	void reset();
};
