#include "FileManager.h"

FileManager::FileManager()
{
	// Create main directory
	if (!(CreateDirectory(mainDirectory.c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError()))
	{
		Communicator::output_error("Error creating main directory");
	}
}


long FileManager::getFileLoadPosition(string filename)
{
	if (filenameToLoadPosition.count(filename) == 1)
	{
		// Filename was found, so return position from corresponding index
		return filenameToLoadPosition.at(filename);
	}
	else
	{
		// Filename was not found, so return beginning of the file
		return 0;
	}
}


void FileManager::insertFileLoadPosition(string filename, long pos)
{
	filenameToLoadPosition[filename] = pos;
}


string FileManager::getMainFilename()
{
	return mainFilename;
}


int FileManager::saveDoublesBuffer(FILE *file, double* buffer, int size)
{
	return fwrite(buffer, sizeof(double), size, file);
}


int FileManager::loadDoublesBuffer(FILE* file, double* buffer, int size)
{
	return fread(buffer, sizeof(double), size, file);
}


void FileManager::saveBuffers(string filename, vector<double*>& buffers, int size)
{
	// Open append binary stream to file
	FILE *file = fopen(filename.c_str(), "ab");

	for (auto &buffer : buffers)
	{
		if (saveDoublesBuffer(file, buffer, size) != size)
		{
			Communicator::output_warn("Not all values were saved to file");
		}

		delete[] buffer;
	}


	fclose(file);
}


bool FileManager::continuousLoadDoubles(string filename, double*& buffer, int& doublesNumber)
{
	// Create buffer of doubles
	buffer = new double[doublesNumber];
	// Open output binary stream from file
	FILE* file = fopen(filename.c_str(), "rb");

	// Resume previous position
	long pos = getFileLoadPosition(filename);
	fseek(file, getFileLoadPosition(filename), SEEK_SET);

	// Load given amount of doubles and save how many of them were actually loaded
	doublesNumber = loadDoublesBuffer(file, buffer, doublesNumber);

	//// Save current file load position if file was not finished
	// Get current pos
	long currentPos = ftell(file);

	fseek(file, 0, SEEK_END);
	long endPos = ftell(file);

	if (endPos == currentPos)
	{
		// File finished
		insertFileLoadPosition(filename, 0);
		fclose(file);

		return true;
	}
	else
	{
		// File not finished
		// Save current pos
		insertFileLoadPosition(filename, currentPos);
		fclose(file);

		return false;
	}
}


int FileManager::staticLoadDoubles(string filename, double*& buffer, int doublesNumber)
{
	// Create buffer of doubles
	buffer = new double[doublesNumber];
	// Open output binary stream from file
	FILE* file = fopen(filename.c_str(), "rb");

	// Load given amount of doubles and save how many of them were actually loaded
	doublesNumber = loadDoublesBuffer(file, buffer, doublesNumber);

	fclose(file);

	return doublesNumber;
}


void FileManager::saveBuffer(string targetName, double* buffer, int size, bool append)
{
	// Open input stream to file
	FILE* file;

	if (append)
	{
		file = fopen(targetName.c_str(), "ab");
	}
	else
	{
		file = fopen(targetName.c_str(), "wb");
	}


	saveDoublesBuffer(file, buffer, size);

	delete[] buffer;

	fclose(file);
}


int FileManager::loadTxtDoubles(string targetName, double*& buffer, int number)
{
	ifstream file;
	file.open(targetName);

	buffer = new double[number];

	for (int i = 0; i < number; i++)
	{
		file >> buffer[i];

		// Check if file was finished
		if (file.peek() == EOF)
		{
			number = i;
			break;
		}
	}

	file.close();

	return number;
}


void FileManager::reset()
{
	// Reset containers used to store file position
	filenameToLoadPosition.clear();

	// Reset data in file mainFilename
	FILE* file = fopen(mainFilename.c_str(), "wb");
	fclose(file);
}
