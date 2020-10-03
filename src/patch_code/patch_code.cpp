#include <sdw.h>
#include "../common/patch.h"
#include "../common/string.h"

int UMain(int argc, UChar* argv[])
{
	UString sCodeFileName;
	UString sInputCodeFileName;
	UString sOutputCodeFileName;
	if (argc == 3)
	{
		sCodeFileName = argv[1];
		sInputCodeFileName = argv[2];
		sOutputCodeFileName = sInputCodeFileName;
	}
	else if (argc == 4)
	{
		sCodeFileName = argv[1];
		sInputCodeFileName = argv[2];
		sOutputCodeFileName = argv[3];
	}
	else
	{
		// TODO UPrintf Error
		return 1;
	}
	FILE* fp = UFopen(sCodeFileName.c_str(), USTR("rb"), false);
	if (fp == nullptr)
	{
		// TODO UPrintf Error
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uTxtSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* pTemp = new char[uTxtSize + 1];
	fread(pTemp, 1, uTxtSize, fp);
	fclose(fp);
	pTemp[uTxtSize] = '\0';
	string sTxt = pTemp;
	delete[] pTemp;
	vector<string> vTxt = SplitOf(sTxt, "\r\n");
	for (vector<string>::iterator it = vTxt.begin(); it != vTxt.end(); ++it)
	{
		*it = trim(*it);
	}
	vector<string>::iterator itLine = remove_if(vTxt.begin(), vTxt.end(), empty);
	vTxt.erase(itLine, vTxt.end());
	if (vTxt.empty() && sInputCodeFileName == sOutputCodeFileName)
	{
		return 0;
	}
	fp = UFopen(sInputCodeFileName.c_str(), USTR("rb"), false);
	if (fp == nullptr)
	{
		// TODO UPrintf Error
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uCodeSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	u8* pCode = new u8[uCodeSize];
	fread(pCode, 1, uCodeSize, fp);
	fclose(fp);
	if (!vTxt.empty())
	{
		if (!patchCode(vTxt, pCode, uCodeSize))
		{
			delete[] pCode;
			// TODO UPrintf Error
			return 1;
		}
	}
	fp = UFopen(sOutputCodeFileName.c_str(), USTR("wb"), false);
	if (fp == nullptr)
	{
		delete[] pCode;
		// TODO UPrintf Error
		return 1;
	}
	fwrite(pCode, 1, uCodeSize, fp);
	fclose(fp);
	delete[] pCode;
	return 0;
}
