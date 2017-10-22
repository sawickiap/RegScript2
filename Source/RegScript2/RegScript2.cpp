#include "Include/RegScript2.hpp"

namespace RegScript2
{

const wchar_t* const ERR_MSG_VALUE_NOT_CONST = L"Value is not constant.";
static const wchar_t* const ERR_MSG_PARAM_READ_ONLY = L"Parameter is read-only.";
static const wchar_t* const ERR_MSG_PARAM_WRITE_ONLY = L"Paramter is write-only.";
static const wchar_t* const ERR_MSG_CANNOT_SET_VALUE = L"Cannot set parameter value.";

struct StorageFunction { };
StorageFunction storageFunction;

////////////////////////////////////////////////////////////////////////////////
// class EnumDesc

EnumDesc::EnumDesc(
	const wchar_t* name,
	size_t itemCount,
	const wchar_t* const* itemNames,
	const int32_t* itemValues) :
	Name(name),
	ItemCount(itemCount),
	ItemNames(itemNames),
	ItemValues(itemValues)
{
	assert(!common::StrIsEmpty(name));
	assert(itemCount);
	assert(itemNames);
	for(size_t i = 0; i < itemCount; ++i)
	{
		assert(!common::StrIsEmpty(itemNames[i]));
	}
}

size_t EnumDesc::FindItemByName(const wchar_t* name, bool caseSensitive) const
{
	for(size_t i = 0; i < ItemCount; ++i)
	{
		if((caseSensitive && wcscmp(name, ItemNames[i]) == 0) ||
			(!caseSensitive && _wcsicmp(name, ItemNames[i]) == 0))
		{
			return i;
		}
	}
	return INVALID_INDEX;
}

size_t EnumDesc::FindItemByValue(int32_t value) const
{
	if(ItemValues)
	{
		for(size_t i = 0; i < ItemCount; ++i)
			if(value == ItemValues[i])
				return i;
		return INVALID_INDEX;
	}
	else
	{
		if(value < 0)
			return INVALID_INDEX;
		size_t index = (size_t)value;
		if(index < ItemCount)
			return index;
		else
			return INVALID_INDEX;
	}
}

void EnumDesc::ValueToStr(std::wstring& out, int32_t value) const
{
	size_t index = FindItemByValue(value);
	if(index != INVALID_INDEX)
		out = ItemNames[index];
	else
		common::IntToStr(&out, value);
}

bool EnumDesc::StrToValue(int32_t& out, const wchar_t* str, bool caseSensitive, bool allowInteger) const
{
	size_t index = FindItemByName(str, caseSensitive);
	if(index != INVALID_INDEX)
	{
		out = GetValue(index);
		return true;
	}
	else
	{
		if(allowInteger)
			return common::StrToInt(&out, str) == 0;
		else
			return false;
	}
}

////////////////////////////////////////////////////////////////////////////////
// class BoolParam

bool BoolParam::GetConst() const
{
	bool value;
	if(TryGetConst(value))
		return value;
	else
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

void BoolParam::SetConst(bool value)
{
	// TODO
	m_ValueType = VALUE_TYPE::CONSTANT;
	m_Value = value;
}

////////////////////////////////////////////////////////////////////////////////
// class IntParam

int32_t IntParam::GetConst() const
{
	int32_t value;
	if(TryGetConst(value))
		return value;
	else
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

void IntParam::SetConst(int32_t value)
{
	// TODO
	m_ValueType = VALUE_TYPE::CONSTANT;
	m_Value = value;
}

////////////////////////////////////////////////////////////////////////////////
// class UintParam

uint32_t UintParam::GetConst() const
{
	uint32_t value;
	if(TryGetConst(value))
		return value;
	else
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

void UintParam::SetConst(uint32_t value)
{
	// TODO
	m_ValueType = VALUE_TYPE::CONSTANT;
	m_Value = value;
}

////////////////////////////////////////////////////////////////////////////////
// class EnumParam

int32_t EnumParam::GetConst() const
{
	int32_t value;
	if(TryGetConst(value))
		return value;
	else
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

void EnumParam::SetConst(int32_t value)
{
	// TODO
	m_ValueType = VALUE_TYPE::CONSTANT;
	m_Value = value;
}

////////////////////////////////////////////////////////////////////////////////
// class FloatParam

float FloatParam::GetConst() const
{
	float value;
	if(TryGetConst(value))
		return value;
	else
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

void FloatParam::SetConst(float value)
{
	// TODO
	m_ValueType = VALUE_TYPE::CONSTANT;
	m_Value = value;
}

////////////////////////////////////////////////////////////////////////////////
// class StringParam

void StringParam::GetConst(std::wstring& outValue) const
{
	if(!TryGetConst(outValue))
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

const std::wstring* StringParam::AccessConst() const
{
    assert(IsConst());
    return &m_Value;
}

void StringParam::SetConst(const wchar_t* value, size_t valueLen)
{
	// TODO
	m_ValueType = VALUE_TYPE::CONSTANT;
	m_Value.assign(value, value + valueLen);
}

void StringParam::SetConst(const wchar_t* value)
{
	// TODO
	m_ValueType = VALUE_TYPE::CONSTANT;
	m_Value = value;
}

////////////////////////////////////////////////////////////////////////////////
// class GameTimeParam

common::GameTime GameTimeParam::GetConst() const
{
	common::GameTime value;
	if(TryGetConst(value))
		return value;
	else
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

void GameTimeParam::SetConst(common::GameTime value)
{
	// TODO
	m_ValueType = VALUE_TYPE::CONSTANT;
	m_Value = value;
}

////////////////////////////////////////////////////////////////////////////////
// template class VecParam

template class VecParam<common::VEC2>;
template class VecParam<common::VEC3>;
template class VecParam<common::VEC4>;

template<typename Vec_t>
void VecParam<Vec_t>::GetConst(Vec_t& outValue) const
{
	if(!TryGetConst(outValue))
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

template<typename Vec_t>
void VecParam<Vec_t>::SetConst(const Vec_t& value)
{
	// TODO
	m_ValueType = VALUE_TYPE::CONSTANT;
	m_Value = value;
}

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
    {
        if(Params[i]->CanWrite())
		    SetParamToDefault(obj, i);
    }
}

void StructDesc::CopyObj(void* dstObj, const void* srcObj) const
{
	if(m_BaseStructDesc)
		m_BaseStructDesc->CopyObj(dstObj, srcObj);
	for(size_t i = 0, count = Params.size(); i < count; ++i)
		CopyParam(dstObj, srcObj, i);
}

size_t StructDesc::Find(const wchar_t* name, bool caseSensitive) const
{
	for(size_t i = 0, count = Names.size(); i < count; ++i)
	{
		if((caseSensitive && Names[i] == name) ||
			(!caseSensitive && _wcsicmp(Names[i].c_str(), name) == 0))
		{
			return i;
		}
	}
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
// class ParamDesc

void ParamDesc::CheckCanWrite() const
{
	if(!CanWrite())
		throw common::Error(ERR_MSG_PARAM_READ_ONLY, __TFILE__, __LINE__);
}

void ParamDesc::CheckCanRead() const
{
	if(!CanRead())
		throw common::Error(ERR_MSG_PARAM_WRITE_ONLY, __TFILE__, __LINE__);
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
	CheckCanWrite();

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
	CheckCanRead();
	CheckCanWrite();

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
	CheckCanWrite();

	size_t elementSize = m_ElementParamDesc->GetParamSize();
	size_t elementOffset = index * elementSize;
	char* element = (char*)param + elementOffset;
	m_ElementParamDesc->SetToDefault(element);
}

void FixedSizeArrayParamDesc::CopyElement(void* dstParam, const void* srcParam, size_t index) const
{
	CheckCanRead();
	CheckCanWrite();

	size_t elementSize = m_ElementParamDesc->GetParamSize();
	size_t elementOffset = index * elementSize;
	char* dstElement = (char*)dstParam + elementOffset;
	const char* srcElement = (const char*)srcParam + elementOffset;
	m_ElementParamDesc->Copy(dstElement, srcElement);
}

////////////////////////////////////////////////////////////////////////////////
// class BoolParamDesc

size_t BoolParamDesc::GetParamSize() const
{
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return sizeof(Value_t);
	case STORAGE::PARAM:
		return sizeof(Param_t);
	default:
		return 0;
	}
}

bool BoolParamDesc::IsConst(const void* param) const
{
	if(!CanRead())
		return false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return true;
	case STORAGE::PARAM:
		return AccessAsParam(param)->IsConst();
	default:
		assert(0);
		return true;
	}
}

bool BoolParamDesc::TryGetConst(Value_t& outValue, const void* param) const
{
	if(!CanRead())
		return false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		outValue = *AccessAsRaw(param);
		return true;
	case STORAGE::PARAM:
		return AccessAsParam(param)->TryGetConst(outValue);
	case STORAGE::FUNCTION:
		return GetFunc(outValue, param);
	default:
		assert(0);
		return false;
	}
}

BoolParamDesc::Value_t BoolParamDesc::GetConst(const void* param) const
{
	Value_t value;
	if(TryGetConst(value, param))
		return value;
	else
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

bool BoolParamDesc::TrySetConst(void* param, Value_t value) const
{
	if(!CanWrite())
		return false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(param) = value;
		break;
	case STORAGE::PARAM:
		AccessAsParam(param)->SetConst(value);
		break;
	case STORAGE::FUNCTION:
		return SetFunc(param, value);
		break;
	default:
		assert(0);
	}
	return true;
}

void BoolParamDesc::SetConst(void* param, Value_t value) const
{
	if(!TrySetConst(param, value))
		throw common::Error(ERR_MSG_CANNOT_SET_VALUE, __TFILE__, __LINE__);
}

void BoolParamDesc::Copy(void* dstParam, const void* srcParam) const
{
	CheckCanRead();
	CheckCanWrite();
	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(dstParam) = *AccessAsRaw(srcParam);
		break;
	case STORAGE::PARAM:
		*AccessAsParam(dstParam) = *AccessAsParam(srcParam);
		break;
	case STORAGE::FUNCTION:
		SetConst(dstParam, GetConst(srcParam));
		break;
	default:
		assert(0);
	}
}

bool BoolParamDesc::ToString(std::wstring& out, const void* srcParam) const
{
	Value_t value;
	if(TryGetConst(value, srcParam))
	{
		SthToStr<bool>(&out, value);
		return true;
	}
	else
		return false;
}

bool BoolParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	Value_t value;
	if(StrToSth<bool>(&value, src))
		return TrySetConst(dstParam, value);
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////
// class IntParamDesc

size_t IntParamDesc::GetParamSize() const
{
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return sizeof(Value_t);
	case STORAGE::PARAM:
		return sizeof(Param_t);
	default:
		return 0;
	}
}

bool IntParamDesc::IsConst(const void* param) const
{
	if(!CanRead())
		return false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return true;
	case STORAGE::PARAM:
		return AccessAsParam(param)->IsConst();
	default:
		assert(0);
		return true;
	}
}

bool IntParamDesc::TryGetConst(Value_t& outValue, const void* param) const
{
	if(!CanRead())
		return false;
	bool ok = false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		outValue = *AccessAsRaw(param);
		ok = true;
		break;
	case STORAGE::PARAM:
		ok = AccessAsParam(param)->TryGetConst(outValue);
		break;
	case STORAGE::FUNCTION:
		ok = GetFunc(outValue, param);
		break;
	default:
		assert(0);
	}
	if(ok && (Flags & FLAG_MINMAX_CLAMP_ON_GET))
		ClampValueToMinMax(outValue);
	return ok;
}

IntParamDesc::Value_t IntParamDesc::GetConst(const void* param) const
{
	Value_t value;
	if(TryGetConst(value, param))
		return value;
	else
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

bool IntParamDesc::TrySetConst(void* param, Value_t value) const
{
	if(!CanWrite())
		return false;
	if((Flags & FLAG_MINMAX_FAIL_ON_SET) && !ValueInMinMax(value))
		return false;
	else if(Flags & FLAG_MINMAX_CLAMP_ON_SET)
		ClampValueToMinMax(value);
	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(param) = value;
		break;
	case STORAGE::PARAM:
		AccessAsParam(param)->SetConst(value);
		break;
	case STORAGE::FUNCTION:
		return SetFunc(param, value);
		break;
	default:
		assert(0);
	}
	return true;
}

void IntParamDesc::SetConst(void* param, Value_t value) const
{
	if(!TrySetConst(param, value))
		throw common::Error(ERR_MSG_CANNOT_SET_VALUE, __TFILE__, __LINE__);
}

void IntParamDesc::Copy(void* dstParam, const void* srcParam) const
{
	CheckCanRead();
	CheckCanWrite();

	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(dstParam) = *AccessAsRaw(srcParam);
		break;
	case STORAGE::PARAM:
		*AccessAsParam(dstParam) = *AccessAsParam(srcParam);
		break;
	case STORAGE::FUNCTION:
		SetConst(dstParam, GetConst(srcParam));
		break;
	default:
		assert(0);
	}
}

bool IntParamDesc::ToString(std::wstring& out, const void* srcParam) const
{
	Value_t value;
	if(TryGetConst(value, srcParam))
	{
		common::IntToStr(&out, value);
		return true;
	}
	else
		return false;
}

bool IntParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	Value_t value;
	if(common::StrToInt(&value, src) == 0)
		return TrySetConst(dstParam, value);
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////
// class UintParamDesc

size_t UintParamDesc::GetParamSize() const
{
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return sizeof(Value_t);
	case STORAGE::PARAM:
		return sizeof(Param_t);
	default:
		return 0;
	}
}

