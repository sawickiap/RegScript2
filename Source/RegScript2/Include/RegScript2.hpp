#pragma once

#include "RegScript2_Utils.hpp"

#include <Common/Base.hpp>
#include <Common/Math.hpp>
#include <Common/DateTime.hpp>
#include <Common/Error.hpp>

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <cassert>
#include <cstdint>
#include <cfloat>

namespace RegScript2
{

// For internal use only.
extern const wchar_t* const ERR_MSG_VALUE_NOT_CONST;

class StructDesc;

class EnumDesc
{
public:
	static const size_t INVALID_INDEX = (size_t)-1;

	const wchar_t* Name;
	size_t ItemCount;
	const wchar_t* const* ItemNames;
	// Optional. If null, values are just indices to ItemNames: 0, 1, 2, ...
	const int32_t* ItemValues;

	// All passed pointers must exist during lifetime of this object. Data are not copied.
	EnumDesc(
		const wchar_t* name,
		size_t itemCount,
		const wchar_t* const* itemNames,
		const int32_t* itemValues = nullptr);
	
	// Works regardless of whether ItemValues != null.
	int32_t GetValue(size_t index) const
	{
		return ItemValues ? ItemValues[index] : (int32_t)index;
	}

	// Returns index or INVALID_INDEX if not found.
	size_t FindItemByName(const wchar_t* name, bool caseSensitive) const;
	// Returns index or INVALID_INDEX if not found.
	size_t FindItemByValue(int32_t value) const;

	bool ValueIsValid(int32_t value) const
	{
		return FindItemByValue(value) != INVALID_INDEX;
	}
	void ValueToStr(std::wstring& out, int32_t value) const;
	bool StrToValue(int32_t& out, const wchar_t* str, bool caseSensitive, bool allowInteger) const;
};

template<typename Enum_t>
class TypedEnumDesc : public EnumDesc
{
	static_assert(sizeof(Enum_t) == sizeof(int32_t), "Enum type has size different than int32_t.");

public:
	TypedEnumDesc(
		const wchar_t* name,
		size_t itemCount,
		const wchar_t* const* itemNames,
		const int32_t* itemValues = nullptr) :
		EnumDesc(name, itemCount, itemNames, itemValues)
	{
	}
	TypedEnumDesc(
		const wchar_t* name,
		size_t itemCount,
		const wchar_t* const* itemNames,
		const Enum_t* itemValues) :
		EnumDesc(name, itemCount, itemNames, (const int32_t*)itemValues)
	{
	}

	bool ValueIsValid(Enum_t value) const { return EnumDesc::ValueIsValid((int32_t)value); }
	void ValueToStr(std::wstring& out, Enum_t value) const { EnumDesc::ValueToStr(out, (int32_t)value); }
	bool StrToValue(Enum_t& out, const wchar_t* str, bool caseSensitive, bool allowInteger) const { return EnumDesc::StrToValue((int32_t&)out, str, caseSensitive, allowInteger); }
};

// Class is NOT polymorphic.
class Param
{
private:
#ifdef _DEBUG
	static const uint32_t MAGIC_NUMBER_VALUE = 0x42346813;
	uint32_t m_MagicNumber;
#endif

public:
	enum class VALUE_TYPE
	{
		CONSTANT,
		WAVEFORM, // TODO Implement
		CURVE, // TODO Implement
		EXPRESSION, // TODO Implement
	};

	Param() :
#ifdef _DEBUG
		m_MagicNumber(MAGIC_NUMBER_VALUE),
#endif
		m_ValueType(VALUE_TYPE::CONSTANT)
	{
	}

#ifdef _DEBUG
	void CheckMagicNumber() const { assert(m_MagicNumber == MAGIC_NUMBER_VALUE); }
#else
	void CheckMagicNumber() const { }
#endif

	VALUE_TYPE GetValueType() { return m_ValueType; }

protected:
	VALUE_TYPE m_ValueType;
};

class BoolParam : public Param
{
public:
	BoolParam() { }
	BoolParam(bool initialValue) : m_Value(initialValue) { }

	bool IsConst() const { return true; } // TODO
	bool TryGetConst(bool& outValue) const { outValue = m_Value; return true; } // TODO
	bool GetConst() const;
	
	void SetConst(bool value);
	BoolParam& operator=(bool value) { SetConst(value); return *this; }

private:
	bool m_Value;
};

class IntParam : public Param
{
public:
	IntParam() { }
	IntParam(int32_t initialValue) : m_Value(initialValue) { }

	bool IsConst() const { return true; } // TODO
	bool TryGetConst(int32_t& outValue) const { outValue = m_Value; return true; } // TODO
	int32_t GetConst() const;

	void SetConst(int32_t value);
	IntParam& operator=(int32_t value) { SetConst(value); return *this; }

private:
	int32_t m_Value;
};

class UintParam : public Param
{
public:
	UintParam() { }
	UintParam(uint32_t initialValue) : m_Value(initialValue) { }

	bool IsConst() const { return true; } // TODO
	bool TryGetConst(uint32_t& outValue) const { outValue = m_Value; return true; } // TODO
	uint32_t GetConst() const;

	void SetConst(uint32_t value);
	UintParam& operator=(uint32_t value) { SetConst(value); return *this; }

private:
	uint32_t m_Value;
};

class EnumParam : public Param
{
public:
	EnumParam() { }
	EnumParam(int32_t initialValue) : m_Value(initialValue) { }

	bool IsConst() const { return true; } // TODO
	bool TryGetConst(int32_t& outValue) const { outValue = m_Value; return true; } // TODO
	int32_t GetConst() const;

	void SetConst(int32_t value);
	EnumParam& operator=(int32_t value) { SetConst(value); return *this; }

private:
	int32_t m_Value;
};

template<typename Enum_t>
class TypedEnumParam : public EnumParam
{
	static_assert(sizeof(Enum_t) == sizeof(int32_t), "Enum type has size different than int32_t.");

public:
	TypedEnumParam() { }
	TypedEnumParam(Enum_t initialValue) : EnumParam((int32_t)initialValue) { }

