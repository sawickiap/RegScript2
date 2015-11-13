#include "Include/RegScript2.hpp"
#include <vector>
#include <cstdio>
#include <cstdarg>

namespace RegScript2
{

} // namespace RegScript2

void Format(std::wstring& str, const wchar_t* format, ...)
{
	va_list argList;
	va_start(argList, format);
	VFormat(str, format, argList);
	va_end(argList);
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

wstring Format_r(const wchar_t* format, ...)
{
	wstring result;

	va_list argList;
	va_start(argList, format);
	VFormat(result, format, argList);
	va_end(argList);

	return result;
}

void AppendFormat(std::wstring& str, const wchar_t* format, ...)
{
	va_list argList;
	va_start(argList, format);
	AppendVFormat(str, format, argList);
	va_end(argList);
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
