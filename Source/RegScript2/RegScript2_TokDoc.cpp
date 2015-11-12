#include "PCH.hpp"
#include "Include/RegScript2_TokDoc.hpp"

namespace RegScript2
{

static inline bool IsFlagOptional(uint32_t flags)
{
	return (flags & (TOKDOC_FLAG_OPTIONAL | TOKDOC_FLAG_OPTIONAL_CORRECT)) != 0;
}
static inline bool IsFlagRequired(uint32_t flags)
{
	return !IsFlagOptional(flags);
}

bool LoadParamFromTokDoc(void* dstParam, const BoolParamDesc& paramDesc, const common::tokdoc::Node& srcNode, uint32_t flags)
{
	BoolParam* boolParam = (BoolParam*)dstParam;
	if(common::tokdoc::NodeTo(boolParam->Value, srcNode, IsFlagRequired(flags)))
		return true;
	else
	{
		if((flags & TOKDOC_FLAG_DEFAULT))
			paramDesc.SetToDefault(dstParam);
		return false;
	}
}

bool LoadParamFromTokDoc(void* dstParam, const UintParamDesc& paramDesc, const common::tokdoc::Node& srcNode, uint32_t flags)
{
	UintParam* uintParam = (UintParam*)dstParam;
	if(common::tokdoc::NodeTo(uintParam->Value, srcNode, IsFlagRequired(flags)))
		return true;
	else
	{
		if((flags & TOKDOC_FLAG_DEFAULT))
			paramDesc.SetToDefault(dstParam);
		return false;
	}
}

bool LoadParamFromTokDoc(void* dstParam, const FloatParamDesc& paramDesc, const common::tokdoc::Node& srcNode, uint32_t flags)
{
	FloatParam* floatParam = (FloatParam*)dstParam;
	if(common::tokdoc::NodeTo(floatParam->Value, srcNode, IsFlagRequired(flags)))
		return true;
	else
	{
		if((flags & TOKDOC_FLAG_DEFAULT))
			paramDesc.SetToDefault(dstParam);
		return false;
	}
}

bool LoadParamFromTokDoc(void* dstParam, const GameTimeParamDesc& paramDesc, const common::tokdoc::Node& srcNode, uint32_t flags)
{
	float seconds = 0.f;
	if(common::tokdoc::NodeTo(seconds, srcNode, IsFlagRequired(flags)))
	{
		GameTimeParam* gameTimeParam = (GameTimeParam*)dstParam;
		gameTimeParam->Value = common::SecondsToGameTime(seconds);
		return true;
	}
	else
	{
		if((flags & TOKDOC_FLAG_DEFAULT))
			paramDesc.SetToDefault(dstParam);
		return false;
	}
}

bool LoadParamFromTokDoc(void* dstParam, const StructParamDesc& paramDesc, const common::tokdoc::Node& srcNode, uint32_t flags)
{
	return LoadObjFromTokDoc(dstParam, *paramDesc.GetStructDesc(), srcNode, flags);
}

bool LoadParamFromTokDoc(void* dstParam, const FixedSizeArrayParamDesc& paramDesc, const common::tokdoc::Node& srcNode, uint32_t flags)
{
	if(!srcNode.HasChildren())
	{
		if(!IsFlagOptional(flags))
			throw common::Error(L"Array parameter is empty.", __TFILE__, __LINE__);
		if((flags & TOKDOC_FLAG_DEFAULT))
			paramDesc.SetToDefault(dstParam);
		//PrintWarning(Format_r(L"Configuration array \"%s\" is empty.", subnodeName.c_str()).c_str());
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
		if(!LoadParamFromTokDoc(dstElement, *elementParamDesc, *elementNode, flags))
			allOk = false;
		elementNode = elementNode->GetNextSibling();
		++index;
		dstElement += elementSize;
	}
	if((elementNode == nullptr) != (index == elementCount))
	{
		if(!IsFlagOptional(flags))
			throw common::Error(L"Array parameter has invalid size.", __TFILE__, __LINE__);
		if((flags & TOKDOC_FLAG_DEFAULT))
		{
			for(; index < elementCount; ++index)
				paramDesc.SetElementToDefault(dstParam, index);
		}
		//PrintWarning(Format_r(L"Configuration array \"%s\" has invalid size.", subnodeName.c_str(), index).c_str());
		allOk = false;
	}
	return allOk;
}

// ADD NEW PARAMETER TYPES HERE.

bool LoadParamFromTokDoc(void* dstParam, const ParamDesc& paramDesc, const common::tokdoc::Node& srcNode, uint32_t flags)
{
	if(dynamic_cast<const BoolParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const BoolParamDesc&)paramDesc, srcNode, flags);
	if(dynamic_cast<const UintParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const UintParamDesc&)paramDesc, srcNode, flags);
	if(dynamic_cast<const FloatParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const FloatParamDesc&)paramDesc, srcNode, flags);
	if(dynamic_cast<const GameTimeParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const GameTimeParamDesc&)paramDesc, srcNode, flags);
	if(dynamic_cast<const StructParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const StructParamDesc&)paramDesc, srcNode, flags);
	if(dynamic_cast<const FixedSizeArrayParamDesc*>(&paramDesc))
		return LoadParamFromTokDoc(dstParam, (const FixedSizeArrayParamDesc&)paramDesc, srcNode, flags);
	// ADD NEW PARAMETER TYPES HERE.
	
	assert(!"Unsupported parameter type.");
	return false;
}

bool LoadObjFromTokDoc(void* dstObj, const StructDesc& structDesc, const common::tokdoc::Node& srcNode, uint32_t flags)
{
	const StructDesc* baseStructDesc = structDesc.GetBaseStructDesc();
	bool allOk = true;
	if(baseStructDesc)
		allOk = LoadObjFromTokDoc(dstObj, *baseStructDesc, srcNode, flags);

	for(size_t i = 0, count = structDesc.Params.size(); i < count; ++i)
	{
		ERR_TRY;

		common::tokdoc::Node* subNode = srcNode.FindFirstChild(structDesc.Names[i]);
		if(subNode)
		{
			if(!LoadParamFromTokDoc(
				structDesc.AccessRawParam(dstObj, i),
				*structDesc.Params[i],
				*subNode,
				flags))
			{
				allOk = false;
			}
		}
		else
		{
			if(IsFlagOptional(flags))
			{
				if((flags & TOKDOC_FLAG_DEFAULT))
					structDesc.SetParamToDefault(dstObj, i);
				allOk = false;
			}
			else
				throw common::Error(L"Parameter not found.", __TFILE__, __LINE__);
		}

		ERR_CATCH(L"RegScript2 TokDoc parameter: " + structDesc.Names[i]);
	}
	return allOk;
}

} // namespace RegScript2