	bool TryGetConst(Enum_t& outValue) const { return EnumParam::TryGetConst((int32_t&)outValue); }
	Enum_t GetConst() const { return (Enum_t)EnumParam::GetConst(); }

	void SetConst(Enum_t value) { EnumParam::SetConst((int32_t)value); }
	EnumParam& operator=(int32_t value) { SetConst(value); return *this; }
};

class FloatParam : public Param
{
public:
	FloatParam() { }
	FloatParam(float initialValue) : m_Value(initialValue) { }

	bool IsConst() const { return true; } // TODO
	bool TryGetConst(float& outValue) const { outValue = m_Value; return true; } // TODO
	float GetConst() const;

	void SetConst(float value);
	FloatParam& operator=(float value) { SetConst(value); return *this; }

private:
	float m_Value;
};

class StringParam : public Param
{
public:
	StringParam() { }
	StringParam(const wchar_t* initialValue) : m_Value(initialValue) { }
	StringParam(const std::wstring& initialValue) : m_Value(initialValue) { }

	bool IsConst() const { return true; } // TODO
	bool TryGetConst(std::wstring& outValue) const { outValue = m_Value; return true; } // TODO
	void GetConst(std::wstring& outValue) const;

	void SetConst(const wchar_t* value);
	StringParam& operator=(const std::wstring& value) { SetConst(value.c_str()); return *this; }
	StringParam& operator=(const wchar_t* value) { SetConst(value); return *this; }

private:
	std::wstring m_Value;
};

class GameTimeParam : public Param
{
public:
	GameTimeParam() { }
	GameTimeParam(const common::GameTime& initialValue) : m_Value(initialValue) { }

	bool IsConst() const { return true; } // TODO
	bool TryGetConst(common::GameTime& outValue) const { outValue = m_Value; return true; } // TODO
	common::GameTime GetConst() const;

	void SetConst(common::GameTime value);
	GameTimeParam& operator=(common::GameTime value) { SetConst(value); return *this; }

private:
	common::GameTime m_Value;
};

// Vec_t: Use common::VEC2, VEC3, VEC4.
template<typename Vec_t>
class VecParam : public Param
{
public:
	VecParam() { }
	VecParam(const Vec_t& initialValue) : m_Value(initialValue) { }

	bool IsConst() const { return true; } // TODO
	bool TryGetConst(Vec_t& outValue) const { outValue = m_Value; return true; } // TODO
	void GetConst(Vec_t& outValue) const;

	void SetConst(const Vec_t& value);
	VecParam<Vec_t>& operator=(const Vec_t& value) { SetConst(value); return *this; }

private:
	Vec_t m_Value;
};

typedef VecParam<common::VEC2> Vec2Param;
typedef VecParam<common::VEC3> Vec3Param;
typedef VecParam<common::VEC4> Vec4Param;

enum class STORAGE
{
	RAW, // Value of destination type, e.g. unsigned, float.
	PARAM, // Value of appropriate parameter type, e.g. UintParam, FloatParam.
	FUNCTION, // No actual value, but GetFunc and/or SetFunc.
};

typedef struct StorageFunction StorageFunction;
extern StorageFunction storageFunction;

class ParamDesc
{
public:
	enum FLAGS
	{
		FLAG_READ_ONLY           = 0x01,
		FLAG_WRITE_ONLY          = 0x02,
		// Clamping or checking min-max range affects all sets, gets, SetToDefault, ToString, Parse,
		// but does not affect Copy.
		FLAG_MINMAX_CLAMP_ON_GET = 0x04,
		FLAG_MINMAX_CLAMP_ON_SET = 0x08,
		FLAG_MINMAX_FAIL_ON_SET  = 0x10,
	};

	uint32_t Flags;
	std::wstring UnitName;

	ParamDesc(STORAGE storage, uint32_t flags) : Flags(flags), m_Storage(storage) { }
	virtual ~ParamDesc() { }

	ParamDesc& SetFlags(uint32_t flags) { Flags = flags; return *this; }
	ParamDesc& SetUnitName(const wchar_t* unitName) { UnitName = unitName; return *this; }

	STORAGE GetStorage() const { return m_Storage; }

	virtual size_t GetParamSize() const = 0;
	virtual bool CanWrite() const = 0;
	virtual bool CanRead() const = 0;
	// If !CanRead, returns false.
	virtual bool IsConst(const void* param) const = 0;
	virtual void SetToDefault(void* param) const = 0;
	virtual void Copy(void* dstParam, const void* srcParam) const = 0;
	// If not supported, returns false.
	virtual bool ToString(std::wstring& out, const void* srcParam) const { out.clear(); return false; }
	// If not supported or parse error, returns false and leaves value undefined.
	virtual bool Parse(void* dstParam, const wchar_t* src) const { return false; }

protected:
	// If !CanWrite(), throws appropriate exception.
	void CheckCanWrite() const;
	// If !CanRead(), throws appropriate exception.
	void CheckCanRead() const;

private:
	STORAGE m_Storage;
};

class StructParamDesc : public ParamDesc
{
public:
	StructParamDesc(const StructDesc* structDesc) :
		ParamDesc(STORAGE::RAW, 0),
		m_StructDesc(structDesc)
	{
		assert(structDesc != nullptr);
	}
	const StructDesc* GetStructDesc() const { return m_StructDesc; }

	inline virtual size_t GetParamSize() const;
	virtual bool CanWrite() const { return !(Flags & FLAG_READ_ONLY); }
	virtual bool CanRead() const { return !(Flags & FLAG_WRITE_ONLY); }
	virtual bool IsConst(const void* param) const { return CanRead(); }
	inline virtual void SetToDefault(void* param) const;
	inline virtual void Copy(void* dstParam, const void* srcParam) const;

private:
	const StructDesc* m_StructDesc;
};

class FixedSizeArrayParamDesc : public ParamDesc
{
public:
	// Takes ownership of elementParamDesc.
	FixedSizeArrayParamDesc(const ParamDesc* elementParamDesc, size_t count) :
		ParamDesc(STORAGE::RAW, 0),
		m_ElementParamDesc(elementParamDesc), m_Count(count)
	{
		assert(elementParamDesc != nullptr);
	}
	const ParamDesc* GetElementParamDesc() const { return m_ElementParamDesc.get(); }
	size_t GetCount() const { return m_Count; }

