#include "Include/RegScript2_DebugPrint.hpp"
#include <cmath>

namespace RegScript2
{

static const wchar_t* const INDENT =
	L"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

static const wchar_t* GetIndent(uint32_t indentLevel)
{
	assert(indentLevel <= 64);
	return INDENT + (64 - indentLevel);
}
	
static inline float PowerToDB(float powerRatio)
{
	if(powerRatio > 0.f)
		return 10.f * log10(powerRatio);
	else
		return -1000.f;
}

static void DebugPrintParamValue(
	IPrinter& printer,
	uint32_t indentLevel,
	const wchar_t* paramName,
	const wchar_t* valueStr)
{
	printer.printf(L"%s%s = %s", GetIndent(indentLevel), paramName, valueStr);
}

template<typename T>
static void DebugPrintParamValueDefault(
	IPrinter& printer,
	uint32_t indentLevel,
	const wchar_t* paramName,
	const T& value)
{
	wstring valueStr;
	SthToStr<T>(&valueStr, value);
	DebugPrintParamValue(printer, indentLevel, paramName, valueStr.c_str());
}

void DebugPrintParam(
	IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const BoolParamDesc& paramDesc,
	uint32_t indentLevel)
{
	const BoolParam* boolParam = (const BoolParam*)srcParam;
	DebugPrintParamValueDefault(printer, indentLevel, paramName, boolParam->Value);
}

void DebugPrintParam(
	IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const UintParamDesc& paramDesc,
	uint32_t indentLevel)
{
	const UintParam* uintParam = (const UintParam*)srcParam;
	wstring valueStr;
	common::UintToStr(&valueStr, uintParam->Value, paramDesc.Base);
	DebugPrintParamValue(printer, indentLevel, paramName, valueStr.c_str());
}

void DebugPrintParam(
	IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const FloatParamDesc& paramDesc,
	uint32_t indentLevel)
{
	const FloatParam* floatParam = (const FloatParam*)srcParam;
	const float value = floatParam->Value;
	wstring valueStr;
	if(isfinite(value))
	{
		if(paramDesc.Format == FloatParamDesc::FORMAT_PERCENT)
		{
			SthToStr<float>(&valueStr, value * 100.f);
			valueStr += L'%';
			DebugPrintParamValue(printer, indentLevel, paramName, valueStr.c_str());
			return;
		}
		if(paramDesc.Format == FloatParamDesc::FORMAT_DB && value > 0.f)
		{
			SthToStr<float>(&valueStr, PowerToDB(value));
			valueStr += L"dB";
			DebugPrintParamValue(printer, indentLevel, paramName, valueStr.c_str());
			return;
		}
	}
	DebugPrintParamValueDefault(printer, indentLevel, paramName, value);
}

void DebugPrintParam(
	IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const StringParamDesc& paramDesc,
	uint32_t indentLevel)
{
	const StringParam* stringParam = (const StringParam*)srcParam;
	DebugPrintParamValue(printer, indentLevel, paramName, stringParam->Value.c_str());
}

void DebugPrintParam(
	IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const GameTimeParamDesc& paramDesc,
	uint32_t indentLevel)
{
	const GameTimeParam* gameTimeParam = (const GameTimeParam*)srcParam;
	wstring valueStr;
	GameTimeToFriendlyStr(valueStr, gameTimeParam->Value);
	DebugPrintParamValue(printer, indentLevel, paramName, valueStr.c_str());
}

void DebugPrintParam(
	IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const Vec2ParamDesc& paramDesc,
	uint32_t indentLevel)
{
	const Vec2Param* vecParam = (const Vec2Param*)srcParam;
	DebugPrintParamValueDefault(printer, indentLevel, paramName, vecParam->Value);
}

void DebugPrintParam(
	IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const Vec3ParamDesc& paramDesc,
	uint32_t indentLevel)
{
	const Vec3Param* vecParam = (const Vec3Param*)srcParam;
	DebugPrintParamValueDefault(printer, indentLevel, paramName, vecParam->Value);
}

void DebugPrintParam(
	IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const Vec4ParamDesc& paramDesc,
	uint32_t indentLevel)
{
	const Vec4Param* vecParam = (const Vec4Param*)srcParam;
	DebugPrintParamValueDefault(printer, indentLevel, paramName, vecParam->Value);
}

void DebugPrintParam(
	IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const StructParamDesc& paramDesc,
	uint32_t indentLevel)
{
	printer.printf(L"%s%s:", GetIndent(indentLevel), paramName);
	DebugPrintObj(printer, srcParam, *paramDesc.GetStructDesc(), indentLevel + 1);
}

void DebugPrintParam(
	IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const FixedSizeArrayParamDesc& paramDesc,
	uint32_t indentLevel)
{
	size_t index = 0;
	const char* srcElement = (const char*)srcParam;
	const size_t elementCount = paramDesc.GetCount();
	const ParamDesc* elementParamDesc = paramDesc.GetElementParamDesc();
	const size_t elementSize = elementParamDesc->GetParamSize();
	wstring elementName;
	for(size_t i = 0; i < elementCount; ++i)
	{
		Format(elementName, L"%s[%u]", paramName, i);
		DebugPrintParam(printer, srcElement, elementName.c_str(), *elementParamDesc, indentLevel);
		srcElement += elementSize;
	}
}

// ADD NEW PARAMETER TYPES HERE.

void DebugPrintParam(IPrinter& printer, const void* srcParam, const wchar_t* paramName, const ParamDesc& paramDesc, uint32_t indentLevel)
{
	if(dynamic_cast<const BoolParamDesc*>(&paramDesc))
		return DebugPrintParam(printer, srcParam, paramName, (const BoolParamDesc&)paramDesc, indentLevel);
	if(dynamic_cast<const UintParamDesc*>(&paramDesc))
		return DebugPrintParam(printer, srcParam, paramName, (const UintParamDesc&)paramDesc, indentLevel);
	if(dynamic_cast<const FloatParamDesc*>(&paramDesc))
		return DebugPrintParam(printer, srcParam, paramName, (const FloatParamDesc&)paramDesc, indentLevel);
	if(dynamic_cast<const StringParamDesc*>(&paramDesc))
		return DebugPrintParam(printer, srcParam, paramName, (const StringParamDesc&)paramDesc, indentLevel);
	if(dynamic_cast<const GameTimeParamDesc*>(&paramDesc))
		return DebugPrintParam(printer, srcParam, paramName, (const GameTimeParamDesc&)paramDesc, indentLevel);
	if(dynamic_cast<const Vec2ParamDesc*>(&paramDesc))
		return DebugPrintParam(printer, srcParam, paramName, (const Vec2ParamDesc&)paramDesc, indentLevel);
	if(dynamic_cast<const Vec3ParamDesc*>(&paramDesc))
		return DebugPrintParam(printer, srcParam, paramName, (const Vec3ParamDesc&)paramDesc, indentLevel);
	if(dynamic_cast<const Vec4ParamDesc*>(&paramDesc))
		return DebugPrintParam(printer, srcParam, paramName, (const Vec4ParamDesc&)paramDesc, indentLevel);
	if(dynamic_cast<const StructParamDesc*>(&paramDesc))
		return DebugPrintParam(printer, srcParam, paramName, (const StructParamDesc&)paramDesc, indentLevel);
	if(dynamic_cast<const FixedSizeArrayParamDesc*>(&paramDesc))
		return DebugPrintParam(printer, srcParam, paramName, (const FixedSizeArrayParamDesc&)paramDesc, indentLevel);
	// ADD NEW PARAMETER TYPES HERE.

	assert(!"Unsupported parameter type.");
}

void DebugPrintObj(IPrinter& printer, const void* srcObj, const StructDesc& structDesc, uint32_t indentLevel)
{
	const StructDesc* baseStructDesc = structDesc.GetBaseStructDesc();
	if(baseStructDesc)
		DebugPrintObj(printer, srcObj, *baseStructDesc, indentLevel);

	for(size_t i = 0, count = structDesc.Params.size(); i < count; ++i)
	{
		DebugPrintParam(
			printer,
			structDesc.AccessRawParam(srcObj, i),
			structDesc.Names[i].c_str(),
			*structDesc.Params[i],
			indentLevel);
	}
}

} // namespace RegScript2
