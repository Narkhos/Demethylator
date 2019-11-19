#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include "json.hpp"

using namespace std;
class Config
{
public:
	string filepath;
	nlohmann::json data;

	Config(string _filepath);

	void save();

	void trace();
};