	void* AccessElement(void* param, size_t elementIndex) const;
	const void* AccessElement(const void* param, size_t elementIndex) const;

	inline virtual size_t GetParamSize() const;
	virtual bool CanWrite() const { return !(Flags & FLAG_READ_ONLY); }
	virtual bool CanRead() const { return !(Flags & FLAG_WRITE_ONLY); }
	virtual bool IsConst(const void* param) const { return CanRead(); }
	virtual void SetToDefault(void* param) const;
	virtual void Copy(void* dstParam, const void* srcParam) const;

	void SetElementToDefault(void* param, size_t index) const;
	void CopyElement(void* dstParam, const void* srcParam, size_t index) const;

private:
	std::shared_ptr<const ParamDesc> m_ElementParamDesc;
	size_t m_Count;
};

template<typename Value_t>
class TypedParamDesc : public ParamDesc
{
public:
	Value_t DefaultValue;

	TypedParamDesc(STORAGE storage, const Value_t& defaultValue, uint32_t flags) :
		ParamDesc(storage, flags),
		DefaultValue(defaultValue)
	{
	}

private:
};

class BoolParamDesc : public TypedParamDesc<bool>
{
public:
	typedef BoolParam Param_t;
	typedef bool Value_t;
	typedef std::function<bool(Value_t&, const void*)> GetFunc_t;
	typedef std::function<bool(void*, Value_t)> SetFunc_t;

	GetFunc_t GetFunc;
	SetFunc_t SetFunc;

	BoolParamDesc(STORAGE storage, Value_t defaultValue = Value_t(), uint32_t flags = 0) :
		TypedParamDesc<bool>(storage, defaultValue, flags)
	{
	}
	BoolParamDesc(StorageFunction& storageFunction, GetFunc_t getFunc, SetFunc_t setFunc, Value_t defaultValue = Value_t(), uint32_t flags = 0) :
		TypedParamDesc<bool>(STORAGE::FUNCTION, defaultValue, flags),
		GetFunc(getFunc),
		SetFunc(setFunc)
	{
	}

	BoolParamDesc& SetDefault(Value_t defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual size_t GetParamSize() const;

	Value_t* AccessAsRaw(void* param) const { assert(GetStorage() == STORAGE::RAW); return (Value_t*)param; }
	const Value_t* AccessAsRaw(const void* param) const { assert(GetStorage() == STORAGE::RAW); return (const Value_t*)param; }
	Param_t* AccessAsParam(void* param) const { assert(GetStorage() == STORAGE::PARAM); Param_t* result = (Param_t*)param; result->CheckMagicNumber(); return result; }
	const Param_t* AccessAsParam(const void* param) const { assert(GetStorage() == STORAGE::PARAM); const Param_t* result = (const Param_t*)param; result->CheckMagicNumber(); return result; }

	virtual bool CanWrite() const { if(GetStorage() == STORAGE::FUNCTION && !SetFunc) return false; return !(Flags & FLAG_READ_ONLY); }
	virtual bool CanRead() const { if(GetStorage() == STORAGE::FUNCTION && !GetFunc) return false; return !(Flags & FLAG_WRITE_ONLY); }
	virtual bool IsConst(const void* param) const;
	bool TryGetConst(Value_t& outValue, const void* param) const;
	Value_t GetConst(const void* param) const;
	bool TrySetConst(void* param, Value_t value) const;
	void SetConst(void* param, Value_t value) const;

	virtual void SetToDefault(void* param) const { SetConst(param, DefaultValue); }
	virtual void Copy(void* dstParam, const void* srcParam) const;
	virtual bool ToString(std::wstring& out, const void* srcParam) const;
	virtual bool Parse(void* dstParam, const wchar_t* src) const;
};

class IntParamDesc : public TypedParamDesc<int32_t>
{
public:
	typedef IntParam Param_t;
	typedef int32_t Value_t;
	typedef std::function<bool(Value_t&, const void*)> GetFunc_t;
	typedef std::function<bool(void*, Value_t)> SetFunc_t;

	GetFunc_t GetFunc;
	SetFunc_t SetFunc;

	Value_t MinValue, MaxValue;

	IntParamDesc(
		STORAGE storage,
		Value_t defaultValue = Value_t(),
		uint32_t flags = 0) :
		TypedParamDesc<int32_t>(storage, defaultValue, flags),
		MinValue(INT_MIN),
		MaxValue(INT_MAX)
	{
	}
	IntParamDesc(
		StorageFunction& storageFunction,
		GetFunc_t getFunc,
		SetFunc_t setFunc,
		Value_t defaultValue = Value_t(),
		uint32_t flags = 0) :
		TypedParamDesc<int32_t>(STORAGE::FUNCTION, defaultValue, flags),
		GetFunc(getFunc),
		SetFunc(setFunc),
		MinValue(INT_MIN),
		MaxValue(INT_MAX)
	{
	}

	IntParamDesc& SetDefault(Value_t defaultValue) { DefaultValue = defaultValue; return *this; }
	IntParamDesc& SetMin(Value_t minValue) { MinValue = minValue; return *this; }
	IntParamDesc& SetMax(Value_t maxValue) { MaxValue = maxValue; return *this; }

	virtual size_t GetParamSize() const;

	bool ValueInMinMax(Value_t value) const { return value <= MaxValue && value >= MinValue; }
	void ClampValueToMinMax(Value_t& value) const { if(value < MinValue) value = MinValue; else if(value > MaxValue) value = MaxValue; }

	Value_t* AccessAsRaw(void* param) const { assert(GetStorage() == STORAGE::RAW); return (Value_t*)param; }
	const Value_t* AccessAsRaw(const void* param) const { assert(GetStorage() == STORAGE::RAW); return (const Value_t*)param; }
	Param_t* AccessAsParam(void* param) const { assert(GetStorage() == STORAGE::PARAM); Param_t* result = (Param_t*)param; result->CheckMagicNumber(); return result; }
	const Param_t* AccessAsParam(const void* param) const { assert(GetStorage() == STORAGE::PARAM); const Param_t* result = (const Param_t*)param; result->CheckMagicNumber(); return result; }

