#include "Include/RegScript2.hpp"

namespace RegScript2
{

////////////////////////////////////////////////////////////////////////////////
// class ClassDesc


void StructDesc::SetParamToDefault(void* obj, size_t paramIndex) const
{
	Params[paramIndex]->SetToDefault(AccessRawParam(obj, paramIndex));
}

void StructDesc::CopyParam(void* dstObj, const void* srcObj, size_t paramIndex) const
{
	Params[paramIndex]->Copy(AccessRawParam(dstObj, paramIndex), AccessRawParam(srcObj, paramIndex));
}

void StructDesc::SetObjToDefault(void* obj) const
{
	if(m_BaseStructDesc)
		m_BaseStructDesc->SetObjToDefault(obj);
	for(size_t i = 0, count = Params.size(); i < count; ++i)
		SetParamToDefault(obj, i);
}

void StructDesc::CopyObj(void* dstObj, const void* srcObj) const
{
	if(m_BaseStructDesc)
		m_BaseStructDesc->CopyObj(dstObj, srcObj);
	for(size_t i = 0, count = Params.size(); i < count; ++i)
		CopyParam(dstObj, srcObj, i);
}

size_t StructDesc::Find(const wchar_t* name) const
{
	for(size_t i = 0, count = Names.size(); i < count; ++i)
		if(Names[i] == name)
			return i;
	return (size_t)-1;
}

ParamDesc* StructDesc::GetParamDesc(size_t index)
{
	assert(index < Params.size());
	return Params[index].get();
}

const ParamDesc* StructDesc::GetParamDesc(size_t index) const
{
	assert(index < Params.size());
	return Params[index].get();
}

////////////////////////////////////////////////////////////////////////////////
// class FixedSizeArrayParamDesc

void* FixedSizeArrayParamDesc::AccessElement(void* param, size_t elementIndex) const
{
	size_t elementSize = m_ElementParamDesc->GetParamSize();
	return (char*)param + elementIndex * elementSize;
}

const void* FixedSizeArrayParamDesc::AccessElement(const void* param, size_t elementIndex) const
{
	size_t elementSize = m_ElementParamDesc->GetParamSize();
	return (const char*)param + elementIndex * elementSize;
}

void FixedSizeArrayParamDesc::SetToDefault(void* param) const
{
	char* element = (char*)param;
	size_t elementSize = m_ElementParamDesc->GetParamSize();
	for(size_t i = 0; i < m_Count; ++i)
	{
		m_ElementParamDesc->SetToDefault(element);
		element += elementSize;
	}
}

void FixedSizeArrayParamDesc::Copy(void* dstParam, const void* srcParam) const
{
	char* dstElement = (char*)dstParam;
	const char* srcElement = (const char*)srcParam;
	size_t elementSize = m_ElementParamDesc->GetParamSize();
	for(size_t i = 0; i < m_Count; ++i)
	{
		m_ElementParamDesc->Copy(dstElement, srcElement);
		dstElement += elementSize;
		srcElement += elementSize;
	}
}

void FixedSizeArrayParamDesc::SetElementToDefault(void* param, size_t index) const
{
	size_t elementSize = m_ElementParamDesc->GetParamSize();
	size_t elementOffset = index * elementSize;
	char* element = (char*)param + elementOffset;
	m_ElementParamDesc->SetToDefault(element);
}

void FixedSizeArrayParamDesc::CopyElement(void* dstParam, const void* srcParam, size_t index) const
{
	size_t elementSize = m_ElementParamDesc->GetParamSize();
	size_t elementOffset = index * elementSize;
	char* dstElement = (char*)dstParam + elementOffset;
	const char* srcElement = (const char*)srcParam + elementOffset;
	m_ElementParamDesc->Copy(dstElement, srcElement);
}

////////////////////////////////////////////////////////////////////////////////
// class BoolParamDesc

bool BoolParamDesc::ToString(wstring& out, const void* srcParam) const
{
	const Param_t* param = (const Param_t*)srcParam;
	SthToStr<bool>(&out, param->Value);
	return true;
}

bool BoolParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	Param_t* param = (Param_t*)dstParam;
	return StrToSth<bool>(&param->Value, src);
}

