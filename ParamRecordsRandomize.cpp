#include "ParamRecordsRandomize.h"

ParamRecordsRandomize::ParamRecordsRandomize(DataManager* dataManager)
	: Parameter("recrand", dataManager)
{
}


string ParamRecordsRandomize::toString()
{
	return "recrand [number of records]";
}


void ParamRecordsRandomize::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 2)
	{
		int number = stoi(userInputParsed[1]);
		if (number > 0)
		{
			dataManager->recordsRandomize(number);
			Communicator::output_success("Successfully added " + to_string(number) + " records");
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