bool UintParamDesc::IsConst(const void* param) const
{
	if(!CanRead())
		return false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return true;
	case STORAGE::PARAM:
		return AccessAsParam(param)->IsConst();
	default:
		assert(0);
		return true;
	}
}

bool UintParamDesc::TryGetConst(Value_t& outValue, const void* param) const
{
	if(!CanRead())
		return false;
	bool ok = false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		outValue = *AccessAsRaw(param);
		ok = true;
		break;
	case STORAGE::PARAM:
		ok = AccessAsParam(param)->TryGetConst(outValue);
		break;
	case STORAGE::FUNCTION:
		ok = GetFunc(outValue, param);
		break;
	default:
		assert(0);
	}
	if(ok && (Flags & FLAG_MINMAX_CLAMP_ON_GET))
		ClampValueToMinMax(outValue);
	return ok;
}

UintParamDesc::Value_t UintParamDesc::GetConst(const void* param) const
{
	Value_t value;
	if(TryGetConst(value, param))
		return value;
	else
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

bool UintParamDesc::TrySetConst(void* param, Value_t value) const
{
	if(!CanWrite())
		return false;
	if((Flags & FLAG_MINMAX_FAIL_ON_SET) && !ValueInMinMax(value))
		return false;
	else if(Flags & FLAG_MINMAX_CLAMP_ON_SET)
		ClampValueToMinMax(value);
	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(param) = value;
		break;
	case STORAGE::PARAM:
		AccessAsParam(param)->SetConst(value);
		break;
	case STORAGE::FUNCTION:
		return SetFunc(param, value);
		break;
	default:
		assert(0);
	}
	return true;
}

