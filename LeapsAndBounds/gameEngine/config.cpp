#include "config.hpp"

// class Config
Config::Config(string _filepath)
	:filepath(_filepath)
{
	ifstream i(this->filepath);

	i >> data;
}

void Config::save()
{
	ofstream o(filepath);
	o << setw(4) << data << endl;
}

void Config::trace()
{
	cout << data << endl;
}
