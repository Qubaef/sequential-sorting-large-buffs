#include "ParamPrintFile.h"

ParamPrintFile::ParamPrintFile(DataManager* dataManager)
	: Parameter("print", dataManager)
{
}


string ParamPrintFile::toString()
{
	return "print [filename] [number of records to print]";
}


void ParamPrintFile::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 3)
	{
		int number = stoi(userInputParsed[2]);

		if (number > 0)
		{
			if (FILE *file = fopen(userInputParsed[1].c_str(), "r")) {
				fclose(file);

				number = dataManager->print(userInputParsed[1], number);
				Communicator::output_success("Successfully printed " + to_string(number) + " found records.");
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
