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
	
static void DebugPrintParamValue(
	IPrinter& printer,
	uint32_t indentLevel,
	const wchar_t* paramName,
	const wchar_t* valueStr)
{
	printer.printf(L"%s%s = %s", GetIndent(indentLevel), paramName, valueStr);
}

static void DebugPrintParamDefault(
	IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const ParamDesc& paramDesc,
	uint32_t indentLevel)
{
	wstring valueStr;
	bool ok = paramDesc.ToString(valueStr, srcParam);
	assert(ok);
	DebugPrintParamValue(printer, indentLevel, paramName, valueStr.c_str());
}

static void DebugPrintStructParam(
	IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const StructParamDesc& paramDesc,
	uint32_t indentLevel)
{
	printer.printf(L"%s%s:", GetIndent(indentLevel), paramName);
	DebugPrintObj(printer, srcParam, *paramDesc.GetStructDesc(), indentLevel + 1);
}

static void DebugPrintFixedSizeArrayParam(
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
	if(typeid(BoolParamDesc) == typeid(paramDesc) ||
		typeid(UintParamDesc) == typeid(paramDesc) ||
		typeid(FloatParamDesc) == typeid(paramDesc) ||
		typeid(StringParamDesc) == typeid(paramDesc) ||
		typeid(GameTimeParamDesc) == typeid(paramDesc) ||
		typeid(Vec2ParamDesc) == typeid(paramDesc) ||
		typeid(Vec3ParamDesc) == typeid(paramDesc) ||
		typeid(Vec4ParamDesc) == typeid(paramDesc))
		DebugPrintParamDefault(printer, srcParam, paramName, paramDesc, indentLevel);
	else if(typeid(StructParamDesc) == typeid(paramDesc))
		DebugPrintStructParam(printer, srcParam, paramName, (const StructParamDesc&)paramDesc, indentLevel);
	else if(typeid(FixedSizeArrayParamDesc) == typeid(paramDesc))
		DebugPrintFixedSizeArrayParam(printer, srcParam, paramName, (const FixedSizeArrayParamDesc&)paramDesc, indentLevel);
	// ADD NEW PARAMETER TYPES HERE.
	else
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
