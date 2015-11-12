#pragma once

#include "RegScript2.hpp"
#include <Common/TokDoc.hpp>

namespace RegScript2
{

enum TOKDOC_FLAGS
{
	// Default. If parameter doesn't exist or is incorrect, throw error.
	TOKDOC_FLAG_REQUIRED = 0x00,
	// If parameter doesn't exist, continue. If incorrect, throw error.
	TOKDOC_FLAG_OPTIONAL_CORRECT = 0x01,
	// If parameter doesn't exist or is incorrect, continue.
	TOKDOC_FLAG_OPTIONAL = 0x02,
	// If parameter doesn't exist or is incorrect but continuing, initialize it with default value.
	// Without this flag, its value is undefined.
	TOKDOC_FLAG_DEFAULT = 0x04,
};

struct STokDocLoadConfig
{
	uint32_t Flags;
	IPrinter* WarningPrinter;

	STokDocLoadConfig() :
		Flags(0), WarningPrinter(nullptr) { }
	STokDocLoadConfig(uint32_t flags, IPrinter* warningPrinter = nullptr) :
		Flags(flags), WarningPrinter(warningPrinter) { }
};

bool LoadParamFromTokDoc(void* dstParam, const BoolParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config);
bool LoadParamFromTokDoc(void* dstParam, const UintParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config);
bool LoadParamFromTokDoc(void* dstParam, const FloatParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config);
bool LoadParamFromTokDoc(void* dstParam, const StringParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config);
bool LoadParamFromTokDoc(void* dstParam, const GameTimeParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config);
bool LoadParamFromTokDoc(void* dstParam, const Vec2ParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config);
bool LoadParamFromTokDoc(void* dstParam, const Vec3ParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config);
bool LoadParamFromTokDoc(void* dstParam, const Vec4ParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config);
bool LoadParamFromTokDoc(void* dstParam, const StructParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config);
bool LoadParamFromTokDoc(void* dstParam, const FixedSizeArrayParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config);
// ADD NEW PARAMETER TYPES HERE.

bool LoadParamFromTokDoc(void* dstParam, const ParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config);

bool LoadObjFromTokDoc(void* dstObj, const StructDesc& structDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config);

} // namespace RegScript2