////////////////////////////////////////////////////////////////////////////////
// class UintParamDesc

bool UintParamDesc::ToString(wstring& out, const void* srcParam) const
{
	const Param_t* param = (const Param_t*)srcParam;
	switch(Format)
	{
	case FORMAT_DEC:
		common::UintToStr(&out, param->Value, 10);
		break;
	case FORMAT_HEX:
		common::UintToStr(&out, param->Value, 16);
		out.insert(0, L"0x");
		break;
	default:
		assert(0);
		return false;
	}
	return true;
}

bool UintParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	Param_t* param = (Param_t*)dstParam;
	return StrToUint_AutoBase(param->Value, src);
}

////////////////////////////////////////////////////////////////////////////////
// class FloatParamDesc

bool FloatParamDesc::ToString(wstring& out, const void* srcParam) const
{
	const FloatParam* floatParam = (const FloatParam*)srcParam;
	const float value = floatParam->Value;
	if(isfinite(value))
	{
		if(Format == FORMAT_PERCENT)
		{
			SthToStr<float>(&out, value * 100.f);
			out += L'%';
			return true;
		}
		if(Format == FORMAT_DB && value > 0.f)
		{
			SthToStr<float>(&out, PowerToDB(value));
			out += L"dB";
			return true;
		}
	}
	SthToStr<float>(&out, value);
	return true;
}

bool FloatParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	Param_t* param = (Param_t*)dstParam;
	if(common::StrEnds(src, L"%", true))
	{
		wstring newStr { src, src + wcslen(src) - 1 };
		float val;
		if(StrToSth<float>(&val, newStr))
		{
			param->Value = val * 0.01f;
			return true;
		}
		else
			return false;
	}
	else if(common::StrEnds(src, L"dB", false))
	{
		wstring newStr(src, src + wcslen(src) - 2);
		float val;
		if(StrToSth<float>(&val, newStr))
		{
			param->Value = DBToPower(val);
			return true;
		}
		else
			return false;
	}
	else
		return StrToSth<float>(&param->Value, src);
}

////////////////////////////////////////////////////////////////////////////////
// class StringParamDesc

bool StringParamDesc::ToString(wstring& out, const void* srcParam) const
{
	const StringParam* stringParam = (const StringParam*)srcParam;
	out = stringParam->Value;
	return true;
}

bool StringParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	Param_t* param = (Param_t*)dstParam;
	param->Value = src;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// class GameTimeParamDesc

bool GameTimeParamDesc::ToString(wstring& out, const void* srcParam) const
{
	const GameTimeParam* gameTimeParam = (const GameTimeParam*)srcParam;
	GameTimeToFriendlyStr(out, gameTimeParam->Value);
	return true;
}

bool GameTimeParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	Param_t* param = (Param_t*)dstParam;
	return FriendlyStrToGameTime(param->Value, src);
}

////////////////////////////////////////////////////////////////////////////////
// class Vec2ParamDesc

bool Vec2ParamDesc::ToString(wstring& out, const void* srcParam) const
{
	const Param_t* param = (const Param_t*)srcParam;
	SthToStr<common::VEC2>(&out, param->Value);
	return true;
}

bool Vec2ParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	Param_t* param = (Param_t*)dstParam;
	return StrToSth<common::VEC2>(&param->Value, src);
}

////////////////////////////////////////////////////////////////////////////////
// class Vec3ParamDesc

bool Vec3ParamDesc::ToString(wstring& out, const void* srcParam) const
{
	const Param_t* param = (const Param_t*)srcParam;
	SthToStr<common::VEC3>(&out, param->Value);
	return true;
}