void UintParamDesc::SetConst(void* param, Value_t value) const
{
	if(!TrySetConst(param, value))
		throw common::Error(ERR_MSG_CANNOT_SET_VALUE, __TFILE__, __LINE__);
}

void UintParamDesc::Copy(void* dstParam, const void* srcParam) const
{
	CheckCanRead();
	CheckCanWrite();

	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(dstParam) = *AccessAsRaw(srcParam);
		break;
	case STORAGE::PARAM:
		*AccessAsParam(dstParam) = *AccessAsParam(srcParam);
		break;
	case STORAGE::FUNCTION:
		SetConst(dstParam, GetConst(srcParam));
		break;
	default:
		assert(0);
	}
}

bool UintParamDesc::ToString(std::wstring& out, const void* srcParam) const
{
	Value_t value;
	if(TryGetConst(value, srcParam))
	{
		if(Flags & FLAG_FORMAT_HEX)
		{
			common::UintToStr(&out, value, 16);
			out.insert(0, L"0x");
		}
		else
			common::UintToStr(&out, value, 10);
		return true;
	}
	else
		return false;
}

bool UintParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	Value_t value;
	if(StrToUint_AutoBase<Value_t>(value, src))
		return TrySetConst(dstParam, value);
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////
// class EnumParamDesc

