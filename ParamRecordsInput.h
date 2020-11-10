#pragma once
#include "stdafx.h"
#include "Parameter.h"
#include "Communicator.h"

class ParamRecordsInput : Parameter
{
public:
	ParamRecordsInput(DataManager* dataManager);

	string toString();
	void execute(vector<string> userInputParsed);
};
