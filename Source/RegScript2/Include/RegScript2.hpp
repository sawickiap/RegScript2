#pragma once

#include "RegScript2_Utils.hpp"

#include <Common/Base.hpp>
#include <Common/Math.hpp>
#include <Common/DateTime.hpp>
#include <Common/Error.hpp>

#include <string>
#include <vector>
#include <memory>

#include <cassert>
#include <cstdint>

namespace RegScript2
{

// For internal use only.
extern const wchar_t* const ERR_MSG_VALUE_NOT_CONST;

class StructDesc;

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

class ParamDesc
{
public:
	enum class STORAGE
	{
		RAW, // Value of destination type, e.g. unsigned, float.
		PARAM, // Value of appropriate parameter type, e.g. UintParam, FloatParam.
	};

	ParamDesc(STORAGE storage) : m_Storage(storage) { }
	virtual ~ParamDesc() { }

	STORAGE GetStorage() const { return m_Storage; }

	virtual size_t GetParamSize() const = 0;
	virtual bool IsConst(const void* param) const = 0;
	virtual void SetToDefault(void* param) const = 0;
	virtual void Copy(void* dstParam, const void* srcParam) const = 0;
	// If not supported, returns false.
	virtual bool ToString(std::wstring& out, const void* srcParam) const { out.clear(); return false; }
	// If not supported or parse error, returns false and leaves value undefined.
	virtual bool Parse(void* dstParam, const wchar_t* src) const { return false; }

private:
	STORAGE m_Storage;
};

class StructParamDesc : public ParamDesc
{
public:
	StructParamDesc(const StructDesc* structDesc) :
		ParamDesc(STORAGE::RAW),
		m_StructDesc(structDesc)
	{
		assert(structDesc != nullptr);
	}
	const StructDesc* GetStructDesc() const { return m_StructDesc; }

	inline virtual size_t GetParamSize() const;
	virtual bool IsConst(const void* param) const { return true; }
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
		ParamDesc(STORAGE::RAW),
		m_ElementParamDesc(elementParamDesc), m_Count(count)
	{
		assert(elementParamDesc != nullptr);
	}
	const ParamDesc* GetElementParamDesc() const { return m_ElementParamDesc.get(); }
	size_t GetCount() const { return m_Count; }

	void* AccessElement(void* param, size_t elementIndex) const;
	const void* AccessElement(const void* param, size_t elementIndex) const;

	inline virtual size_t GetParamSize() const;
	virtual bool IsConst(const void* param) const { return true; }
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

	TypedParamDesc(STORAGE storage) :
		ParamDesc(storage),
		DefaultValue()
	{
	}

private:
};

class BoolParamDesc : public TypedParamDesc<bool>
{
public:
	typedef BoolParam Param_t;
	typedef bool Value_t;

	BoolParamDesc(STORAGE storage) :
		TypedParamDesc<bool>(storage)
	{
	}

	BoolParamDesc& SetDefault(Value_t defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual size_t GetParamSize() const;

	Value_t* AccessAsRaw(void* param) const { return (Value_t*)param; }
	const Value_t* AccessAsRaw(const void* param) const { return (const Value_t*)param; }
	Param_t* AccessAsParam(void* param) const { Param_t* result = (Param_t*)param; result->CheckMagicNumber(); return result; }
	const Param_t* AccessAsParam(const void* param) const { const Param_t* result = (const Param_t*)param; result->CheckMagicNumber(); return result; }

	virtual bool IsConst(const void* param) const;
	bool TryGetConst(Value_t& outValue, const void* param) const;
	Value_t GetConst(const void* param) const;
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

	// It only affects the way of displaying value.
	enum FORMAT
	{
		FORMAT_DEC,
		FORMAT_HEX, // "0x" prefix.
		FORMAT_COUNT
	};
	FORMAT Format;

	UintParamDesc(STORAGE storage) :
		TypedParamDesc<uint32_t>(storage),
		Format(FORMAT_DEC)
	{
	}

	UintParamDesc& SetDefault(Value_t defaultValue) { DefaultValue = defaultValue; return *this; }
	UintParamDesc& SetFormat(FORMAT format) { Format = format; return *this; }

	virtual size_t GetParamSize() const;

	Value_t* AccessAsRaw(void* param) const { return (Value_t*)param; }
	const Value_t* AccessAsRaw(const void* param) const { return (const Value_t*)param; }
	Param_t* AccessAsParam(void* param) const { Param_t* result = (Param_t*)param; result->CheckMagicNumber(); return result; }
	const Param_t* AccessAsParam(const void* param) const { const Param_t* result = (const Param_t*)param; result->CheckMagicNumber(); return result; }

	virtual bool IsConst(const void* param) const;
	bool TryGetConst(Value_t& outValue, const void* param) const;
	Value_t GetConst(const void* param) const;
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

	// It only affects the way of displaying value.
	enum FORMAT
	{
		FORMAT_NORMAL,
		FORMAT_PERCENT, // Doesn't limit range to 0..1.
		FORMAT_DB,      // Value must be positive. Otherwise NORMAL is used.
		FORMAT_COUNT
	};
	FORMAT Format;

	FloatParamDesc(STORAGE storage) :
		TypedParamDesc<float>(storage),
		Format(FORMAT_NORMAL)
	{
	}

	FloatParamDesc& SetDefault(Value_t defaultValue) { DefaultValue = defaultValue; return *this; }
	FloatParamDesc& SetFormat(FORMAT format) { Format = format; return *this; }

	virtual size_t GetParamSize() const;

	Value_t* AccessAsRaw(void* param) const { return (Value_t*)param; }
	const Value_t* AccessAsRaw(const void* param) const { return (const Value_t*)param; }
	Param_t* AccessAsParam(void* param) const { Param_t* result = (Param_t*)param; result->CheckMagicNumber(); return result; }
	const Param_t* AccessAsParam(const void* param) const { const Param_t* result = (const Param_t*)param; result->CheckMagicNumber(); return result; }

	virtual bool IsConst(const void* param) const;
	bool TryGetConst(Value_t& outValue, const void* param) const;
	Value_t GetConst(const void* param) const;
	void SetConst(void* param, Value_t value) const;

	virtual void SetToDefault(void* param) const { SetConst(param, DefaultValue); }
	virtual void Copy(void* dstParam, const void* srcParam) const;
	virtual bool ToString(std::wstring& out, const void* srcParam) const;
	virtual bool Parse(void* dstParam, const wchar_t* src) const;
};

class StringParamDesc : public TypedParamDesc<std::wstring>
{
public:
	typedef StringParam Param_t;
	typedef std::wstring Value_t;

