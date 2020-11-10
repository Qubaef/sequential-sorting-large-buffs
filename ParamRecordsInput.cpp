#include "ParamRecordsInput.h"


ParamRecordsInput::ParamRecordsInput(DataManager* dataManager)
	: Parameter("recinput", dataManager)
{
}


string ParamRecordsInput::toString()
{
	return "recinput [number of records]";
}


void ParamRecordsInput::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 2)
	{
		int number = stoi(userInputParsed[1]);
		if (number > 0)
		{
			dataManager->recordsInput(number);
			Communicator::output_success("Successfully added " + to_string(number) + " records.");
		}
		else
		{
			Communicator::output_error("Number must be larger than 0.");
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