size_t EnumParamDesc::GetParamSize() const
{
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return sizeof(Value_t);
	case STORAGE::PARAM:
		return sizeof(Param_t);
	default:
		return 0;
	}
}

bool EnumParamDesc::IsConst(const void* param) const
{
	if(!CanRead())
		return false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return true;
	case STORAGE::PARAM:
		return AccessAsParam(param)->IsConst();
	default:
		assert(0);
		return true;
	}
}

bool EnumParamDesc::TryGetConst(Value_t& outValue, const void* param) const
{
	if(!CanRead())
		return false;
	bool ok = false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		outValue = *AccessAsRaw(param);
		ok = true;
		break;
	case STORAGE::PARAM:
		ok = AccessAsParam(param)->TryGetConst(outValue);
		break;
	case STORAGE::FUNCTION:
		ok = GetFunc(outValue, param);
		break;
	default:
		assert(0);
	}
	return ok;
}

EnumParamDesc::Value_t EnumParamDesc::GetConst(const void* param) const
{
	Value_t value;
	if(TryGetConst(value, param))
		return value;
	else
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

bool EnumParamDesc::TrySetConst(void* param, Value_t value) const
{
	if(!CanWrite())
		return false;
	if((Flags & FLAG_MINMAX_FAIL_ON_SET) && m_EnumDesc->ValueIsValid(value))
		return false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(param) = value;
		break;
	case STORAGE::PARAM:
		AccessAsParam(param)->SetConst(value);
		break;
	case STORAGE::FUNCTION:
		return SetFunc(param, value);
		break;
	default:
		assert(0);
	}
	return true;
}