	virtual bool CanWrite() const { if(GetStorage() == STORAGE::FUNCTION && !SetFunc) return false; return !(Flags & FLAG_READ_ONLY); }
	virtual bool CanRead() const { if(GetStorage() == STORAGE::FUNCTION && !GetFunc) return false; return !(Flags & FLAG_WRITE_ONLY); }
	virtual bool IsConst(const void* param) const;
	bool TryGetConst(Value_t& outValue, const void* param) const;
	Value_t GetConst(const void* param) const;
	bool TrySetConst(void* param, Value_t value) const;
	void SetConst(void* param, Value_t value) const;

	virtual void SetToDefault(void* param) const { SetConst(param, DefaultValue); }
	virtual void Copy(void* dstParam, const void* srcParam) const;
	virtual bool ToString(std::wstring& out, const void* srcParam) const;
	virtual bool Parse(void* dstParam, const wchar_t* src) const;
};

class UintParamDesc : public TypedParamDesc<uint32_t>
{
public:
	typedef UintParam Param_t;
	typedef uint32_t Value_t;
	typedef std::function<bool(Value_t&, const void*)> GetFunc_t;
	typedef std::function<bool(void*, Value_t)> SetFunc_t;

	GetFunc_t GetFunc;
	SetFunc_t SetFunc;

	// It only affects the way of displaying value.
	enum UINT_FLAGS
	{
		FLAG_FORMAT_HEX = 0x10000, // "0x" prefix.
	};

	Value_t MinValue, MaxValue;

	UintParamDesc(
		STORAGE storage,
		Value_t defaultValue = Value_t(),
		uint32_t flags = 0) :
		TypedParamDesc<uint32_t>(storage, defaultValue, flags),
		MinValue(0),
		MaxValue(UINT_MAX)
	{
	}
	UintParamDesc(
		StorageFunction& storageFunction,
		GetFunc_t getFunc,
		SetFunc_t setFunc,
		Value_t defaultValue = Value_t(),
		uint32_t flags = 0) :
		TypedParamDesc<uint32_t>(STORAGE::FUNCTION, defaultValue, flags),
		GetFunc(getFunc),
		SetFunc(setFunc),
		MinValue(0),
		MaxValue(UINT_MAX)
	{
	}

	UintParamDesc& SetDefault(Value_t defaultValue) { DefaultValue = defaultValue; return *this; }
	UintParamDesc& SetMin(Value_t minValue) { MinValue = minValue; return *this; }
	UintParamDesc& SetMax(Value_t maxValue) { MaxValue = maxValue; return *this; }

	virtual size_t GetParamSize() const;

	bool ValueInMinMax(Value_t value) const { return value <= MaxValue && value >= MinValue; }
	void ClampValueToMinMax(Value_t& value) const { if(value < MinValue) value = MinValue; else if(value > MaxValue) value = MaxValue; }

	Value_t* AccessAsRaw(void* param) const { assert(GetStorage() == STORAGE::RAW); return (Value_t*)param; }
	const Value_t* AccessAsRaw(const void* param) const { assert(GetStorage() == STORAGE::RAW); return (const Value_t*)param; }
	Param_t* AccessAsParam(void* param) const { assert(GetStorage() == STORAGE::PARAM); Param_t* result = (Param_t*)param; result->CheckMagicNumber(); return result; }
	const Param_t* AccessAsParam(const void* param) const { assert(GetStorage() == STORAGE::PARAM); const Param_t* result = (const Param_t*)param; result->CheckMagicNumber(); return result; }

	virtual bool CanWrite() const { if(GetStorage() == STORAGE::FUNCTION && !SetFunc) return false; return !(Flags & FLAG_READ_ONLY); }
	virtual bool CanRead() const { if(GetStorage() == STORAGE::FUNCTION && !GetFunc) return false; return !(Flags & FLAG_WRITE_ONLY); }
	virtual bool IsConst(const void* param) const;
	bool TryGetConst(Value_t& outValue, const void* param) const;
	Value_t GetConst(const void* param) const;
	bool TrySetConst(void* param, Value_t value) const;
	void SetConst(void* param, Value_t value) const;

	virtual void SetToDefault(void* param) const { SetConst(param, DefaultValue); }
	virtual void Copy(void* dstParam, const void* srcParam) const;
	virtual bool ToString(std::wstring& out, const void* srcParam) const;
	virtual bool Parse(void* dstParam, const wchar_t* src) const;
};

class EnumParamDesc : public TypedParamDesc<int32_t>
{
public:
	typedef EnumParam Param_t;
	typedef int32_t Value_t;
	typedef std::function<bool(Value_t&, const void*)> GetFunc_t;
	typedef std::function<bool(void*, Value_t)> SetFunc_t;

	const EnumDesc* m_EnumDesc;
	GetFunc_t GetFunc;
	SetFunc_t SetFunc;

	/*
	FLAG_MINMAX_FAIL_ON_SET works with this type.
	If value is not in item list, fails to set.
	It also disallows integer numbers when parsing from string.
	*/

	EnumParamDesc(
		STORAGE storage,
		const EnumDesc* enumDesc,
		Value_t defaultValue = Value_t(),
		uint32_t flags = 0) :
		TypedParamDesc<int32_t>(storage, defaultValue, flags),
		m_EnumDesc(enumDesc)
	{
		assert(enumDesc);
	}
	EnumParamDesc(
		StorageFunction& storageFunction,
		const EnumDesc* enumDesc,
		GetFunc_t getFunc,
		SetFunc_t setFunc,
		Value_t defaultValue = Value_t(),
		uint32_t flags = 0) :
		TypedParamDesc<int32_t>(STORAGE::FUNCTION, defaultValue, flags),
		m_EnumDesc(enumDesc),
		GetFunc(getFunc),
		SetFunc(setFunc)
	{
		assert(enumDesc);
	}

