#pragma once

#include <Common/Base.hpp>
#include <Common/DateTime.hpp>

#include <string>

namespace RegScript2
{

class IPrinter
{
public:
	virtual ~IPrinter() { }
	virtual void printf(const wchar_t* format, ...) = 0;
};

} // namespace RegScript2

void Format(std::wstring& str, const wchar_t* format, ...);
void VFormat(std::wstring& str, const wchar_t* format, va_list argList);
wstring Format_r(const wchar_t* format, ...);

void AppendFormat(std::wstring& str, const wchar_t* format, ...);
void AppendVFormat(std::wstring& str, const wchar_t* format, va_list argList);

// Returns textual representation of time duration, e.g. "12.5 ms" or "1:05:02".
void SecondsToFriendlyStr(std::wstring& out, double seconds);

inline void GameTimeToFriendlyStr(wstring& out, common::GameTime time)
{
	SecondsToFriendlyStr(out, time.ToSeconds_d());
}