void EnumParamDesc::SetConst(void* param, Value_t value) const
{
	if(!TrySetConst(param, value))
		throw common::Error(ERR_MSG_CANNOT_SET_VALUE, __TFILE__, __LINE__);
}

void EnumParamDesc::Copy(void* dstParam, const void* srcParam) const
{
	CheckCanRead();
	CheckCanWrite();

	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(dstParam) = *AccessAsRaw(srcParam);
		break;
	case STORAGE::PARAM:
		*AccessAsParam(dstParam) = *AccessAsParam(srcParam);
		break;
	case STORAGE::FUNCTION:
		SetConst(dstParam, GetConst(srcParam));
		break;
	default:
		assert(0);
	}
}

bool EnumParamDesc::ToString(std::wstring& out, const void* srcParam) const
{
	Value_t value;
	if(TryGetConst(value, srcParam))
	{
		m_EnumDesc->ValueToStr(out, value);
		return true;
	}
	else
		return false;
}

bool EnumParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	Value_t value;
	if(m_EnumDesc->StrToValue(value, src, true, !(Flags & FLAG_MINMAX_FAIL_ON_SET)))
		return TrySetConst(dstParam, value);
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////
// class FloatParamDesc

size_t FloatParamDesc::GetParamSize() const
{
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return sizeof(Value_t);
	case STORAGE::PARAM:
		return sizeof(Param_t);
	default:
		return 0;
	}
}

bool FloatParamDesc::IsConst(const void* param) const
{
	if(!CanRead())
		return false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return true;
	case STORAGE::PARAM:
		return AccessAsParam(param)->IsConst();
	default:
		assert(0);
		return true;
	}
}

bool FloatParamDesc::TryGetConst(Value_t& outValue, const void* param) const
{
	if(!CanRead())
		return false;
	bool ok = false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		outValue = *AccessAsRaw(param);
		ok = true;
		break;
	case STORAGE::PARAM:
		ok = AccessAsParam(param)->TryGetConst(outValue);
		break;
	case STORAGE::FUNCTION:
		ok = GetFunc(outValue, param);
		break;
	default:
		assert(0);
	}
	if(ok && (Flags & FLAG_MINMAX_CLAMP_ON_GET))
		ClampValueToMinMax(outValue);
	return ok;
}

FloatParamDesc::Value_t FloatParamDesc::GetConst(const void* param) const
{
	Value_t value;
	if(TryGetConst(value, param))
		return value;
	else
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

bool FloatParamDesc::TrySetConst(void* param, Value_t value) const
{
	if(!CanWrite())
		return false;
	if((Flags & FLAG_MINMAX_FAIL_ON_SET) && !ValueInMinMax(value))
		return false;
	else if(Flags & FLAG_MINMAX_CLAMP_ON_SET)
		ClampValueToMinMax(value);
	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(param) = value;
		break;
	case STORAGE::PARAM:
		AccessAsParam(param)->SetConst(value);
		break;
	case STORAGE::FUNCTION:
		return SetFunc(param, value);
		break;
	default:
		assert(0);
	}
	return true;
}

void FloatParamDesc::SetConst(void* param, Value_t value) const
{
	if(!TrySetConst(param, value))
		throw common::Error(ERR_MSG_CANNOT_SET_VALUE, __TFILE__, __LINE__);
}

void FloatParamDesc::Copy(void* dstParam, const void* srcParam) const
{
	CheckCanRead();
	CheckCanWrite();

	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(dstParam) = *AccessAsRaw(srcParam);
		break;
	case STORAGE::PARAM:
		*AccessAsParam(dstParam) = *AccessAsParam(srcParam);
		break;
	case STORAGE::FUNCTION:
		SetConst(dstParam, GetConst(srcParam));
		break;
	default:
		assert(0);
	}
}