	EnumParamDesc& SetDefault(Value_t defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual size_t GetParamSize() const;

	Value_t* AccessAsRaw(void* param) const { assert(GetStorage() == STORAGE::RAW); return (Value_t*)param; }
	const Value_t* AccessAsRaw(const void* param) const { assert(GetStorage() == STORAGE::RAW); return (const Value_t*)param; }
	Param_t* AccessAsParam(void* param) const { assert(GetStorage() == STORAGE::PARAM); Param_t* result = (Param_t*)param; result->CheckMagicNumber(); return result; }
	const Param_t* AccessAsParam(const void* param) const { assert(GetStorage() == STORAGE::PARAM); const Param_t* result = (const Param_t*)param; result->CheckMagicNumber(); return result; }

	virtual bool CanWrite() const { if(GetStorage() == STORAGE::FUNCTION && !SetFunc) return false; return !(Flags & FLAG_READ_ONLY); }
	virtual bool CanRead() const { if(GetStorage() == STORAGE::FUNCTION && !GetFunc) return false; return !(Flags & FLAG_WRITE_ONLY); }
	virtual bool IsConst(const void* param) const;
	bool TryGetConst(Value_t& outValue, const void* param) const;
	Value_t GetConst(const void* param) const;
	bool TrySetConst(void* param, Value_t value) const;
	void SetConst(void* param, Value_t value) const;

	virtual void SetToDefault(void* param) const { SetConst(param, DefaultValue); }
	virtual void Copy(void* dstParam, const void* srcParam) const;
	virtual bool ToString(std::wstring& out, const void* srcParam) const;
	virtual bool Parse(void* dstParam, const wchar_t* src) const;
};

class FloatParamDesc : public TypedParamDesc<float>
{
public:
	typedef FloatParam Param_t;
	typedef float Value_t;
	typedef std::function<bool(Value_t&, const void*)> GetFunc_t;
	typedef std::function<bool(void*, Value_t)> SetFunc_t;

	GetFunc_t GetFunc;
	SetFunc_t SetFunc;

	// It only affects the way of displaying value.
	enum FLOAT_FLAGS
	{
		FLAG_FORMAT_PERCENT = 0x20000, // Doesn't limit range to 0..1.
		FLAG_FORMAT_DB      = 0x40000, // Value must be positive. Otherwise NORMAL is used.
	};

	// When min-max values are active, non-finite values are also not accepted.
	Value_t MinValue, MaxValue, Step;
	// UINT_MAX means general format should be used.
	uint32_t Precision;

	FloatParamDesc(
		STORAGE storage,
		Value_t defaultValue = Value_t(),
		uint32_t flags = 0) :
		TypedParamDesc<float>(storage, defaultValue, flags),
		MinValue(-FLT_MAX),
		MaxValue(FLT_MAX),
		Step(1.f),
		Precision(UINT_MAX)
	{
	}
	FloatParamDesc(
		StorageFunction& storageFunction,
		GetFunc_t getFunc,
		SetFunc_t setFunc,
		Value_t defaultValue = Value_t(),
		uint32_t flags = 0) :
		TypedParamDesc<float>(STORAGE::FUNCTION, defaultValue, flags),
		GetFunc(getFunc),
		SetFunc(setFunc),
		MinValue(-FLT_MAX),
		MaxValue(FLT_MAX),
		Step(1.f),
		Precision(UINT_MAX)
	{
	}

	virtual size_t GetParamSize() const;

	FloatParamDesc& SetDefault(Value_t defaultValue) { DefaultValue = defaultValue; return *this; }
	FloatParamDesc& SetMin(Value_t minValue) { MinValue = minValue; return *this; }
	FloatParamDesc& SetMax(Value_t maxValue) { MaxValue = maxValue; return *this; }
	FloatParamDesc& SetStep(Value_t step) { Step = step; return *this; }
	FloatParamDesc& SetPrecision(uint32_t precision) { Precision = precision; return *this; }

	bool ValueInMinMax(Value_t value) const { return value <= MaxValue && value >= MinValue; }
	void ClampValueToMinMax(Value_t& value) const { if(!(value >= MinValue)) value = MinValue; else if(!(value <= MaxValue)) value = MaxValue; }
	bool HasMinMax() const { return MinValue != -FLT_MAX && MaxValue != FLT_MAX; }

	Value_t* AccessAsRaw(void* param) const { assert(GetStorage() == STORAGE::RAW); return (Value_t*)param; }
	const Value_t* AccessAsRaw(const void* param) const { assert(GetStorage() == STORAGE::RAW); return (const Value_t*)param; }
	Param_t* AccessAsParam(void* param) const { assert(GetStorage() == STORAGE::PARAM); Param_t* result = (Param_t*)param; result->CheckMagicNumber(); return result; }
	const Param_t* AccessAsParam(const void* param) const { assert(GetStorage() == STORAGE::PARAM); const Param_t* result = (const Param_t*)param; result->CheckMagicNumber(); return result; }

	virtual bool CanWrite() const { if(GetStorage() == STORAGE::FUNCTION && !SetFunc) return false; return !(Flags & FLAG_READ_ONLY); }
	virtual bool CanRead() const { if(GetStorage() == STORAGE::FUNCTION && !GetFunc) return false; return !(Flags & FLAG_WRITE_ONLY); }
	virtual bool IsConst(const void* param) const;
	bool TryGetConst(Value_t& outValue, const void* param) const;
	Value_t GetConst(const void* param) const;
	bool TrySetConst(void* param, Value_t value) const;
	void SetConst(void* param, Value_t value) const;

	virtual void SetToDefault(void* param) const { SetConst(param, DefaultValue); }
	virtual void Copy(void* dstParam, const void* srcParam) const;
	virtual bool ToString(std::wstring& out, const void* srcParam) const;
	virtual bool Parse(void* dstParam, const wchar_t* src) const;

private:
	void ValueToStr(std::wstring& out, float value) const;
};

class StringParamDesc : public TypedParamDesc<std::wstring>
{
public:
	typedef StringParam Param_t;
	typedef std::wstring Value_t;
	typedef std::function<bool(Value_t&, const void*)> GetFunc_t;
	typedef std::function<bool(void*, const Value_t&)> SetFunc_t;

