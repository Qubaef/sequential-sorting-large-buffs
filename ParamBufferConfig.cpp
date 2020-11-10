#include "ParamBufferConfig.h"

ParamBufferConfig::ParamBufferConfig(DataManager* dataManager)
	: Parameter("buff", dataManager)
{
}


string ParamBufferConfig::toString()
{
	return "buff [number of buffers] | current: [" + to_string(dataManager->getMemoryBuffersCount()) + "]";
}


void ParamBufferConfig::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 2)
	{
		int buffersCount = stoi(userInputParsed[1]);
		if (buffersCount > 2)
		{
			dataManager->setMemoryBuffersCount(buffersCount);
			Communicator::output_success("Successfully set buffers number.");
		}
		else
		{
			Communicator::output_error("Number of buffers must be larger than 2.");
		}
	}
	else if (userInputParsed.size() < 2)
	{
		Communicator::output_error("Too few arguments were given. Should be 2.");
	}
	else if (userInputParsed.size() > 2)
	{
		Communicator::output_error("Too many arguments were given. Should be 2.");
	}
}