bool FloatParamDesc::ToString(std::wstring& out, const void* srcParam) const
{
	Value_t value;
	if(TryGetConst(value, srcParam))
	{
		if(isfinite(value))
		{
			if(Flags & FLAG_FORMAT_PERCENT)
			{
				ValueToStr(out, value * 100.f);
				out += L'%';
				return true;
			}
			else if (Flags & FLAG_FORMAT_DB && value > 0.f)
			{
				ValueToStr(out, PowerToDB(value));
				out += L"dB";
				return true;
			}
            else if(Flags & FLAG_FORMAT_DEG)
            {
                ValueToStr(out, common::RadToDeg(value));
                out += L"deg";
                return true;
            }
		}
		ValueToStr(out, value);
		return true;
	}
	else
		return false;
}

bool FloatParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	Value_t value;
	if(common::StrEnds(src, L"%", true))
	{
		wstring newStr { src, src + wcslen(src) - 1 };
		if(StrToSth<float>(&value, newStr))
			return TrySetConst(dstParam, value * 0.01f);
		else
			return false;
	}
	else if(common::StrEnds(src, L"dB", false))
	{
		wstring newStr(src, src + wcslen(src) - 2);
		if(StrToSth<float>(&value, newStr))
			return TrySetConst(dstParam, DBToPower(value));
		else
			return false;
	}
    else if(common::StrEnds(src, L"deg", false))
    {
        wstring newStr(src, src + wcslen(src) - 3);
        if(StrToSth<float>(&value, newStr))
            return TrySetConst(dstParam, common::DegToRad(value));
        else
            return false;
    }
	else
	{
		if(StrToSth<float>(&value, src))
			return TrySetConst(dstParam, value);
		else
			return false;
	}
}

void FloatParamDesc::ValueToStr(std::wstring& out, float value) const
{
	if(Precision == UINT_MAX)
		common::FloatToStr(&out, value, 'g');
	else
		common::FloatToStr(&out, value, 'f', (int)Precision);
}

////////////////////////////////////////////////////////////////////////////////
// class StringParamDesc

size_t StringParamDesc::GetParamSize() const
{
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return sizeof(Value_t);
	case STORAGE::PARAM:
		return sizeof(Param_t);
	default:
		return 0;
	}
}

bool StringParamDesc::IsConst(const void* param) const
{
	if(!CanRead())
		return false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return true;
	case STORAGE::PARAM:
		return AccessAsParam(param)->IsConst();
	default:
		assert(0);
		return true;
	}
}

bool StringParamDesc::TryGetConst(Value_t& outValue, const void* param) const
{
	if(!CanRead())
		return false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		outValue = *AccessAsRaw(param);
		return true;
	case STORAGE::PARAM:
		return AccessAsParam(param)->TryGetConst(outValue);
	case STORAGE::FUNCTION:
		return GetFunc(outValue, param);
	default:
		assert(0);
		return false;
	}
}

void StringParamDesc::GetConst(StringParamDesc::Value_t& outValue, const void* param) const
{
	if(!TryGetConst(outValue, param))
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

const StringParamDesc::Value_t* StringParamDesc::AccessConst(const void* param) const
{
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return AccessAsRaw(param);
	case STORAGE::PARAM:
		return AccessAsParam(param)->AccessConst();
	case STORAGE::FUNCTION:
	default:
		assert(0);
		return nullptr;
	}
}

bool StringParamDesc::TrySetConst(void* param, const wchar_t* value, size_t valueLen) const
{
	if(!CanWrite())
		return false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		AccessAsRaw(param)->assign(value, value + valueLen);
		break;
	case STORAGE::PARAM:
		AccessAsParam(param)->SetConst(value, valueLen);
		break;
	case STORAGE::FUNCTION:
		return SetFunc(param, std::wstring(value, value + valueLen));
		break;
	default:
		assert(0);
	}
	return true;
}

void StringParamDesc::SetConst(void* param, const wchar_t* value, size_t valueLen) const
{
	if(!TrySetConst(param, value, valueLen))
		throw common::Error(ERR_MSG_CANNOT_SET_VALUE, __TFILE__, __LINE__);
}

bool StringParamDesc::TrySetConst(void* param, const wchar_t* value) const
{
    return TrySetConst(param, value, wcslen(value));
}

void StringParamDesc::SetConst(void* param, const wchar_t* value) const
{
    SetConst(param, value, wcslen(value));
}

