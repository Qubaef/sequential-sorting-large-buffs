#include "DataManager.h"

DataManager::DataManager()
{
	reset();

	// Calculate how many records fits into one memory buffer and round its size to this number
	recordsPerBuffer = memoryBufferSize / Record::getSize();
}


void DataManager::generateMemoryBuffers(bool printDetails)
{
	// Generate memory buffers
	for (int i = 0; i < memoryBuffersCount; i++)
	{
		Record** newBuffer = new Record*[recordsPerBuffer];
		memoryBuffers.push_back(newBuffer);
	}

	if (printDetails)
	{
		Communicator::output_inform("Successfully allocated " + to_string(memoryBuffersCount) + " buffers of " + to_string(memoryBufferSize) + " Bytes each.");
	}
}


void DataManager::deallocateMemoryBuffers(bool printDetails)
{
	for (auto buffer : memoryBuffers)
	{
		delete[] buffer;
	}

	memoryBuffers.clear();

	if (printDetails)
	{
		Communicator::output_inform("Successfully deallocated " + to_string(memoryBuffersCount) + " buffers.");
	}
}


bool DataManager::fillMemoryBuffers(string filename, int& recordsNumber)
{
	int n = Record::getParamsNumber();
	double* buffer = nullptr;
	int doublesNumber = recordsNumber * n;

	// Load data
	bool finished = fileManager.continuousLoadDoubles(filename, buffer, doublesNumber);

	// Update recordsNumber value to amount of loaded records number
	recordsNumber = doublesNumber / n;

	// Parse data to Records
	for (int i = 0; i < recordsPerBuffer * memoryBuffersCount; i++)
	{
		if (i < recordsNumber)
		{
			// Parse new Record and insert it to memoryBuffer
			// TODO: fix Record constructor to work on buffer of doubles
			Record* newRec = new Record(buffer[i * n], buffer[i * n + 1], buffer[i * n + 2], buffer[i * n + 3], buffer[i * n + 4]);
			memoryBuffers[i / recordsPerBuffer][i % recordsPerBuffer] = newRec;
		}
		else
		{
			// If record is empty, set it to null
			memoryBuffers[i / recordsPerBuffer][i % recordsPerBuffer] = nullptr;
		}
	}

	// Deallocate buffer
	delete[] buffer;

	// Calculate disk operations
	diskOperations += (int)(recordsNumber / recordsPerBuffer) * recordsPerBuffer == recordsNumber ? recordsNumber / recordsPerBuffer : recordsNumber / recordsPerBuffer + 1;

	return finished;
}


bool DataManager::loadRun(int runIndex, int bufferIndex)
{
	int n = Record::getParamsNumber();
	double* buffer = nullptr;
	int doublesNumber = recordsPerBuffer * n;

	// Load data
	bool finished = fileManager.continuousLoadDoubles(getTargetfileName(runIndex), buffer, doublesNumber);

	// Parse data to Records
	for (int i = 0; i < recordsPerBuffer; i++)
	{
		if (i < doublesNumber / n)
		{
			// Parse new Record and insert it to memoryBuffer
			// TODO: fix Record constructor to work on buffer of doubles
			Record* newRec = new Record(buffer[i * n], buffer[i * n + 1], buffer[i * n + 2], buffer[i * n + 3], buffer[i * n + 4]);
			memoryBuffers[bufferIndex][i] = newRec;
		}
		else
		{
			// If record is empty, set it to null
			memoryBuffers[bufferIndex][i] = nullptr;
		}
	}

	// Deallocate buffer
	delete[] buffer;

	// Calculate disk operations
	diskOperations++;

	return finished;
}


int DataManager::saveMemoryBuffers(string targetName, bool append)
{
	for (int i = 0; i < memoryBuffersCount; i++)
	{
		// If memory buffer is empty
		if (memoryBuffers[i][0] == nullptr)
		{
			return i;
		}

		if (append == false && i == 0)
		{
			saveMemoryBuffer(i, targetName, false);
		}
		else
		{
			saveMemoryBuffer(i, targetName, true);
		}
	}

	return memoryBuffersCount;
}


void DataManager::saveMemoryBuffer(int index, string targetName, bool append)
{
	int j;
	int size = Record::getParamsNumber();

	double* buffer = new double[recordsPerBuffer * size];

	// Build doubles buffer from buffer of records
	for (j = 0; j < recordsPerBuffer; j++)
	{
		if (memoryBuffers[index][j] != nullptr)
		{
			// Get buffer of record parameters as an dynamic array
			memoryBuffers[index][j]->appendParameters(&buffer[j * size]);

			// Delete the whole record from memory
			delete memoryBuffers[index][j];
		}
		else
		{
			// If next buffers are nullptr, exit
			break;
		}
	}

	// Save generated buffer to file
	fileManager.saveBuffer(targetName, buffer, j * Record::getParamsNumber(), append);

	// Calculate disk operations
	diskOperations++;
}


