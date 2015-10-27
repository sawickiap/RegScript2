#include "PCH.hpp"
#include "Include/RegScript2_TokDoc.hpp"

namespace RegScript2
{

void LoadParamFromTokDoc(void* dstParam, const BoolParamDesc& paramDesc, const common::tokdoc::Node& srcNode)
{
	BoolParam* boolParam = (BoolParam*)dstParam;
	if(!common::tokdoc::NodeTo(boolParam->Value, srcNode, /*TODO required*/false))
	{
		// TODO Configurable on error.
		paramDesc.SetToDefault(dstParam);
	}
}

void LoadParamFromTokDoc(void* dstParam, const UintParamDesc& paramDesc, const common::tokdoc::Node& srcNode)
{
	UintParam* uintParam = (UintParam*)dstParam;
	if(!common::tokdoc::NodeTo(uintParam->Value, srcNode, /*TODO required*/false))
	{
		// TODO Configurable on error.
		paramDesc.SetToDefault(dstParam);
	}
}

void LoadParamFromTokDoc(void* dstParam, const FloatParamDesc& paramDesc, const common::tokdoc::Node& srcNode)
{
	FloatParam* floatParam = (FloatParam*)dstParam;
	if(!common::tokdoc::NodeTo(floatParam->Value, srcNode, /*TODO required*/false))
	{
		// TODO Configurable on error.
		paramDesc.SetToDefault(dstParam);
	}
}

void LoadParamFromTokDoc(void* dstParam, const GameTimeParamDesc& paramDesc, const common::tokdoc::Node& srcNode)
{
	float seconds = 0.f;
	if(common::tokdoc::NodeTo(seconds, srcNode, /*TODO required*/false))
	{
		GameTimeParam* gameTimeParam = (GameTimeParam*)dstParam;
		gameTimeParam->Value = common::SecondsToGameTime(seconds);
	}
	else
	{
		// TODO Configurable on error.
		paramDesc.SetToDefault(dstParam);
	}
}

void LoadParamFromTokDoc(void* dstParam, const ClassParamDesc& paramDesc, const common::tokdoc::Node& srcNode)
{
	LoadObjFromTokDoc(dstParam, *paramDesc.GetClassDesc(), srcNode);
}

void LoadParamFromTokDoc(void* dstParam, const FixedSizeArrayParamDesc& paramDesc, const common::tokdoc::Node& srcNode)
{
	if(!srcNode.HasChildren())
	{
		// TODO Configurable on error.
		//PrintWarning(Format_r(L"Configuration array \"%s\" is empty.", subnodeName.c_str()).c_str());
		//return false;
		return;
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
		LoadParamFromTokDoc(dstElement, *elementParamDesc, *elementNode);
		elementNode = elementNode->GetNextSibling();
		++index;
		dstElement += elementSize;
	}
	if((elementNode == nullptr) != (index == elementCount))
	{
		// TODO Configurable on error.
		//PrintWarning(Format_r(L"Configuration array \"%s\" has invalid size.", subnodeName.c_str(), index).c_str());
		allOk = false;
	}
	// TODO Configurable on error.
	//return allOk;
}

void LoadParamFromTokDoc(void* dstParam, const ParamDesc& paramDesc, const common::tokdoc::Node& srcNode)
{
	if(dynamic_cast<const BoolParamDesc*>(&paramDesc))
		LoadParamFromTokDoc(dstParam, (const BoolParamDesc&)paramDesc, srcNode);
	else if(dynamic_cast<const UintParamDesc*>(&paramDesc))
		LoadParamFromTokDoc(dstParam, (const UintParamDesc&)paramDesc, srcNode);
	else if(dynamic_cast<const FloatParamDesc*>(&paramDesc))
		LoadParamFromTokDoc(dstParam, (const FloatParamDesc&)paramDesc, srcNode);
	else if(dynamic_cast<const GameTimeParamDesc*>(&paramDesc))
		LoadParamFromTokDoc(dstParam, (const GameTimeParamDesc&)paramDesc, srcNode);
	else if(dynamic_cast<const ClassParamDesc*>(&paramDesc))
		LoadParamFromTokDoc(dstParam, (const ClassParamDesc&)paramDesc, srcNode);
	else if(dynamic_cast<const FixedSizeArrayParamDesc*>(&paramDesc))
		LoadParamFromTokDoc(dstParam, (const FixedSizeArrayParamDesc&)paramDesc, srcNode);
	else
		assert(!"Unsupported parameter type.");
}

void LoadObjFromTokDoc(void* dstObj, const ClassDesc& classDesc, const common::tokdoc::Node& srcNode)
{
	const ClassDesc* baseClassDesc = classDesc.GetBaseClassDesc();
	if(baseClassDesc)
		LoadObjFromTokDoc(dstObj, *baseClassDesc, srcNode);

	for(size_t i = 0, count = classDesc.Params.size(); i < count; ++i)
	{
		common::tokdoc::Node* subNode = srcNode.FindFirstChild(classDesc.Names[i]);
		if(subNode)
		{
			LoadParamFromTokDoc(
				classDesc.AccessRawParam(dstObj, i),
				*classDesc.Params[i],
				*subNode);
		}
		else
		{
			// TODO Configurable on error.
			classDesc.SetParamToDefault(dstObj, i);
		}
	}
}

} // namespace RegScript2