	GetFunc_t GetFunc;
	SetFunc_t SetFunc;

	StringParamDesc(STORAGE storage, const Value_t& defaultValue = Value_t(), uint32_t flags = 0) :
		TypedParamDesc<std::wstring>(storage, defaultValue, flags)
	{
	}
	StringParamDesc(StorageFunction& storageFunction, GetFunc_t getFunc, SetFunc_t setFunc, const Value_t& defaultValue = Value_t(), uint32_t flags = 0) :
		TypedParamDesc<std::wstring>(STORAGE::FUNCTION, defaultValue, flags),
		GetFunc(getFunc),
		SetFunc(setFunc)
	{
	}

	StringParamDesc& SetDefault(const Value_t& defaultValue) { DefaultValue = defaultValue; return *this; }
	StringParamDesc& SetDefault(const wchar_t* defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual size_t GetParamSize() const;

	Value_t* AccessAsRaw(void* param) const { assert(GetStorage() == STORAGE::RAW); return (Value_t*)param; }
	const Value_t* AccessAsRaw(const void* param) const { assert(GetStorage() == STORAGE::RAW); return (const Value_t*)param; }
	Param_t* AccessAsParam(void* param) const { assert(GetStorage() == STORAGE::PARAM); Param_t* result = (Param_t*)param; result->CheckMagicNumber(); return result; }
	const Param_t* AccessAsParam(const void* param) const { assert(GetStorage() == STORAGE::PARAM); const Param_t* result = (const Param_t*)param; result->CheckMagicNumber(); return result; }

	virtual bool CanWrite() const { if(GetStorage() == STORAGE::FUNCTION && !SetFunc) return false; return !(Flags & FLAG_READ_ONLY); }
	virtual bool CanRead() const { if(GetStorage() == STORAGE::FUNCTION && !GetFunc) return false; return !(Flags & FLAG_WRITE_ONLY); }
	virtual bool IsConst(const void* param) const;
	bool TryGetConst(Value_t& outValue, const void* param) const;
	void GetConst(Value_t& outValue, const void* param) const;
	bool TrySetConst(void* param, const wchar_t* value) const;
	void SetConst(void* param, const wchar_t* value) const;
	bool TrySetConst(void* param, const std::wstring& value) const { return TrySetConst(param, value.c_str()); }
	void SetConst(void* param, const std::wstring& value) const { SetConst(param, value.c_str()); }

	virtual void SetToDefault(void* param) const { SetConst(param, DefaultValue.c_str()); }
	virtual void Copy(void* dstParam, const void* srcParam) const;
	virtual bool ToString(std::wstring& out, const void* srcParam) const;
	virtual bool Parse(void* dstParam, const wchar_t* src) const;
};

class GameTimeParamDesc : public TypedParamDesc<common::GameTime>
{
public:
	typedef GameTimeParam Param_t;
	typedef common::GameTime Value_t;
	typedef std::function<bool(Value_t&, const void*)> GetFunc_t;
	typedef std::function<bool(void*, Value_t)> SetFunc_t;

	GetFunc_t GetFunc;
	SetFunc_t SetFunc;

	Value_t MinValue, MaxValue;

	GameTimeParamDesc(
		STORAGE storage,
		Value_t defaultValue = Value_t(),
		uint32_t flags = 0) :
		TypedParamDesc<common::GameTime>(storage, defaultValue, flags),
		MinValue(common::GameTime::MIN_VALUE),
		MaxValue(common::GameTime::MAX_VALUE)
	{
	}
	GameTimeParamDesc(
		StorageFunction& storageFunction,
		GetFunc_t getFunc,
		SetFunc_t setFunc,
		Value_t defaultValue = Value_t(),
		uint32_t flags = 0) :
		TypedParamDesc<common::GameTime>(STORAGE::FUNCTION, defaultValue, flags),
		GetFunc(getFunc),
		SetFunc(setFunc),
		MinValue(common::GameTime::MIN_VALUE),
		MaxValue(common::GameTime::MAX_VALUE)
	{
	}

	GameTimeParamDesc& SetDefault(Value_t defaultValue) { DefaultValue = defaultValue; return *this; }
	GameTimeParamDesc& SetMin(Value_t minValue) { MinValue = minValue; return *this; }
	GameTimeParamDesc& SetMax(Value_t maxValue) { MaxValue = maxValue; return *this; }

	virtual size_t GetParamSize() const;

	bool ValueInMinMax(Value_t value) const { return value <= MaxValue && value >= MinValue; }
	void ClampValueToMinMax(Value_t& value) const { if(value < MinValue) value = MinValue; else if(value > MaxValue) value = MaxValue; }

	Value_t* AccessAsRaw(void* param) const { assert(GetStorage() == STORAGE::RAW); return (Value_t*)param; }
	const Value_t* AccessAsRaw(const void* param) const { assert(GetStorage() == STORAGE::RAW); return (const Value_t*)param; }
	Param_t* AccessAsParam(void* param) const { assert(GetStorage() == STORAGE::PARAM); Param_t* result = (Param_t*)param; result->CheckMagicNumber(); return result; }
	const Param_t* AccessAsParam(const void* param) const { assert(GetStorage() == STORAGE::PARAM); const Param_t* result = (const Param_t*)param; result->CheckMagicNumber(); return result; }

	virtual bool CanWrite() const { if(GetStorage() == STORAGE::FUNCTION && !SetFunc) return false; return !(Flags & FLAG_READ_ONLY); }
	virtual bool CanRead() const { if(GetStorage() == STORAGE::FUNCTION && !GetFunc) return false; return !(Flags & FLAG_WRITE_ONLY); }
	virtual bool IsConst(const void* param) const;
	bool TryGetConst(Value_t& outValue, const void* param) const;
	Value_t GetConst(const void* param) const;
	bool TrySetConst(void* param, Value_t value) const;
	void SetConst(void* param, Value_t value) const;