void DataManager::memorySort(int low, int high)
{
	if (low < high)
	{
		// Calculate partitioning index
		int pi = memorySortPartition(low, high);

		memorySort(low, pi - 1);
		memorySort(pi + 1, high);
	}
}


int DataManager::memorySortPartition(int low, int high)
{
	double pivot = valueOf(high);

	// Index of smaller element
	int i = low - 1;

	for (int j = low; j <= high - 1; j++)
	{
		// If current element is smaller than the pivot
		if (valueOf(j) < pivot)
		{
			i++;
			memorySwap(i, j);
		}
	}

	memorySwap(i + 1, high);
	return i + 1;
}


void DataManager::memorySwap(int index1, int index2)
{
	Record** record1 = getRecordByIndex(index1);
	Record** record2 = getRecordByIndex(index2);
	Record* tmp = *record2;

	*record2 = *record1;
	*record1 = tmp;
}


string DataManager::merge(int minRun, int maxRun)
{
	string filename = fileManager.getMainFilename() + "merge";

	// Calculate number of merged runs
	int numberOfRuns = maxRun - minRun + 1;
	if (numberOfRuns > memoryBuffersCount - 1)
	{
		Communicator::output_error("Error while loading buffers");
	}

	// Prepare initial state
	vector<int> buffersIterators;
	buffersIterators.reserve(numberOfRuns);
	vector<bool> runsStates;
	runsStates.reserve(numberOfRuns);

	for (int i = 0; i < numberOfRuns; i++)
	{
		// Load runs to buffers
		runsStates.push_back(loadRun(minRun + i, i));

		// Delete run file if loaded
		if (runsStates[i] == true)
		{
			deleteFile(getTargetfileName(minRun + i));
		}

		// Initialize vector of buffers iterators
		buffersIterators.push_back(0);
	}

	int outputBuffElements = 0;

	// Merging loop
	while (true)
	{
		// Loop through buffers to find min value
		double minVal = DBL_MAX;
		int minIndex = -1;

		// Loop to find min value and index
		for (int i = 0; i < numberOfRuns; i++)
		{
			if (ifBufferFinished(buffersIterators, i) == false)
			{
				if (minVal > memoryBuffers[i][buffersIterators[i]]->getValue())
				{
					minVal = memoryBuffers[i][buffersIterators[i]]->getValue();
					minIndex = i;
				}
			}
		}

		// Check if any value was found
		if (minIndex == -1)
		{
			// If not, merging should be finished
			break;
		}

		// Copy min value to nth buffer
		Record* recordToCopy = memoryBuffers[minIndex][buffersIterators[minIndex]];
		memoryBuffers[memoryBuffersCount - 1][outputBuffElements] = recordToCopy;

		outputBuffElements++;
		// If output buffer is full save it to the memory
		if (outputBuffElements == recordsPerBuffer)
		{
			saveMemoryBuffer(memoryBuffersCount - 1, filename, true);
			outputBuffElements = 0;
		}

		buffersIterators[minIndex]++;
		// If buffer is finished and run is not, load next part of run
		if (ifBufferFinished(buffersIterators, minIndex) && runsStates[minIndex] == false)
		{
			// Load next part of run
			runsStates[minIndex] = loadRun(minRun + minIndex, minIndex);

			// Delete run file if loaded
			if (runsStates[minIndex] == true)
			{
				deleteFile(getTargetfileName(minRun + minIndex));
			}

			buffersIterators[minIndex] = 0;
		}
	}

	// If output buffer contains remaining data, save it to the file
	if (outputBuffElements > 0)
	{
		// Fill remaining part of the buffer with nullptrs
		for (int i = outputBuffElements; i < recordsPerBuffer; i++)
		{
			memoryBuffers[memoryBuffersCount - 1][i] = nullptr;
		}

		// Save buffer
		saveMemoryBuffer(memoryBuffersCount - 1, filename, true);
	}

	// return filename of output disk file
	return filename;
}


bool DataManager::ifBufferFinished(vector<int>& buffersIterators, int n)
{
	return buffersIterators[n] == recordsPerBuffer || memoryBuffers[n][buffersIterators[n]] == nullptr;
}


