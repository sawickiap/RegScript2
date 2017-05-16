#include "Include/RegScript2.hpp"
#include <vector>
#include <cstdio>
#include <cstdarg>

namespace RegScript2
{

} // namespace RegScript2

void Format(std::string& str, const char* format, ...)
{
    va_list argList;
    va_start(argList, format);
    VFormat(str, format, argList);
    va_end(argList);
}

void Format(std::wstring& str, const wchar_t* format, ...)
{
	va_list argList;
	va_start(argList, format);
	VFormat(str, format, argList);
	va_end(argList);
}

void VFormat(std::string& str, const char* format, va_list argList)
{
	size_t dstLen = (size_t)_vscprintf(format, argList);

	if(dstLen)
	{
		std::vector<char> buf(dstLen + 1);
		vsprintf_s(&buf[0], dstLen + 1, format, argList);
		str.assign(&buf[0], &buf[dstLen]);
	}
	else
		str.clear();
}

void VFormat(std::wstring& str, const wchar_t* format, va_list argList)
{
    size_t dstLen = (size_t)_vscwprintf(format, argList);

    if(dstLen)
    {
        std::vector<wchar_t> buf(dstLen + 1);
        vswprintf_s(&buf[0], dstLen + 1, format, argList);
        str.assign(&buf[0], &buf[dstLen]);
    }
    else
        str.clear();
}

string Format_r(const char* format, ...)
{
	string result;

	va_list argList;
	va_start(argList, format);
	VFormat(result, format, argList);
	va_end(argList);

	return result;
}

wstring Format_r(const wchar_t* format, ...)
{
    wstring result;

    va_list argList;
    va_start(argList, format);
    VFormat(result, format, argList);
    va_end(argList);

    return result;
}

void AppendFormat(std::string& str, const char* format, ...)
{
	va_list argList;
	va_start(argList, format);
	AppendVFormat(str, format, argList);
	va_end(argList);
}

void AppendFormat(std::wstring& str, const wchar_t* format, ...)
{
    va_list argList;
    va_start(argList, format);
    AppendVFormat(str, format, argList);
    va_end(argList);
}

void AppendVFormat(std::string& str, const char* format, va_list argList)
{
	size_t dstLen = (size_t)_vscprintf(format, argList);

	if(dstLen)
	{
		std::vector<char> buf(dstLen + 1);
		vsprintf_s(&buf[0], dstLen + 1, format, argList);
		str.append(&buf[0], &buf[dstLen]);
	}
}

void AppendVFormat(std::wstring& str, const wchar_t* format, va_list argList)
{
    size_t dstLen = (size_t)_vscwprintf(format, argList);

    if(dstLen)
    {
        std::vector<wchar_t> buf(dstLen + 1);
        vswprintf_s(&buf[0], dstLen + 1, format, argList);
        str.append(&buf[0], &buf[dstLen]);
    }
}

void SecondsToFriendlyStr(std::wstring& out, double seconds)
{
	bool negative = seconds < 0.;
	if(negative)
		seconds = -seconds;

	// 0: 0
	if(seconds == 0.f)
		out = L"0";
	// seconds < 1 ns: Whatever s
	else if(seconds < 1e-9)
		::Format(out, L"%gs", seconds);
	// seconds < 10 ns: N.NNns
	else if(seconds < 1e-8)
		::Format(out, L"%.2fns", seconds * 1e9);
	// seconds < 100 ns: NN.Nns
	else if(seconds < 1e-7)
		::Format(out, L"%.1fns", seconds * 1e9);
	// seconds < 1 us: NNNns
	else if(seconds < 1e-6)
		::Format(out, L"%.0fns", seconds * 1e9);
	// seconds < 10 us: N.NNus
	else if(seconds < 1e-5)
		::Format(out, L"%.2fus", seconds * 1e6);
	// seconds < 100 us: NN.Nus
	else if(seconds < 1e-4)
		::Format(out, L"%.1fus", seconds * 1e6);
	// seconds < 1 ms: NNNus
	else if(seconds < 1e-3)
		::Format(out, L"%.0fus", seconds * 1e6);
	// seconds < 10 ms: N.NNms
	else if(seconds < 1e-2)
		::Format(out, L"%.2fms", seconds * 1e3);
	// seconds < 100 ms: NN.Nms
	else if(seconds < 1e-1)
		::Format(out, L"%.1fms", seconds * 1e3);
	// seconds < 1 s: NNNms
	else if(seconds < 1.0)
		::Format(out, L"%.0fms", seconds * 1e3);
	// seconds < 10 s: N.NNs
	else if(seconds < 10.0)
		::Format(out, L"%.2fs", seconds);
	// seconds < 1 min: NN.Ns"
	else if(seconds < 60.0)
		::Format(out, L"%.1fs", seconds);
	else
	{
		uint64_t secondsU = (uint64_t)(seconds + 0.5);
		uint64_t minutesU = secondsU / 60;
		secondsU %= 60;
		// seconds < 1 h: N:NN
		if(minutesU < 60)
			::Format(out, L"%u:%02u", (uint32_t)minutesU, (uint32_t)secondsU);
		else
		{
			uint64_t hoursU = minutesU / 60;
			minutesU %= 60;
			// N:NN:NN
			::Format(out, L"%u:%02u:%02u", (uint32_t)hoursU, (uint32_t)minutesU, (uint32_t)secondsU);
		}
	}

	if(negative)
		out.insert(0, L"-");
}

