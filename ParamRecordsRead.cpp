#include "ParamRecordsRead.h"

ParamRecordsRead::ParamRecordsRead(DataManager* dataManager)
	: Parameter("recread", dataManager)
{
}


string ParamRecordsRead::toString()
{
	return "recread [file path] [number of records]";
}


void ParamRecordsRead::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 3)
	{
		int number = stoi(userInputParsed[2]);

		if (number > 0)
		{
			if (FILE *file = fopen(userInputParsed[1].c_str(), "r")) {
				fclose(file);

				number = dataManager->recordsRead(userInputParsed[1], number);
				Communicator::output_success("Successfully loaded " + to_string(number) + " records from " + userInputParsed[1]);
			}
			else
			{
				Communicator::output_error("Given file not accessible.");
			}
		}
		else
		{
			Communicator::output_error("Number must be greater than 0.");
		}

	}
	else if (userInputParsed.size() < 3)
	{
		Communicator::output_error("Too few arguments were given. Should be 3.");
	}
	else if (userInputParsed.size() > 3)
	{
		Communicator::output_error("Too many arguments were given. Should be 3.");
	}
}