Record** DataManager::getRecordByIndex(int index)
{
	return &memoryBuffers[index / recordsPerBuffer][index % recordsPerBuffer];
}


double DataManager::valueOf(int index)
{
	return (**getRecordByIndex(index)).getValue();
}


string DataManager::getTargetfileName(int n)
{
	return fileManager.getMainFilename() + to_string(n);
}


void DataManager::renameFile(string oldname, string newname)
{
	if (rename(oldname.c_str(), newname.c_str()) != 0)
	{
		Communicator::output_error("Error renaming file " + oldname);
	}
}


void DataManager::deleteFile(string filename)
{
	if (remove(filename.c_str()) != 0)
	{
		Communicator::output_error("Error deleting file " + filename);
	}
}


void DataManager::sort(bool printDetails, bool printFiles)
{
	// Generate memory buffers
	generateMemoryBuffers(printDetails);

	// Variable storing amount of runs
	int runs = 0;

	// Calculate amount of records which should be loaded to memory
	int recordsPerLoad = memoryBuffersCount * (int)(memoryBufferSize / Record::getSize());
	int recordsLoaded;
	bool finished = false;

	if (printDetails)
	{
		Communicator::output_inform("Starting memory sorting phase.");
	}

	if (printFiles)
	{
		Communicator::output_inform("Printing first " + to_string(defaultPrintedRecordsCount) + " records of " + fileManager.getMainFilename());
		print(fileManager.getMainFilename(), defaultPrintedRecordsCount);
	}

	//// Distribute main data file to runs
	do
	{
		// Load data to buffers
		recordsLoaded = recordsPerLoad;
		finished = fillMemoryBuffers(fileManager.getMainFilename(), recordsLoaded);

		// Sort data in buffers
		memorySort(0, recordsLoaded - 1);
		runs++;

		// Save run to disk
		int savedBuffers = saveMemoryBuffers(getTargetfileName(runs), false);

		// Print details
		if (printDetails)
		{
			Communicator::output_inform("Sorted " + to_string(savedBuffers) + " pages in buffers and saved them as " + getTargetfileName(runs));
		}

		// Print generated file
		if (printFiles)
		{
			Communicator::output_inform("Printing first " + to_string(defaultPrintedRecordsCount) + " records of " + getTargetfileName(runs));
			print(getTargetfileName(runs), defaultPrintedRecordsCount);
		}

		// Loop until main file is fully distributed
	} while (finished == false);

	if (printDetails)
	{
		Communicator::output_inform("Finished memory sorting. " + to_string(runs) + " runs created. Started merging phase.");
	}

	////// Merge runs
	//int minRun = 1;
	//int maxRun = runs;
	//int merges = 0;
	//string runFilename;

	//// Loop until one run remains
	//while (minRun != maxRun)
	//{
	//	// Count merges number for stats
	//	merges++;

	//	// Calculate current max run
	//	int currMaxRun = min(minRun + memoryBuffersCount - 2, maxRun);

	//	// Merge runs in range [minRun, currMaxRun] and place them in separate file
	//	runFilename = merge(minRun, currMaxRun);

	//	// Rename previously generated file to "data{currMaxRun}"
	//	renameFile(runFilename, getTargetfileName(currMaxRun));

	//	if (printDetails)
	//	{
	//		Communicator::output_inform("Finished merging runs " + to_string(minRun) + " to " + to_string(currMaxRun) + ".");
	//	}

	//	if (printFiles)
	//	{
	//		Communicator::output_inform("Printing first " + to_string(defaultPrintedRecordsCount) + " records of " + getTargetfileName(currMaxRun));
	//		print(getTargetfileName(currMaxRun), defaultPrintedRecordsCount);
	//	}

	//	// update minRun
	//	minRun = currMaxRun;
	//}

	//// Merge runs
	int minRun = 1;
	int maxRun = runs;
	int merges = 0;
	string runFilename;

	// Loop until one run remains
	while (minRun != maxRun)
	{
		int submerges = 0;

		// Calculate initial range of merge
		int currMinRun = minRun;
		int currMaxRun = min(currMinRun + memoryBuffersCount - 2, maxRun);

		// Perform all submerges on current level
		while (currMinRun < maxRun)
		{
			// Count merges
			submerges++;

			// Merge runs in range [currMinRun, currMaxRun] and place them in separate file
			runFilename = merge(currMinRun, currMaxRun);

			// Rename previously generated file to "data{submerges}"
			renameFile(runFilename, getTargetfileName(submerges));

			if (printDetails)
			{
				Communicator::output_inform("Finished merging runs " + to_string(currMinRun) + " to " + to_string(currMaxRun) + ".");
			}

			if (printFiles)
			{
				Communicator::output_inform("Printing first " + to_string(defaultPrintedRecordsCount) + " records of " + getTargetfileName(submerges));
				print(getTargetfileName(submerges), defaultPrintedRecordsCount);
			}

			// Calculate new range of merge
			currMinRun = currMaxRun + 1;
			currMaxRun = min(currMinRun + memoryBuffersCount - 2, maxRun);
		}

		// Update merges number for stats
		merges += submerges;

		// If last file was not merged, change its name to include it on next level
		if (currMinRun == maxRun)
		{
			// If last file was not merged, change its name to include it on next level
			submerges++;
			renameFile(getTargetfileName(maxRun), getTargetfileName(submerges));
		}

		// Update range of runs
		maxRun = submerges;
	}

	// Set final merge file as output file
	deleteFile(fileManager.getMainFilename());
	renameFile(getTargetfileName(minRun), fileManager.getMainFilename());

	if (printDetails)
	{
		cout << "Finished full sorting process:" << endl;
		cout << "Disk operations: " + to_string(diskOperations) << endl;
		cout << "Runs: " + to_string(runs) << endl;
		cout << "Merges: " + to_string(merges) << endl;
	}

	if (printFiles)
	{
		Communicator::output_inform("Printing first " + to_string(defaultPrintedRecordsCount) + " records of " + fileManager.getMainFilename());
		print(fileManager.getMainFilename(), defaultPrintedRecordsCount);
	}

	// Destroy buffers
	deallocateMemoryBuffers(printDetails);
}