	StringParamDesc(STORAGE storage) :
		TypedParamDesc<std::wstring>(storage)
	{
	}

	StringParamDesc& SetDefault(const wchar_t* defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual size_t GetParamSize() const;

	Value_t* AccessAsRaw(void* param) const { return (Value_t*)param; }
	const Value_t* AccessAsRaw(const void* param) const { return (const Value_t*)param; }
	Param_t* AccessAsParam(void* param) const { Param_t* result = (Param_t*)param; result->CheckMagicNumber(); return result; }
	const Param_t* AccessAsParam(const void* param) const { const Param_t* result = (const Param_t*)param; result->CheckMagicNumber(); return result; }

	virtual bool IsConst(const void* param) const;
	bool TryGetConst(Value_t& outValue, const void* param) const;
	void GetConst(Value_t& outValue, const void* param) const;
	void SetConst(void* param, const wchar_t* value) const;
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

	GameTimeParamDesc(STORAGE storage) :
		TypedParamDesc<common::GameTime>(storage)
	{
	}

	GameTimeParamDesc& SetDefault(const Value_t& defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual size_t GetParamSize() const;

	Value_t* AccessAsRaw(void* param) const { return (Value_t*)param; }
	const Value_t* AccessAsRaw(const void* param) const { return (const Value_t*)param; }
	Param_t* AccessAsParam(void* param) const { Param_t* result = (Param_t*)param; result->CheckMagicNumber(); return result; }
	const Param_t* AccessAsParam(const void* param) const { const Param_t* result = (const Param_t*)param; result->CheckMagicNumber(); return result; }

	virtual bool IsConst(const void* param) const;
	bool TryGetConst(Value_t& outValue, const void* param) const;
	Value_t GetConst(const void* param) const;
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

	VecParamDesc(STORAGE storage) :
		TypedParamDesc<Vec_t>(storage)
	{
	}

	VecParamDesc<Vec_t>& SetDefault(const Value_t& defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual size_t GetParamSize() const;

	Value_t* AccessAsRaw(void* param) const { return (Value_t*)param; }
	const Value_t* AccessAsRaw(const void* param) const { return (const Value_t*)param; }
	Param_t* AccessAsParam(void* param) const { Param_t* result = (Param_t*)param; result->CheckMagicNumber(); return result; }
	const Param_t* AccessAsParam(const void* param) const { const Param_t* result = (const Param_t*)param; result->CheckMagicNumber(); return result; }

	virtual bool IsConst(const void* param) const;
	bool TryGetConst(Value_t& outValue, const void* param) const;
	void GetConst(Value_t& outValue, const void* param) const;
	void SetConst(void* param, const Value_t& value) const;

	virtual void SetToDefault(void* param) const { SetConst(param, DefaultValue); }
	virtual void Copy(void* dstParam, const void* srcParam) const;
	virtual bool ToString(std::wstring& out, const void* srcParam) const;
	virtual bool Parse(void* dstParam, const wchar_t* src) const;
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

	template<typename ParamDesc_t>
	void AddParam(const wchar_t* name, size_t offset, const ParamDesc_t& param)
	{
		Names.push_back(name);
		Offsets.push_back(offset);
		Params.emplace_back(std::make_shared<ParamDesc_t>(param));
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
	size_t Find(const wchar_t* name) const;
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
	const wchar_t* path);

inline size_t StructParamDesc::GetParamSize() const
{
	return m_StructDesc->GetStructSize();
}

inline void StructParamDesc::SetToDefault(void* param) const
{
	m_StructDesc->SetObjToDefault(param);
}

inline void StructParamDesc::Copy(void* dstParam, const void* srcParam) const
{
	m_StructDesc->CopyObj(dstParam, srcParam);
}

inline size_t FixedSizeArrayParamDesc::GetParamSize() const
{
	return m_ElementParamDesc->GetParamSize() * m_Count;
}

} // namespace RegScript2
