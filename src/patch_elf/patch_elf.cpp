#include <sdw.h>
#include "../common/patch.h"
#include "../common/string.h"

int UMain(int argc, UChar* argv[])
{
	UString sCodeFileName;
	UString sInputElfFileName;
	UString sOutputElfFileName;
	if (argc == 3)
	{
		sCodeFileName = argv[1];
		sInputElfFileName = argv[2];
		sOutputElfFileName = sInputElfFileName;
	}
	else if (argc == 4)
	{
		sCodeFileName = argv[1];
		sInputElfFileName = argv[2];
		sOutputElfFileName = argv[3];
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
	if (vTxt.empty() && sInputElfFileName == sOutputElfFileName)
	{
		return 0;
	}
	const u32 uOffsetOffset = 0x48;
	fp = UFopen(sInputElfFileName.c_str(), USTR("rb"), false);
	if (fp == nullptr)
	{
		// TODO UPrintf Error
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uElfSize = ftell(fp);
	if (uElfSize < uOffsetOffset + sizeof(u32))
	{
		fclose(fp);
		// TODO UPrintf Error
		return 1;
	}
	fseek(fp, 0, SEEK_SET);
	u8* pElf = new u8[uElfSize];
	fread(pElf, 1, uElfSize, fp);
	fclose(fp);
	u32 uBaseOffset = *reinterpret_cast<u32*>(pElf + uOffsetOffset);
	if (uBaseOffset >= uElfSize)
	{
		delete[] pElf;
		// TODO UPrintf Error
		return 1;
	}
	if (!vTxt.empty())
	{
		if (!patchCode(vTxt, pElf + uBaseOffset, uElfSize - uBaseOffset))
		{
			delete[] pElf;
			// TODO UPrintf Error
			return 1;
		}
	}
	fp = UFopen(sOutputElfFileName.c_str(), USTR("wb"), false);
	if (fp == nullptr)
	{
		delete[] pElf;
		// TODO UPrintf Error
		return 1;
	}
	fwrite(pElf, 1, uElfSize, fp);
	fclose(fp);
	delete[] pElf;
	return 0;
}
