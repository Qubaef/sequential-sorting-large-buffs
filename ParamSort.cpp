#include "ParamSort.h"


ParamSort::ParamSort(DataManager* dataManager)
	: Parameter("sort", dataManager)
{
}


string ParamSort::toString()
{
	return "sort [-i for detailed info after each phase] [-p print file after each phase] | current records: " + to_string(dataManager->getRecordsNumber());
}


void ParamSort::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 1 || userInputParsed.size() == 2 || userInputParsed.size() == 3)
	{
		string detailsParam = "-i";
		string printParam = "-p";

		if (userInputParsed.size() == 3)
		{
			// Validate 1st param
			if (userInputParsed[1] != detailsParam && userInputParsed[1] != printParam)
			{
				Communicator::output_error("Unknown parameter \"" + userInputParsed[1] + "\". ");
				return;
			}

			// Validate 2nd param
			if (userInputParsed[2] != detailsParam && userInputParsed[2] != printParam)
			{
				Communicator::output_error("Unknown parameter \"" + userInputParsed[2] + "\". ");
				return;
			}

			if (userInputParsed[1] == userInputParsed[2])
			{
				if (userInputParsed[1] == detailsParam)
				{
					dataManager->sort(true, false);
					Communicator::output_success("Successfully sorted.");
				}
				else
				{
					dataManager->sort(false, true);
					Communicator::output_success("Successfully sorted.");
				}
			}
			else
			{
				dataManager->sort(true, true);
				Communicator::output_success("Successfully sorted.");
			}
		}
		else if (userInputParsed.size() == 2)
		{
			// Validate 1st param
			if (userInputParsed[1] != detailsParam && userInputParsed[1] != printParam)
			{
				Communicator::output_error("Unknown parameter \"" + userInputParsed[1] + "\". ");
				return;
			}

			if (userInputParsed[1] == detailsParam)
			{
				dataManager->sort(true, false);
				Communicator::output_success("Successfully sorted.");
			}
			else
			{
				dataManager->sort(false, true);
				Communicator::output_success("Successfully sorted.");
			}
		}
		else
		{
			dataManager->sort(false, false);
			Communicator::output_success("Successfully sorted.");
		}
	}
	else if (userInputParsed.size() < 1)
	{
		Communicator::output_error("Too few arguments were given. Should be 1, 2 or 3.");
	}
	else if (userInputParsed.size() > 3)
	{
		Communicator::output_error("Too many arguments were given. Should be 1, 2 or 3.");
	}
}