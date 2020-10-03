#include <sdw.h>

#include SDW_MSC_PUSH_PACKED
struct NsoHeader
{
	u32 Signature;
	u32 Version;
	u32 Reserved1;
	u32 Flags;
	u32 TextFileOffset;
	u32 TextMemoryOffset;
	u32 TextSize;
	u32 ModuleNameOffset;
	u32 RoFileOffset;
	u32 RoMemoryOffset;
	u32 RoSize;
	u32 ModuleNameSize;
	u32 DataFileOffset;
	u32 DataMemoryOffset;
	u32 DataSize;
	u32 BssSize;
	u8 ModuleId[32];
	u32 TextFileSize;
	u32 RoFileSize;
	u32 DataFileSize;
	u8 Reserved2[4];
	u32 EmbededOffset;
	u32 EmbededSize;
	u8 Reserved3[40];
	u8 TextHash[32];
	u8 RoHash[32];
	u8 DataHash[32];
} SDW_GNUC_PACKED;
#include SDW_MSC_POP_PACKED

int UMain(int argc, UChar* argv[])
{
	UString sInputNsoHeaderFileName;
	UString sOutputNsoHeaderFileName;
	if (argc == 2)
	{
		sInputNsoHeaderFileName = argv[1];
		sOutputNsoHeaderFileName = sInputNsoHeaderFileName;
	}
	else if (argc == 3)
	{
		sInputNsoHeaderFileName = argv[1];
		sOutputNsoHeaderFileName = argv[2];
	}
	else
	{
		// TODO UPrintf Error
		return 1;
	}
	FILE* fp = UFopen(sInputNsoHeaderFileName.c_str(), USTR("rb"), false);
	if (fp == nullptr)
	{
		// TODO UPrintf Error
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uNsoHeaderSize = ftell(fp);
	if (uNsoHeaderSize < sizeof(NsoHeader))
	{
		fclose(fp);
		// TODO UPrintf Error
		return 1;
	}
	fseek(fp, 0, SEEK_SET);
	u8* pNso = new u8[uNsoHeaderSize];
	fread(pNso, 1, uNsoHeaderSize, fp);
	fclose(fp);
	NsoHeader* pNsoHeader = reinterpret_cast<NsoHeader*>(pNso);
	if (pNsoHeader->TextSize == pNsoHeader->RoMemoryOffset && sInputNsoHeaderFileName == sOutputNsoHeaderFileName)
	{
		delete[] pNso;
		return 0;
	}
	pNsoHeader->TextSize = pNsoHeader->RoMemoryOffset;
	fp = UFopen(sOutputNsoHeaderFileName.c_str(), USTR("wb"), false);
	if (fp == nullptr)
	{
		delete[] pNso;
		// TODO UPrintf Error
		return 1;
	}
	fwrite(pNso, 1, uNsoHeaderSize, fp);
	fclose(fp);
	delete[] pNso;
	return 0;
}