	virtual void SetToDefault(void* param) const { SetConst(param, DefaultValue); }
	virtual void Copy(void* dstParam, const void* srcParam) const;
	virtual bool ToString(std::wstring& out, const void* srcParam) const;
	virtual bool Parse(void* dstParam, const wchar_t* src) const;
};

template<typename Vec_t>
class VecParamDesc : public TypedParamDesc<Vec_t>
{
public:
	typedef VecParam<Vec_t> Param_t;
	typedef Vec_t Value_t;
	typedef std::function<bool(Value_t&, const void*)> GetFunc_t;
	typedef std::function<bool(void*, const Value_t&)> SetFunc_t;

	GetFunc_t GetFunc;
	SetFunc_t SetFunc;

	Value_t MinValue, MaxValue;

	VecParamDesc(
		STORAGE storage,
		Value_t defaultValue = Value_t(),
		uint32_t flags = 0) :
		TypedParamDesc<Vec_t>(storage, defaultValue, flags),
		MinValue(DefaultMinValue()),
		MaxValue(DefaultMaxValue())
	{
	}
	VecParamDesc(
		StorageFunction& storageFunction,
		GetFunc_t getFunc,
		SetFunc_t setFunc,
		Value_t defaultValue = Value_t(),
		uint32_t flags = 0) :
		TypedParamDesc<Vec_t>(STORAGE::FUNCTION, defaultValue, flags),
		GetFunc(getFunc),
		SetFunc(setFunc),
		MinValue(DefaultMinValue()),
		MaxValue(DefaultMaxValue())
	{
	}

	VecParamDesc<Vec_t>& SetDefault(const Value_t& defaultValue) { DefaultValue = defaultValue; return *this; }
	VecParamDesc<Vec_t>& SetMin(const Value_t& minValue) { MinValue = minValue; return *this; }
	VecParamDesc<Vec_t>& SetMax(const Value_t& maxValue) { MaxValue = maxValue; return *this; }

	virtual size_t GetParamSize() const;

	bool ValueInMinMax(Value_t value) const;
	void ClampValueToMinMax(Value_t& value) const;

	Value_t* AccessAsRaw(void* param) const { assert(GetStorage() == STORAGE::RAW); return (Value_t*)param; }
	const Value_t* AccessAsRaw(const void* param) const { assert(GetStorage() == STORAGE::RAW); return (const Value_t*)param; }
	Param_t* AccessAsParam(void* param) const { assert(GetStorage() == STORAGE::PARAM); Param_t* result = (Param_t*)param; result->CheckMagicNumber(); return result; }
	const Param_t* AccessAsParam(const void* param) const { assert(GetStorage() == STORAGE::PARAM); const Param_t* result = (const Param_t*)param; result->CheckMagicNumber(); return result; }

	virtual bool CanWrite() const { if(GetStorage() == STORAGE::FUNCTION && !SetFunc) return false; return !(Flags & FLAG_READ_ONLY); }
	virtual bool CanRead() const { if(GetStorage() == STORAGE::FUNCTION && !GetFunc) return false; return !(Flags & FLAG_WRITE_ONLY); }
	virtual bool IsConst(const void* param) const;
	bool TryGetConst(Value_t& outValue, const void* param) const;
	void GetConst(Value_t& outValue, const void* param) const;
	bool TrySetConst(void* param, Value_t value) const;
	void SetConst(void* param, const Value_t& value) const;

	virtual void SetToDefault(void* param) const { SetConst(param, DefaultValue); }
	virtual void Copy(void* dstParam, const void* srcParam) const;
	virtual bool ToString(std::wstring& out, const void* srcParam) const;
	virtual bool Parse(void* dstParam, const wchar_t* src) const;

private:
	static Vec_t DefaultMinValue();
	static Vec_t DefaultMaxValue();
};

typedef VecParamDesc<common::VEC2> Vec2ParamDesc;
typedef VecParamDesc<common::VEC3> Vec3ParamDesc;
typedef VecParamDesc<common::VEC4> Vec4ParamDesc;

class StructDesc
{
public:
	std::vector<std::wstring> Names;
	std::vector<size_t> Offsets;
	std::vector<std::shared_ptr<ParamDesc>> Params;

	StructDesc(const wchar_t* name, size_t structSize, const StructDesc* baseStructDesc = nullptr) : m_Name(name), m_StructSize(structSize), m_BaseStructDesc(baseStructDesc) { }
	const wchar_t* GetName() const { return m_Name.c_str(); }
	size_t GetStructSize() const { return m_StructSize; }
	const StructDesc* GetBaseStructDesc() const { return m_BaseStructDesc; }

	// Takes ownership of param.
	template<typename ParamDesc_t>
	ParamDesc_t& AddParam(const wchar_t* name, size_t offset, ParamDesc_t* param)
	{
		Names.push_back(name);
		Offsets.push_back(offset);
		Params.emplace_back(std::shared_ptr<ParamDesc>(param));
		return *param;
	}

	char* AccessRawParam(void* obj, size_t paramIndex) const { return (char*)obj + Offsets[paramIndex]; }
	const char* AccessRawParam(const void* obj, size_t paramIndex) const { return (const char*)obj + Offsets[paramIndex]; }

	template<typename Param_t>
	Param_t* AccessParam(void* obj, size_t paramIndex) const { return (Param_t*)AccessRawParam(obj, paramIndex); }
	template<typename Param_t>
	const Param_t* AccessParam(const void* obj, size_t paramIndex) const { return (const Param_t*)AccessRawParam(obj, paramIndex); }

	void SetParamToDefault(void* obj, size_t paramIndex) const;
	void CopyParam(void* dstObj, const void* srcObj, size_t paramIndex) const;

	void SetObjToDefault(void* obj) const;
	void CopyObj(void* dstObj, const void* srcObj) const;