void DataManager::recordsRandomize(int number)
{
	// Count generated records
	recordsNumber += number;

	vector<double*> recordsBuffers;
	double* buffer = 0;
	int size;

	// Generate records 
	for (int i = 0; i < number; i++)
	{
		size = Record::randomize(buffer);
		recordsBuffers.push_back(buffer);
	}

	// Save generated records to a file
	fileManager.saveBuffers(fileManager.getMainFilename(), recordsBuffers, size);
}


void DataManager::recordsInput(int number)
{
	// Count generated records
	recordsNumber += number;

	vector<double*> recordsBuffers;
	double* buffer = 0;
	int size;


	// Generate records 
	for (int i = 0; i < number; i++)
	{
		Communicator::output_inform("Enter record number " + to_string(i) + ":");

		size = Record::input(buffer);
		recordsBuffers.push_back(buffer);
	}

	// Save generated records to a file
	fileManager.saveBuffers(fileManager.getMainFilename(), recordsBuffers, size);
}


int DataManager::recordsRead(string filename, int number)
{
	// Read records from text file
	double* buffer;

	// Number will store number of full loaded records
	number = fileManager.loadTxtDoubles(filename, buffer, Record::getParamsNumber() * number) / Record::getParamsNumber();

	// Save loaded records to mainfile
	fileManager.saveBuffer(fileManager.getMainFilename(), buffer, Record::getParamsNumber() * number, true);

	// Count generated records
	recordsNumber += number;

	return number;
}


void DataManager::setMemoryBuffersCount(int memoryBuffersCount)
{
	this->memoryBuffersCount = memoryBuffersCount;
}


int DataManager::getMemoryBufferSize()
{
	return memoryBufferSize;
}


int DataManager::getMemoryBuffersCount()
{
	return memoryBuffersCount;
}


int DataManager::getRecordsNumber()
{
	return recordsNumber;
}


int DataManager::print(string filename, int recordsNumber)
{
	int n = Record::getParamsNumber();
	int doublesNumber = recordsNumber * n;

	// Generate new buffer for printing purpose
	double* buffer = nullptr;

	// Load doubles
	doublesNumber = fileManager.staticLoadDoubles(filename, buffer, doublesNumber);
	recordsNumber = doublesNumber / n;

	for (int i = 0; i < recordsNumber; i++)
	{
		Record::printRecord(buffer[i * n], buffer[i * n + 1], buffer[i * n + 2], buffer[i * n + 3], buffer[i * n + 4]);
	}

	delete[] buffer;

	return recordsNumber;
}


void DataManager::reset()
{
	// Reset all fileManager data
	fileManager.reset();

	// Reset all variables
	recordsNumber = 0;
	diskOperations = 0;

	memoryBufferSize = defaultBufferSize;
	memoryBuffersCount = defaultBuffersCount;
}
