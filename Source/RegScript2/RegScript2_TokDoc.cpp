#include "PCH.hpp"
#include "Include/RegScript2_TokDoc.hpp"

namespace RegScript2
{

void SaveParamToTokDoc(common::tokdoc::Node& dstNode, const void* srcParam, const BoolParamDesc& paramDesc)
{
	const BoolParam* boolParam = (const BoolParam*)srcParam;
	common::tokdoc::NodeFrom(dstNode, boolParam->Value);
}

void SaveParamToTokDoc(common::tokdoc::Node& dstNode, const void* srcParam, const UintParamDesc& paramDesc)
{
	const UintParam* uintParam = (const UintParam*)srcParam;
	common::tokdoc::NodeFrom(dstNode, uintParam->Value);
}

void SaveParamToTokDoc(common::tokdoc::Node& dstNode, const void* srcParam, const FloatParamDesc& paramDesc)
{
	const FloatParam* floatParam = (const FloatParam*)srcParam;
	common::tokdoc::NodeFrom(dstNode, floatParam->Value);
}

void SaveParamToTokDoc(common::tokdoc::Node& dstNode, const void* srcParam, const StringParamDesc& paramDesc)
{
	const StringParam* stringParam = (const StringParam*)srcParam;
	common::tokdoc::NodeFrom(dstNode, stringParam->Value);
}

void SaveParamToTokDoc(common::tokdoc::Node& dstNode, const void* srcParam, const GameTimeParamDesc& paramDesc)
{
	const GameTimeParam* gameTimeParam = (const GameTimeParam*)srcParam;
	common::tokdoc::NodeFrom(dstNode, gameTimeParam->Value.ToSeconds_d());
}

void SaveParamToTokDoc(common::tokdoc::Node& dstNode, const void* srcParam, const Vec2ParamDesc& paramDesc)
{
	const Vec2Param* vecParam = (const Vec2Param*)srcParam;
	common::tokdoc::NodeFrom(dstNode, vecParam->Value);
}

void SaveParamToTokDoc(common::tokdoc::Node& dstNode, const void* srcParam, const Vec3ParamDesc& paramDesc)
{
	const Vec3Param* vecParam = (const Vec3Param*)srcParam;
	common::tokdoc::NodeFrom(dstNode, vecParam->Value);
}

void SaveParamToTokDoc(common::tokdoc::Node& dstNode, const void* srcParam, const Vec4ParamDesc& paramDesc)
{
	const Vec4Param* vecParam = (const Vec4Param*)srcParam;
	common::tokdoc::NodeFrom(dstNode, vecParam->Value);
}

void SaveParamToTokDoc(common::tokdoc::Node& dstNode, const void* srcParam, const StructParamDesc& paramDesc)
{
	SaveObjToTokDoc(dstNode, srcParam, *paramDesc.GetStructDesc());
}

void SaveParamToTokDoc(common::tokdoc::Node& dstNode, const void* srcParam, const FixedSizeArrayParamDesc& paramDesc)
{
	size_t index = 0;
	const char* srcElement = (const char*)srcParam;
	const size_t elementCount = paramDesc.GetCount();
	const ParamDesc* elementParamDesc = paramDesc.GetElementParamDesc();
	const size_t elementSize = elementParamDesc->GetParamSize();
	for(size_t i = 0; i < elementCount; ++i)
	{
		common::tokdoc::Node* elementNode = new common::tokdoc::Node();
		dstNode.LinkChildAtEnd(elementNode);
		SaveParamToTokDoc(*elementNode, srcElement, *elementParamDesc);
		srcElement += elementSize;
	}
}

// ADD NEW PARAMETER TYPES HERE.

void SaveParamToTokDoc(common::tokdoc::Node& dstNode, const void* srcParam, const ParamDesc& paramDesc)
{
	if(dynamic_cast<const BoolParamDesc*>(&paramDesc))
		return SaveParamToTokDoc(dstNode, srcParam, (const BoolParamDesc&)paramDesc);
	if(dynamic_cast<const UintParamDesc*>(&paramDesc))
		return SaveParamToTokDoc(dstNode, srcParam, (const UintParamDesc&)paramDesc);
	if(dynamic_cast<const FloatParamDesc*>(&paramDesc))
		return SaveParamToTokDoc(dstNode, srcParam, (const FloatParamDesc&)paramDesc);
	if(dynamic_cast<const StringParamDesc*>(&paramDesc))
		return SaveParamToTokDoc(dstNode, srcParam, (const StringParamDesc&)paramDesc);
	if(dynamic_cast<const GameTimeParamDesc*>(&paramDesc))
		return SaveParamToTokDoc(dstNode, srcParam, (const GameTimeParamDesc&)paramDesc);
	if(dynamic_cast<const Vec2ParamDesc*>(&paramDesc))
		return SaveParamToTokDoc(dstNode, srcParam, (const Vec2ParamDesc&)paramDesc);
	if(dynamic_cast<const Vec3ParamDesc*>(&paramDesc))
		return SaveParamToTokDoc(dstNode, srcParam, (const Vec3ParamDesc&)paramDesc);
	if(dynamic_cast<const Vec4ParamDesc*>(&paramDesc))
		return SaveParamToTokDoc(dstNode, srcParam, (const Vec4ParamDesc&)paramDesc);
	if(dynamic_cast<const StructParamDesc*>(&paramDesc))
		return SaveParamToTokDoc(dstNode, srcParam, (const StructParamDesc&)paramDesc);
	if(dynamic_cast<const FixedSizeArrayParamDesc*>(&paramDesc))
		return SaveParamToTokDoc(dstNode, srcParam, (const FixedSizeArrayParamDesc&)paramDesc);
	// ADD NEW PARAMETER TYPES HERE.

	assert(!"Unsupported parameter type.");
}

void SaveObjToTokDoc(common::tokdoc::Node& dstNode, const void* srcObj, const StructDesc& structDesc)
{
	const StructDesc* baseStructDesc = structDesc.GetBaseStructDesc();
	if(baseStructDesc)
		SaveObjToTokDoc(dstNode, srcObj, *baseStructDesc);

	for(size_t i = 0, count = structDesc.Params.size(); i < count; ++i)
	{
		common::tokdoc::Node* subNode = new common::tokdoc::Node();
		dstNode.LinkChildAtEnd(subNode);
		subNode->Name = structDesc.Names[i];
		SaveParamToTokDoc(
			*subNode,
			structDesc.AccessRawParam(srcObj, i),
			*structDesc.Params[i]);
	}
}

static inline bool IsFlagOptional(uint32_t flags)
{
	return (flags & (TOKDOC_FLAG_OPTIONAL | TOKDOC_FLAG_OPTIONAL_CORRECT)) != 0;
}
static inline bool IsFlagRequired(uint32_t flags)
{
	return !IsFlagOptional(flags);
}

bool LoadParamFromTokDoc(void* dstParam, const BoolParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config)
{
	BoolParam* boolParam = (BoolParam*)dstParam;
	if(common::tokdoc::NodeTo(boolParam->Value, srcNode, IsFlagRequired(config.Flags)))
		return true;
	else
	{
		if((config.Flags & TOKDOC_FLAG_DEFAULT))
			paramDesc.SetToDefault(dstParam);
		if(config.WarningPrinter)
			config.WarningPrinter->printf(L"Invalid bool value.");
		return false;
	}
}

bool LoadParamFromTokDoc(void* dstParam, const UintParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config)
{
	UintParam* uintParam = (UintParam*)dstParam;
	if(common::tokdoc::NodeTo(uintParam->Value, srcNode, IsFlagRequired(config.Flags)))
		return true;
	else
	{
		if((config.Flags & TOKDOC_FLAG_DEFAULT))
			paramDesc.SetToDefault(dstParam);
		if(config.WarningPrinter)
			config.WarningPrinter->printf(L"Invalid uint value.");
		return false;
	}
}

bool LoadParamFromTokDoc(void* dstParam, const FloatParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config)
{
	FloatParam* floatParam = (FloatParam*)dstParam;
	if(common::tokdoc::NodeTo(floatParam->Value, srcNode, IsFlagRequired(config.Flags)))
		return true;
	else
	{
		if((config.Flags & TOKDOC_FLAG_DEFAULT))
			paramDesc.SetToDefault(dstParam);
		if(config.WarningPrinter)
			config.WarningPrinter->printf(L"Invalid float value.");
		return false;
	}
}

bool LoadParamFromTokDoc(void* dstParam, const StringParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config)
{
	StringParam* stringParam = (StringParam*)dstParam;
	if(common::tokdoc::NodeTo(stringParam->Value, srcNode, IsFlagRequired(config.Flags)))
		return true;
	else
	{
		if((config.Flags & TOKDOC_FLAG_DEFAULT))
			paramDesc.SetToDefault(dstParam);
		if(config.WarningPrinter)
			config.WarningPrinter->printf(L"Invalid string value.");
		return false;
	}
}

bool LoadParamFromTokDoc(void* dstParam, const GameTimeParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config)
{
	double seconds = 0.;
	if(common::tokdoc::NodeTo(seconds, srcNode, IsFlagRequired(config.Flags)))
	{
		GameTimeParam* gameTimeParam = (GameTimeParam*)dstParam;
		gameTimeParam->Value = common::SecondsToGameTime(seconds);
		return true;
	}
	else
	{
		if((config.Flags & TOKDOC_FLAG_DEFAULT))
			paramDesc.SetToDefault(dstParam);
		if(config.WarningPrinter)
			config.WarningPrinter->printf(L"Invalid GameTime value.");
		return false;
	}
}

bool LoadParamFromTokDoc(void* dstParam, const Vec2ParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config)
{
	Vec2Param* vecParam = (Vec2Param*)dstParam;
	if(common::tokdoc::NodeTo(vecParam->Value, srcNode, IsFlagRequired(config.Flags)))
		return true;
	else
	{
		if((config.Flags & TOKDOC_FLAG_DEFAULT))
			paramDesc.SetToDefault(dstParam);
		if(config.WarningPrinter)
			config.WarningPrinter->printf(L"Invalid vec2 value.");
		return false;
	}
}

bool LoadParamFromTokDoc(void* dstParam, const Vec3ParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config)
{
	Vec3Param* vecParam = (Vec3Param*)dstParam;
	if(common::tokdoc::NodeTo(vecParam->Value, srcNode, IsFlagRequired(config.Flags)))
		return true;
	else
	{
		if((config.Flags & TOKDOC_FLAG_DEFAULT))
			paramDesc.SetToDefault(dstParam);
		if(config.WarningPrinter)
			config.WarningPrinter->printf(L"Invalid vec2 value.");
		return false;
	}
}

bool LoadParamFromTokDoc(void* dstParam, const Vec4ParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config)
{
	Vec4Param* vecParam = (Vec4Param*)dstParam;
	if(common::tokdoc::NodeTo(vecParam->Value, srcNode, IsFlagRequired(config.Flags)))
		return true;
	else
	{
		if((config.Flags & TOKDOC_FLAG_DEFAULT))
			paramDesc.SetToDefault(dstParam);
		if(config.WarningPrinter)
			config.WarningPrinter->printf(L"Invalid vec2 value.");
		return false;
	}
}

bool LoadParamFromTokDoc(void* dstParam, const StructParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config)
{
	return LoadObjFromTokDoc(dstParam, *paramDesc.GetStructDesc(), srcNode, config);
}

bool LoadParamFromTokDoc(void* dstParam, const FixedSizeArrayParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config)
{
	if(!srcNode.HasChildren())
	{
		if(!IsFlagOptional(config.Flags))
			throw common::Error(L"Array parameter is empty.", __TFILE__, __LINE__);
		if((config.Flags & TOKDOC_FLAG_DEFAULT))
			paramDesc.SetToDefault(dstParam);
		if(config.WarningPrinter)
			config.WarningPrinter->printf(L"Configuration array is empty.");
		return false;
	}
	bool allOk = true;
	common::tokdoc::Node* elementNode = srcNode.GetFirstChild();
	size_t index = 0;
	char* dstElement = (char*)dstParam;
	const size_t elementCount = paramDesc.GetCount();
	const ParamDesc* elementParamDesc = paramDesc.GetElementParamDesc();
	const size_t elementSize = elementParamDesc->GetParamSize();
	while(elementNode && index < elementCount)
	{
		if(!LoadParamFromTokDoc(dstElement, *elementParamDesc, *elementNode, config))
			allOk = false;
		elementNode = elementNode->GetNextSibling();
		++index;
		dstElement += elementSize;
	}
	if((elementNode == nullptr) != (index == elementCount))
	{
		if(!IsFlagOptional(config.Flags))
			throw common::Error(L"Array parameter has invalid size.", __TFILE__, __LINE__);
		if((config.Flags & TOKDOC_FLAG_DEFAULT))
		{
			for(; index < elementCount; ++index)
				paramDesc.SetElementToDefault(dstParam, index);
		}
		if(config.WarningPrinter)
			config.WarningPrinter->printf(L"Configuration array has invalid size.");
		allOk = false;
	}
	return allOk;
}

// ADD NEW PARAMETER TYPES HERE.

bool LoadParamFromTokDoc(void* dstParam, const ParamDesc& paramDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config)
{
	if(dynamic_cast<const BoolParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const BoolParamDesc&)paramDesc, srcNode, config);
	if(dynamic_cast<const UintParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const UintParamDesc&)paramDesc, srcNode, config);
	if(dynamic_cast<const FloatParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const FloatParamDesc&)paramDesc, srcNode, config);
	if(dynamic_cast<const StringParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const StringParamDesc&)paramDesc, srcNode, config);
	if(dynamic_cast<const GameTimeParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const GameTimeParamDesc&)paramDesc, srcNode, config);
	if(dynamic_cast<const Vec2ParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const Vec2ParamDesc&)paramDesc, srcNode, config);
	if(dynamic_cast<const Vec3ParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const Vec3ParamDesc&)paramDesc, srcNode, config);
	if(dynamic_cast<const Vec4ParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const Vec4ParamDesc&)paramDesc, srcNode, config);
	if(dynamic_cast<const StructParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const StructParamDesc&)paramDesc, srcNode, config);
	if(dynamic_cast<const FixedSizeArrayParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const FixedSizeArrayParamDesc&)paramDesc, srcNode, config);
	// ADD NEW PARAMETER TYPES HERE.
	
	assert(!"Unsupported parameter type.");
	return false;
}

