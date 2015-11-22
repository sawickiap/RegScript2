#pragma once

#include <Common/Base.hpp>
#include <Common/Math.hpp>
#include <Common/DateTime.hpp>

#include <string>
#include <cmath>

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
bool FriendlyStrToSeconds(double& outSeconds, const wchar_t *str);

inline void GameTimeToFriendlyStr(wstring& out, common::GameTime time)
{
	SecondsToFriendlyStr(out, time.ToSeconds_d());
}
inline bool FriendlyStrToGameTime(common::GameTime& outTime, const wchar_t* str)
{
	double seconds = 0.;
	if(FriendlyStrToSeconds(seconds, str))
	{
		outTime = common::SecondsToGameTime(seconds);
		return true;
	}
	else
		return false;
}

template<typename UintType>
bool StrToUint_AutoBase(UintType& outValue, const wchar_t* str)
{
	if(common::StrBegins(str, L"0x", false))
		return common::StrToUint<UintType>(&outValue, str + 2, 16) == 0;
	else
		return common::StrToUint<UintType>(&outValue, str, 10) == 0;
}

inline float PowerToDB(float powerRatio)
{
	if(powerRatio > 0.f)
		return 10.f * log10(powerRatio);
	else
		return -1000.f;
}

inline float DBToPower(float db)
{
	return pow(10.f, db * 0.1f);
}

inline void Replicate(common::VEC2 &out, float value) { out.x = out.y = value; }
inline void Replicate(common::VEC3 &out, float value) { out.x = out.y = out.z = value; }
inline void Replicate(common::VEC4 &out, float value) { out.x = out.y = out.z = out.w = value; }

// Replicated -FLT_MAX.
extern const common::VEC2 VEC2_MIN;
extern const common::VEC3 VEC3_MIN;
extern const common::VEC4 VEC4_MIN;

// Replicated FLT_MAX.
extern const common::VEC2 VEC2_MAX;
extern const common::VEC3 VEC3_MAX;
extern const common::VEC4 VEC4_MAX;