bool FriendlyStrToSeconds(double& outSeconds, const wchar_t *str)
{
	size_t strLen = wcslen(str);
	if(strLen == 0)
		return false;

	bool negative = str[0] == L'-';
	if(negative)
	{
		++str;
		--strLen;
	}

	bool ok = true;
	// "m:s" or "h:m:s"
	const wchar_t* firstColon = wcschr(str, L':');
	if(firstColon != nullptr)
	{
		const wchar_t* secondColon = wcschr(firstColon + 1, L':');
		// "h:m:s"
		if(secondColon != nullptr)
		{
			wstring newStr{str, firstColon};
			uint32_t hours = 0;
			ok = common::StrToUint(&hours, newStr) == 0;
			if(ok)
			{		
				newStr = wstring{firstColon + 1, secondColon};
				uint32_t minutes = 0;
				ok = common::StrToUint(&minutes, newStr) == 0;
				if(ok)
				{
					newStr = secondColon + 1;
					ok = common::StrToDouble(&outSeconds, newStr) == 0;
					if(ok)
						outSeconds += ((double)minutes + (double)hours * 60.) * 60.;
				}
			}
		}
		// "m:s"
		else
		{
			wstring newStr{str, firstColon};
			uint32_t minutes = 0;
			ok = common::StrToUint(&minutes, newStr) == 0;
			if(ok)
			{
				newStr = firstColon + 1;
				ok = common::StrToDouble(&outSeconds, newStr) == 0;
				if(ok)
					outSeconds += (double)minutes * 60.;
			}
		}
	}
	else if(common::StrEnds(str, L"ns", true))
	{
		wstring newStr{str, str + (strLen - 2)};
		ok = common::StrToDouble(&outSeconds, newStr) == 0;
		if(ok)
			outSeconds *= 1e-9;
	}
	else if(common::StrEnds(str, L"us", true))
	{
		wstring newStr{str, str + (strLen - 2)};
		ok = common::StrToDouble(&outSeconds, newStr) == 0;
		if(ok)
			outSeconds *= 1e-6;
	}
	else if(common::StrEnds(str, L"ms", true))
	{
		wstring newStr{str, str + (strLen - 2)};
		ok = common::StrToDouble(&outSeconds, newStr) == 0;
		if(ok)
			outSeconds *= 1e-3;
	}
	else if(common::StrEnds(str, L"s", true))
	{
		wstring newStr{str, str + (strLen - 1)};
		ok = common::StrToDouble(&outSeconds, newStr) == 0;
	}
	// No unit: default is seconds.
	else
		ok = common::StrToDouble(&outSeconds, str) == 0;

	if(ok && negative)
		outSeconds = -outSeconds;
	return ok;
}

const common::VEC2 VEC2_MIN = common::VEC2(-FLT_MAX, -FLT_MAX);
const common::VEC3 VEC3_MIN = common::VEC3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
const common::VEC4 VEC4_MIN = common::VEC4(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);

const common::VEC2 VEC2_MAX = common::VEC2(FLT_MAX, FLT_MAX);
const common::VEC3 VEC3_MAX = common::VEC3(FLT_MAX, FLT_MAX, FLT_MAX);
const common::VEC4 VEC4_MAX = common::VEC4(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
