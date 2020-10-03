#include "patch.h"

bool patchCode(const vector<string>& a_vTxt, u8* a_pCode, u32 a_uCodeSize)
{
	bool bHasOffset = false;
	bool bHasValue = false;
	u32 uOffset = 0;
	u32 uValueSize = 4;
	u8 uValue[8] = {};
	for (vector<string>::const_iterator it = a_vTxt.begin(); it != a_vTxt.end(); ++it)
	{
		const string& sLine = *it;
		vector<string> vLine = Split(sLine, " ");
		if (vLine.size() != 3 && vLine.size() != 4)
		{
			// TODO UPrintf Error
			return false;
		}
		u32 uCode[4] = { SToU32(vLine[0], 16), SToU32(vLine[1], 16), SToU32(vLine[2], 16) };
		if (vLine.size() == 4)
		{
			uCode[3] = SToU32(vLine[3], 16);
		}
		if ((uCode[0] & 0xF0FF0000) == 0x00070000)
		{
			if (bHasOffset || bHasValue)
			{
				// TODO UPrintf Error
				return false;
			}
			uOffset = uCode[1];
			uValueSize = uCode[0] >> 24 & 0xF;
			switch (uValueSize)
			{
			case 1:
			case 2:
			case 4:
				if (vLine.size() != 3)
				{
					// TODO UPrintf Error
					return false;
				}
				*reinterpret_cast<u64*>(uValue) = uCode[2];
				break;
			case 8:
				if (vLine.size() != 4)
				{
					// TODO UPrintf Error
					return false;
				}
				*reinterpret_cast<u64*>(uValue) = uCode[2];
				*reinterpret_cast<u64*>(uValue) <<= 32;
				*reinterpret_cast<u64*>(uValue) |= uCode[3];
				break;
			default:
				// TODO UPrintf Error
				return false;
			}
			bHasOffset = true;
			bHasValue = true;
		}
		else if ((uCode[0] & 0xFFFF0000) == 0x40070000)
		{
			if (bHasOffset || bHasValue)
			{
				// TODO UPrintf Error
				return false;
			}
			if (vLine.size() != 3)
			{
				// TODO UPrintf Error
				return false;
			}
			if (uCode[1] != 0)
			{
				// TODO UPrintf Error
				return false;
			}
			uOffset = uCode[2];
			bHasOffset = true;
		}
		else if ((uCode[0] & 0xF0FF0000) == 0x600F0000)
		{
			if (!bHasOffset || bHasValue)
			{
				// TODO UPrintf Error
				return false;
			}
			uValueSize = uCode[0] >> 24 & 0xF;
			switch (uValueSize)
			{
			case 1:
			case 2:
			case 4:
				if (vLine.size() != 3)
				{
					// TODO UPrintf Error
					return false;
				}
				*reinterpret_cast<u64*>(uValue) = uCode[2];
				break;
			case 8:
				if (vLine.size() != 4)
				{
					// TODO UPrintf Error
					return false;
				}
				*reinterpret_cast<u64*>(uValue) = uCode[2];
				*reinterpret_cast<u64*>(uValue) <<= 32;
				*reinterpret_cast<u64*>(uValue) |= uCode[3];
				break;
			default:
				// TODO UPrintf Error
				return false;
			}
			bHasValue = true;
		}
		else
		{
			// TODO UPrintf Error
			return false;
		}
		if (bHasOffset && bHasValue)
		{
			if (uOffset + uValueSize > a_uCodeSize)
			{
				// TODO UPrintf Error
				return false;
			}
			memcpy(a_pCode + uOffset, uValue, uValueSize);
			bHasOffset = false;
			bHasValue = false;
			uOffset = 0;
			*reinterpret_cast<u64*>(uValue) = 0;
		}
	}
	if (bHasOffset || bHasValue)
	{
		// TODO UPrintf Error
		return false;
	}
	return true;
}