void StringParamDesc::Copy(void* dstParam, const void* srcParam) const
{
	CheckCanRead();
	CheckCanWrite();

	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(dstParam) = *AccessAsRaw(srcParam);
		break;
	case STORAGE::PARAM:
		*AccessAsParam(dstParam) = *AccessAsParam(srcParam);
		break;
	case STORAGE::FUNCTION:
		{
			Value_t value;
			GetConst(value, srcParam);
			SetConst(dstParam, value);
		}
		break;
	default:
		assert(0);
	}
}

bool StringParamDesc::ToString(std::wstring& out, const void* srcParam) const
{
	return TryGetConst(out, srcParam);
}

bool StringParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	return TrySetConst(dstParam, src);
}

////////////////////////////////////////////////////////////////////////////////
// class GameTimeParamDesc

size_t GameTimeParamDesc::GetParamSize() const
{
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return sizeof(Value_t);
	case STORAGE::PARAM:
		return sizeof(Param_t);
	default:
		return 0;
	}
}

bool GameTimeParamDesc::IsConst(const void* param) const
{
	if(!CanRead())
		return false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return true;
	case STORAGE::PARAM:
		return AccessAsParam(param)->IsConst();
	default:
		assert(0);
		return true;
	}
}

bool GameTimeParamDesc::TryGetConst(Value_t& outValue, const void* param) const
{
	if(!CanRead())
		return false;
	bool ok = false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		outValue = *AccessAsRaw(param);
		ok = true;
		break;
	case STORAGE::PARAM:
		ok = AccessAsParam(param)->TryGetConst(outValue);
		break;
	case STORAGE::FUNCTION:
		ok = GetFunc(outValue, param);
		break;
	default:
		assert(0);
	}
	if(ok && (Flags & FLAG_MINMAX_CLAMP_ON_GET))
		ClampValueToMinMax(outValue);
	return ok;
}

GameTimeParamDesc::Value_t GameTimeParamDesc::GetConst(const void* param) const
{
	Value_t value;
	if(TryGetConst(value, param))
		return value;
	else
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

bool GameTimeParamDesc::TrySetConst(void* param, Value_t value) const
{
	if(!CanWrite())
		return false;
	if((Flags & FLAG_MINMAX_FAIL_ON_SET) && !ValueInMinMax(value))
		return false;
	else if(Flags & FLAG_MINMAX_CLAMP_ON_SET)
		ClampValueToMinMax(value);
	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(param) = value;
		break;
	case STORAGE::PARAM:
		AccessAsParam(param)->SetConst(value);
		break;
	case STORAGE::FUNCTION:
		return SetFunc(param, value);
		break;
	default:
		assert(0);
	}
	return true;
}

void GameTimeParamDesc::SetConst(void* param, Value_t value) const
{
	if(!TrySetConst(param, value))
		throw common::Error(ERR_MSG_CANNOT_SET_VALUE, __TFILE__, __LINE__);
}

void GameTimeParamDesc::Copy(void* dstParam, const void* srcParam) const
{
	CheckCanRead();
	CheckCanWrite();
	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(dstParam) = *AccessAsRaw(srcParam);
		break;
	case STORAGE::PARAM:
		*AccessAsParam(dstParam) = *AccessAsParam(srcParam);
		break;
	case STORAGE::FUNCTION:
		SetConst(dstParam, GetConst(srcParam));
		break;
	default:
		assert(0);
	}
}

bool GameTimeParamDesc::ToString(std::wstring& out, const void* srcParam) const
{
	Value_t value;
	if(TryGetConst(value, srcParam))
	{
		GameTimeToFriendlyStr(out, value);
		return true;
	}
	else
		return false;
}

bool GameTimeParamDesc::Parse(void* dstParam, const wchar_t* src) const
{
	Value_t value;
	if(FriendlyStrToGameTime(value, src))
		return TrySetConst(dstParam, value);
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////
// template class VecParamDesc

template class VecParamDesc<common::VEC2>;
template class VecParamDesc<common::VEC3>;
template class VecParamDesc<common::VEC4>;

template<typename Vec_t>
size_t VecParamDesc<Vec_t>::GetParamSize() const
{
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return sizeof(Value_t);
	case STORAGE::PARAM:
		return sizeof(Param_t);
	default:
		return 0;
	}
}