bool LoadObjFromTokDoc(void* dstObj, const StructDesc& structDesc, const common::tokdoc::Node& srcNode, const STokDocLoadConfig& config)
{
	const StructDesc* baseStructDesc = structDesc.GetBaseStructDesc();
	bool allOk = true;
	if(baseStructDesc)
		allOk = LoadObjFromTokDoc(dstObj, *baseStructDesc, srcNode, config);

	for(size_t i = 0, count = structDesc.Params.size(); i < count; ++i)
	{
		common::tokdoc::Node* subNode = srcNode.FindFirstChild(structDesc.Names[i]);
		if(subNode)
		{
			ERR_TRY;
			if(!LoadParamFromTokDoc(
				structDesc.AccessRawParam(dstObj, i),
				*structDesc.Params[i],
				*subNode,
				config))
			{
				allOk = false;
				if(config.WarningPrinter)
					config.WarningPrinter->printf(L"RegScript2 TokDoc parameter \"%s\" loading failed.", structDesc.Names[i].c_str());
			}
			ERR_CATCH(L"RegScript2 TokDoc parameter: " + structDesc.Names[i]);
		}
		else
		{
			if(IsFlagOptional(config.Flags))
			{
				if((config.Flags & TOKDOC_FLAG_DEFAULT))
					structDesc.SetParamToDefault(dstObj, i);
				if(config.WarningPrinter)
					config.WarningPrinter->printf(L"RegScript2 TokDoc parameter \"%s\" not found.", structDesc.Names[i].c_str());
				allOk = false;
			}
			else
				throw common::Error(L"Parameter not found.", __TFILE__, __LINE__);
		}
	}
	return allOk;
}

} // namespace RegScript2
