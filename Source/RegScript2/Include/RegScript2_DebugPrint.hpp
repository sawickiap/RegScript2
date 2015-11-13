#pragma once

#include "RegScript2.hpp"

namespace RegScript2
{

void DebugPrintParam(IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const BoolParamDesc& paramDesc,
	uint32_t indentLevel);
void DebugPrintParam(IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const UintParamDesc& paramDesc,
	uint32_t indentLevel);
void DebugPrintParam(IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const FloatParamDesc& paramDesc,
	uint32_t indentLevel);
void DebugPrintParam(IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const StringParamDesc& paramDesc,
	uint32_t indentLevel);
void DebugPrintParam(IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const GameTimeParamDesc& paramDesc,
	uint32_t indentLevel);
void DebugPrintParam(IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const Vec2ParamDesc& paramDesc,
	uint32_t indentLevel);
void DebugPrintParam(IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const Vec3ParamDesc& paramDesc,
	uint32_t indentLevel);
void DebugPrintParam(IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const Vec4ParamDesc& paramDesc,
	uint32_t indentLevel);
void DebugPrintParam(IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const StructParamDesc& paramDesc,
	uint32_t indentLevel);
void DebugPrintParam(IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const FixedSizeArrayParamDesc& paramDesc,
	uint32_t indentLevel);
// ADD NEW PARAMETER TYPES HERE.

void DebugPrintParam(
	IPrinter& printer,
	const void* srcParam,
	const wchar_t* paramName,
	const ParamDesc& paramDesc,
	uint32_t indentLevel);

void DebugPrintObj(
	IPrinter& printer,
	const void* srcObj,
	const StructDesc& structDesc,
	uint32_t indentLevel = 0);

} // namespace RegScript2
