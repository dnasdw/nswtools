#include <sdw.h>

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

bool patchElf(const vector<string>& a_vTxt, u8* a_pElf, u32 a_uElfSize)
{
	const u32 uOffsetOffset = 0x48;
	u32 uBaseOffset = *reinterpret_cast<u32*>(a_pElf + uOffsetOffset);
	bool bHasOffset = false;
	bool bHasValue = false;
	u32 uOffset = 0;
	u32 uValue = 0;
	for (vector<string>::const_iterator it = a_vTxt.begin(); it != a_vTxt.end(); ++it)
	{
		const string& sLine = *it;
		vector<string> vLine = Split(sLine, " ");
		if (vLine.size() != 3)
		{
			// TODO UPrintf Error
			return false;
		}
		u32 uCode[3] = { SToU32(vLine[0], 16), SToU32(vLine[1], 16), SToU32(vLine[2], 16) };
		if ((uCode[0] & 0xFFFF0000) == 0x04070000)
		{
			if (bHasOffset || bHasValue)
			{
				// TODO UPrintf Error
				return false;
			}
			bHasOffset = true;
			bHasValue = true;
			uOffset = uBaseOffset + uCode[1];
			uValue = uCode[2];
		}
		else if ((uCode[0] & 0xFFFF0000) == 0x40070000)
		{
			if (bHasOffset || bHasValue)
			{
				// TODO UPrintf Error
				return false;
			}
			bHasOffset = true;
			uOffset = uBaseOffset + uCode[2];
		}
		else if ((uCode[0] & 0xFFFF0000) == 0x640F0000)
		{
			if (!bHasOffset || bHasValue)
			{
				// TODO UPrintf Error
				return false;
			}
			bHasValue = true;
			uValue = uCode[2];
		}
		else
		{
			// TODO UPrintf Error
			return false;
		}
		if (bHasOffset && bHasValue)
		{
			if (uOffset + 4 > a_uElfSize)
			{
				// TODO UPrintf Error
				return false;
			}
			*reinterpret_cast<u32*>(a_pElf + uOffset) = uValue;
			bHasOffset = false;
			bHasValue = false;
			uOffset = 0;
			uValue = 0;
		}
	}
	if (bHasOffset || bHasValue)
	{
		// TODO UPrintf Error
		return false;
	}
	return true;
}

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
	if (vTxt.empty())
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
	if (uBaseOffset != 0x788)
	{
		// TODO UPrintf Warning
	}
	if (uBaseOffset >= uElfSize)
	{
		delete[] pElf;
		// TODO UPrintf Error
		return 1;
	}
	if (!patchElf(vTxt, pElf, uElfSize))
	{
		delete[] pElf;
		// TODO UPrintf Error
		return 1;
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