bool Vec3ParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	Param_t* param = (Param_t*)dstParam;
	return StrToSth<common::VEC3>(&param->Value, src);
}

////////////////////////////////////////////////////////////////////////////////
// class Vec2ParamDesc

bool Vec4ParamDesc::ToString(wstring& out, const void* srcParam) const
{
	const Param_t* param = (const Param_t*)srcParam;
	SthToStr<common::VEC4>(&out, param->Value);
	return true;
}

bool Vec4ParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	Param_t* param = (Param_t*)dstParam;
	return StrToSth<common::VEC4>(&param->Value, src);
}

////////////////////////////////////////////////////////////////////////////////
// Globals

/*
This function parses following syntax (and combinations):

	ParamName
	ParamName\ParamName
	ParamName[ElementIndex]

It uses following smart algorithm:

In any moment we are either pointing at object (currObj and currStructDesc !=
null) or at parameter (outParam and outParamDesc != null).

ParamName - Enters parameter of current object.
\ - Enters object of current parameter.
[ElementIndex] - Enters element parameter of current parameter.
*/
bool FindObjParamByPath(
	void*& outParam, const ParamDesc*& outParamDesc,
	void* obj, const StructDesc& structDesc,
	const wchar_t* path)
{
	void* currObj = obj;
	const StructDesc* currStructDesc = &structDesc;
	outParam = nullptr;
	outParamDesc = nullptr;
	wstring pathStr{path};
	size_t pathIndex = 0;
	while(pathIndex < pathStr.length())
	{
		// [ElementIndex]
		if(pathStr[pathIndex] == L'[')
		{
			if(outParam == nullptr)
				return false;
			if(typeid(FixedSizeArrayParamDesc) != typeid(*outParamDesc))
				return false;
			const FixedSizeArrayParamDesc* fixedSizeArrayParamDesc = (const FixedSizeArrayParamDesc*)outParamDesc;
			size_t closingBracketIndex = pathStr.find(L']', pathIndex + 1);
			if(closingBracketIndex == wstring::npos)
				return false;
			wstring indexStr{pathStr, pathIndex + 1, closingBracketIndex - pathIndex - 1};
			size_t elementIndex = 0;
			if(common::StrToUint(&elementIndex, indexStr) != 0)
				return false;
			if(elementIndex >= fixedSizeArrayParamDesc->GetCount())
				return false;
			outParam = fixedSizeArrayParamDesc->AccessElement(outParam, elementIndex);
			outParamDesc = fixedSizeArrayParamDesc->GetElementParamDesc();
			pathIndex = closingBracketIndex + 1;
		}
		else if(pathStr[pathIndex] == L'\\')
		{
			if(outParam == nullptr)
				return false;
			if(typeid(StructParamDesc) != typeid(*outParamDesc))
				return false;
			const StructParamDesc* structParamDesc = (const StructParamDesc*)outParamDesc;
			currObj = outParam;
			currStructDesc = structParamDesc->GetStructDesc();
			outParam = nullptr;
			outParamDesc = nullptr;
			++pathIndex;
		}
		// ParamName
		else
		{
			if(currObj == nullptr)
				return false;
			size_t endIndex = pathStr.find_first_of(L"\\[", pathIndex);
			wstring paramName = endIndex == wstring::npos ?
				pathStr.substr(pathIndex) :
				pathStr.substr(pathIndex, endIndex);
			size_t paramIndex = currStructDesc->Find(paramName.c_str());
			if(paramIndex == (size_t)-1)
				return false;
			outParamDesc = currStructDesc->GetParamDesc(paramIndex);
			outParam = currStructDesc->AccessRawParam(currObj, paramIndex);
			currObj = nullptr;
			currStructDesc = nullptr;
			pathIndex = endIndex;
		}
	}
	return outParam != nullptr;
}

} // namespace RegScript2
