#pragma once
#include "stdafx.h"
#include "Parameter.h"
#include "Communicator.h"

class ParamRecordsRandomize : Parameter
{
public:
	ParamRecordsRandomize(DataManager* dataManager);

	string toString();
	void execute(vector<string> userInputParsed);
};
