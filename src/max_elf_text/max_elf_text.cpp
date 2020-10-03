#include <sdw.h>

int UMain(int argc, UChar* argv[])
{
	UString sInputElfFileName;
	UString sOutputElfFileName;
	if (argc == 2)
	{
		sInputElfFileName = argv[1];
		sOutputElfFileName = sInputElfFileName;
	}
	else if (argc == 3)
	{
		sInputElfFileName = argv[1];
		sOutputElfFileName = argv[2];
	}
	else
	{
		// TODO UPrintf Error
		return 1;
	}
	const u32 uOffsetOffset = 0x48;
	FILE* fp = UFopen(sInputElfFileName.c_str(), USTR("rb"), false);
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
	if (uElfSize < 0xB8 + sizeof(u32))
	{
		delete[] pElf;
		// TODO UPrintf Error
		return 1;
	}
	u32 uRoDataOffset = *reinterpret_cast<u32*>(pElf + 0x88);
	u32 uTextSize = *reinterpret_cast<u32*>(pElf + 0x60);
	if (uTextSize > uRoDataOffset)
	{
		delete[] pElf;
		// TODO UPrintf Error
		return 1;
	}
	if (uTextSize == uRoDataOffset && sInputElfFileName == sOutputElfFileName)
	{
		delete[] pElf;
		return 0;
	}
	u32 uDelta = uRoDataOffset - uTextSize;
	u32 uOffset[6] = { 0x60, 0x68, 0x80, 0x98, 0xA0, 0xB8 };
	for (n32 i = 0; i < SDW_ARRAY_COUNT(uOffset); i++)
	{
		*reinterpret_cast<u32*>(pElf + uOffset[i]) = *reinterpret_cast<u32*>(pElf + uOffset[i]) + uDelta;
	}
	vector<u8> vInsert(uDelta);
	fp = UFopen(sOutputElfFileName.c_str(), USTR("wb"), false);
	if (fp == nullptr)
	{
		delete[] pElf;
		// TODO UPrintf Error
		return 1;
	}
	fwrite(pElf, 1, uBaseOffset + uTextSize, fp);
	if (!vInsert.empty())
	{
		fwrite(&*vInsert.begin(), 1, vInsert.size(), fp);
	}
	fwrite(pElf + uBaseOffset + uTextSize, 1, uElfSize - (uBaseOffset + uTextSize), fp);
	fclose(fp);
	delete[] pElf;
	return 0;
}
