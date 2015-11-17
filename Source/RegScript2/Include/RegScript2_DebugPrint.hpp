#pragma once

#include "RegScript2.hpp"

namespace RegScript2
{

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