	// Returns index. Not found: returns -1.
	size_t Find(const wchar_t* name, bool caseSensitive = true) const;
	ParamDesc* GetParamDesc(size_t index);
	const ParamDesc* GetParamDesc(size_t index) const;

private:
	std::wstring m_Name;
	size_t m_StructSize;
	const StructDesc* m_BaseStructDesc;
};

bool FindObjParamByPath(
	void*& outParam, const ParamDesc*& outParamDesc,
	void* obj, const StructDesc& structDesc,
	const wchar_t* path, bool caseSensitive);

inline size_t StructParamDesc::GetParamSize() const
{
	return m_StructDesc->GetStructSize();
}

inline void StructParamDesc::SetToDefault(void* param) const
{
	CheckCanWrite();
	
	m_StructDesc->SetObjToDefault(param);
}

inline void StructParamDesc::Copy(void* dstParam, const void* srcParam) const
{
	CheckCanRead();
	CheckCanWrite();
	
	m_StructDesc->CopyObj(dstParam, srcParam);
}

inline size_t FixedSizeArrayParamDesc::GetParamSize() const
{
	return m_ElementParamDesc->GetParamSize() * m_Count;
}

} // namespace RegScript2

#define RS2_GET_STRUCT_DESC_BEGIN(structName, ...) \
	static unique_ptr<rs2::StructDesc> structDesc; \
	if(!structDesc) \
	{ \
		typedef structName Struct_t; \
		structDesc = std::make_unique<rs2::StructDesc>(L#structName, sizeof(structName), __VA_ARGS__);

#define RS2_GET_STRUCT_DESC_END() \
	} \
	return structDesc.get();

#define RS2_ADD_PARAM_STRUCT(paramName, nestedStructDesc) \
	(structDesc->AddParam( \
		L#paramName, \
		offsetof(Struct_t, paramName), \
		new rs2::StructParamDesc(nestedStructDesc)))
#define RS2_ADD_PARAM_FIXED_SIZE_ARRAY(paramName, elementStructDesc, count) \
	(structDesc->AddParam( \
		L#paramName, \
		offsetof(Struct_t, paramName), \
		new rs2::FixedSizeArrayParamDesc(elementStructDesc, count)))
#define RS2_ADD_PARAM_BOOL(paramName, storage, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		offsetof(Struct_t, paramName), \
		new rs2::BoolParamDesc(storage, __VA_ARGS__)))
#define RS2_ADD_PARAM_INT(paramName, storage, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		offsetof(Struct_t, paramName), \
		new rs2::IntParamDesc(storage, __VA_ARGS__)))
#define RS2_ADD_PARAM_UINT(paramName, storage, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		offsetof(Struct_t, paramName), \
		new rs2::UintParamDesc(storage, __VA_ARGS__)))
#define RS2_ADD_PARAM_FLOAT(paramName, storage, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		offsetof(Struct_t, paramName), \
		new rs2::FloatParamDesc(storage, __VA_ARGS__)))
#define RS2_ADD_PARAM_STRING(paramName, storage, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		offsetof(Struct_t, paramName), \
		new rs2::StringParamDesc(storage, __VA_ARGS__)))
#define RS2_ADD_PARAM_GAMETIME(paramName, storage, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		offsetof(Struct_t, paramName), \
		new rs2::GameTimeParamDesc(storage, __VA_ARGS__)))
#define RS2_ADD_PARAM_VEC2(paramName, storage, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		offsetof(Struct_t, paramName), \
		new rs2::Vec2ParamDesc(storage, __VA_ARGS__)))
#define RS2_ADD_PARAM_VEC3(paramName, storage, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		offsetof(Struct_t, paramName), \
		new rs2::Vec3ParamDesc(storage, __VA_ARGS__)))
#define RS2_ADD_PARAM_VEC4(paramName, storage, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		offsetof(Struct_t, paramName), \
		new rs2::Vec4ParamDesc(storage, __VA_ARGS__)))


// Initializes float param with Format=Percent|MinMaxClampOnSet, Min=0, Max=1, Step=0.02.
#define RS2_ADD_PARAM_FLOAT_PERCENT(paramName, storage, defaultValue) \
	RS2_ADD_PARAM_FLOAT(paramName, storage, defaultValue, RegScript2::FloatParamDesc::FLAG_FORMAT_PERCENT | rs2::ParamDesc::FLAG_MINMAX_CLAMP_ON_SET).SetMin(0.f).SetMax(1.f).SetStep(0.02f)


#define RS2_ADD_PARAM_BOOL_FUNCTION(paramName, getFunc, setFunc, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		0, \
		new rs2::BoolParamDesc(RegScript2::storageFunction, getFunc, setFunc, __VA_ARGS__)))
#define RS2_ADD_PARAM_INT_FUNCTION(paramName, getFunc, setFunc, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		0, \
		new rs2::IntParamDesc(RegScript2::storageFunction, getFunc, setFunc, __VA_ARGS__)))
#define RS2_ADD_PARAM_UINT_FUNCTION(paramName, getFunc, setFunc, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		0, \
		new rs2::UintParamDesc(RegScript2::storageFunction, getFunc, setFunc, __VA_ARGS__)))
#define RS2_ADD_PARAM_FLOAT_FUNCTION(paramName, getFunc, setFunc, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		0, \
		new rs2::FloatParamDesc(RegScript2::storageFunction, getFunc, setFunc, __VA_ARGS__)))
#define RS2_ADD_PARAM_STRING_FUNCTION(paramName, getFunc, setFunc, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		0, \
		new rs2::StringParamDesc(RegScript2::storageFunction, getFunc, setFunc, __VA_ARGS__)))
#define RS2_ADD_PARAM_GAMETIME_FUNCTION(paramName, getFunc, setFunc, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		0, \
		new rs2::GameTimeParamDesc(RegScript2::storageFunction, getFunc, setFunc, __VA_ARGS__)))
#define RS2_ADD_PARAM_VEC2_FUNCTION(paramName, getFunc, setFunc, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		0, \
		new rs2::Vec2ParamDesc(RegScript2::storageFunction, getFunc, setFunc, __VA_ARGS__)))
#define RS2_ADD_PARAM_VEC3_FUNCTION(paramName, getFunc, setFunc, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		0, \
		new rs2::Vec3ParamDesc(RegScript2::storageFunction, getFunc, setFunc, __VA_ARGS__)))
#define RS2_ADD_PARAM_VEC4_FUNCTION(paramName, getFunc, setFunc, ...) \
	(structDesc->AddParam( \
		L#paramName, \
		0, \
		new rs2::Vec4ParamDesc(RegScript2::storageFunction, getFunc, setFunc, __VA_ARGS__)))
