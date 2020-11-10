#pragma once
#include "stdafx.h"
#include "Parameter.h"
#include "Communicator.h"

class ParamBufferConfig : Parameter
{
public:
	ParamBufferConfig(DataManager* dataManager);

	string toString();
	void execute(vector<string> userInputParsed);
};
