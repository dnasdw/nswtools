#include "string.h"

string trim(const string& a_sLine)
{
	string sTrimmed = a_sLine;
	string::size_type uPos = sTrimmed.find_first_not_of("\t\n\v\f\r \x85\xA0");
	if (uPos == string::npos)
	{
		return "";
	}
	sTrimmed.erase(0, uPos);
	uPos = sTrimmed.find_last_not_of("\t\n\v\f\r \x85\xA0");
	if (uPos != string::npos)
	{
		sTrimmed.erase(uPos + 1);
	}
	return sTrimmed;
}

bool empty(const string& a_sLine)
{
	return trim(a_sLine).empty();
}
