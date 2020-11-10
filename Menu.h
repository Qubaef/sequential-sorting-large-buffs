#pragma once
#include "stdafx.h"
#include "ParamBufferConfig.h"
#include "ParamRecordsInput.h"
#include "ParamRecordsRandomize.h"
#include "ParamRecordsRead.h"
#include "ParamPrintFile.h"
#include "ParamSort.h"
#include "ParamReset.h"
#include "Communicator.h"

using namespace std;

class Menu
{
private:
	DataManager* dataManager;
	std::vector<Parameter*> menuParameters;

	// Parse given string to vector of parameters
	vector<string> getParsedInput(string userInput);

	static void display_help(vector<Parameter*>& menuParameters);

public:
	// Initialize menu parameters and run it after initialization
	Menu();

	// Receive user input and manage it
	void run();
};