template<typename Vec_t>
bool VecParamDesc<Vec_t>::ValueInMinMax(Value_t value) const
{
    Vec_t ref;
    Replicate(ref, this->MinValue);
    if(!common::AllGreaterEqual(value, ref))
        return false;
    Replicate(ref, this->MaxValue);
    if(!common::AllLessEqual(value, ref))
        return false;
    return true;
}

template<typename Vec_t>
void VecParamDesc<Vec_t>::ClampValueToMinMax(Value_t& value) const
{
    Vec_t ref;
    Replicate(ref, this->MinValue);
	common::Max(&value, value, ref);
    Replicate(ref, this->MaxValue);
	common::Min(&value, value, ref);
}

template<typename Vec_t>
bool VecParamDesc<Vec_t>::IsConst(const void* param) const
{
	if(!CanRead())
		return false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		return true;
	case STORAGE::PARAM:
		return AccessAsParam(param)->IsConst();
	default:
		assert(0);
		return true;
	}
}

template<typename Vec_t>
bool VecParamDesc<Vec_t>::TryGetConst(Value_t& outValue, const void* param) const
{
	if(!CanRead())
		return false;
	bool ok = false;
	switch(GetStorage())
	{
	case STORAGE::RAW:
		outValue = *AccessAsRaw(param);
		ok = true;
		break;
	case STORAGE::PARAM:
		ok = AccessAsParam(param)->TryGetConst(outValue);
		break;
	case STORAGE::FUNCTION:
		ok = GetFunc(outValue, param);
		break;
	default:
		assert(0);
	}
	if(ok && (Flags & FLAG_MINMAX_CLAMP_ON_GET))
		ClampValueToMinMax(outValue);
	return ok;
}

template<typename Vec_t>
void VecParamDesc<Vec_t>::GetConst(Value_t& outValue, const void* param) const
{
	if(!TryGetConst(outValue, param))
		throw common::Error(ERR_MSG_VALUE_NOT_CONST, __TFILE__, __LINE__);
}

template<typename Vec_t>
bool VecParamDesc<Vec_t>::TrySetConst(void* param, Value_t value) const
{
	if(!CanWrite())
		return false;
	if((Flags & FLAG_MINMAX_FAIL_ON_SET) && !ValueInMinMax(value))
		return false;
	else if(Flags & FLAG_MINMAX_CLAMP_ON_SET)
		ClampValueToMinMax(value);
	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(param) = value;
		break;
	case STORAGE::PARAM:
		AccessAsParam(param)->SetConst(value);
		break;
	case STORAGE::FUNCTION:
		return SetFunc(param, value);
		break;
	default:
		assert(0);
	}
	return true;
}

template<typename Vec_t>
void VecParamDesc<Vec_t>::SetConst(void* param, const Value_t& value) const
{
	if(!TrySetConst(param, value))
		throw common::Error(ERR_MSG_CANNOT_SET_VALUE, __TFILE__, __LINE__);
}

template<typename Vec_t>
void VecParamDesc<Vec_t>::Copy(void* dstParam, const void* srcParam) const
{
	CheckCanRead();
	CheckCanWrite();

	switch(GetStorage())
	{
	case STORAGE::RAW:
		*AccessAsRaw(dstParam) = *AccessAsRaw(srcParam);
		break;
	case STORAGE::PARAM:
		*AccessAsParam(dstParam) = *AccessAsParam(srcParam);
		break;
	case STORAGE::FUNCTION:
	{
		Value_t value;
		GetConst(value, srcParam);
		SetConst(dstParam, value);
	}
	break;
	default:
		assert(0);
	}
}

template<typename Vec_t>
bool VecParamDesc<Vec_t>::ToString(std::wstring& out, const void* srcParam) const
{
	Value_t value;
	if(TryGetConst(value, srcParam))
	{
		SthToStr<Value_t>(&out, value);
		return true;
	}
	else
		return false;
}

template<typename Vec_t>
bool VecParamDesc<Vec_t>::Parse(void* dstParam, const wchar_t* src) const
{
	Value_t value;
	if(StrToSth<Value_t>(&value, src))
		return TrySetConst(dstParam, value);
	else
		return false;
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
	const wchar_t* path, bool caseSensitive)
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
            size_t paramIndex;
            for(;;)
            {
			    paramIndex = currStructDesc->Find(paramName.c_str(), caseSensitive);
			    if(paramIndex != (size_t)-1)
                    break;
                else
                {
                    currStructDesc = currStructDesc->GetBaseStructDesc();
                    if(currStructDesc == nullptr)
				        return false;
                }
            }
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
