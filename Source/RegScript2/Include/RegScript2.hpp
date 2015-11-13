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

class StructDesc;

class BoolParam
{
public:
	bool Value;

	BoolParam() { }
	BoolParam(bool initialValue) : Value(initialValue) { }
};

class UintParam
{
public:
	uint32_t Value;

	UintParam() { }
	UintParam(uint32_t initialValue) : Value(initialValue) { }
};

class FloatParam
{
public:
	float Value;

	FloatParam() { }
	FloatParam(float initialValue) : Value(initialValue) { }
};

class StringParam
{
public:
	std::wstring Value;

	StringParam() { }
	StringParam(const wchar_t* initialValue) : Value(initialValue) { }
	StringParam(const std::wstring& initialValue) : Value(initialValue) { }
};

class GameTimeParam
{
public:
	common::GameTime Value;

	GameTimeParam() { }
	GameTimeParam(const common::GameTime& initialValue) : Value(initialValue) { }
};

class Vec2Param
{
public:
	common::VEC2 Value;

	Vec2Param() { }
	Vec2Param(const common::VEC2& initialValue) : Value(initialValue) { }
};

class Vec3Param
{
public:
	common::VEC3 Value;

	Vec3Param() { }
	Vec3Param(const common::VEC3& initialValue) : Value(initialValue) { }
};

class Vec4Param
{
public:
	common::VEC4 Value;

	Vec4Param() { }
	Vec4Param(const common::VEC4& initialValue) : Value(initialValue) { }
};

template<typename Struct_t>
class StructParam
{
public:
	typedef Struct_t Struct_t;

	// This member must be first because we cast this object directly to value.
	Struct_t Value;

	StructParam() { }
	StructParam(const Struct_t &initialValue) : Value(initialValue) { }
};

template<typename Element_t, size_t Count>
class FixedSizeArrayParam
{
public:
	typedef Element_t Element_t;
	enum { Count = Count };

	Element_t Values[Count];

	FixedSizeArrayParam() { }
	FixedSizeArrayParam(const Element_t* initialValues)
	{
		for(size_t i = 0; i < Count; ++i)
			Values[i] = initialValues[i];
	}
};

class ParamDesc
{
public:
	virtual ~ParamDesc() { }

	virtual size_t GetParamSize() const = 0;
	virtual void SetToDefault(void* param) const = 0;
	virtual void Copy(void* dstParam, const void* srcParam) const = 0;
};

class StructParamDesc : public ParamDesc
{
public:
	StructParamDesc(const StructDesc* structDesc) : m_StructDesc(structDesc)
	{
		assert(structDesc != nullptr);
	}
	const StructDesc* GetStructDesc() const { return m_StructDesc; }

	inline virtual size_t GetParamSize() const;
	inline virtual void SetToDefault(void* param) const;
	inline virtual void Copy(void* dstParam, const void* srcParam) const;

private:
	const StructDesc* m_StructDesc;
};

class FixedSizeArrayParamDesc : public ParamDesc
{
public:
	// Takes ownership of elementParamDesc.
	FixedSizeArrayParamDesc(const ParamDesc* elementParamDesc, size_t count) : m_ElementParamDesc(elementParamDesc), m_Count(count)
	{
		assert(elementParamDesc != nullptr);
	}
	const ParamDesc* GetElementParamDesc() const { return m_ElementParamDesc.get(); }
	size_t GetCount() const { return m_Count; }

	inline virtual size_t GetParamSize() const;
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

	TypedParamDesc() : DefaultValue() { }

private:
};

class BoolParamDesc : public TypedParamDesc<bool>
{
public:
	typedef BoolParam Param_t;

	BoolParamDesc& SetDefault(bool defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual size_t GetParamSize() const { return sizeof(Param_t); }
	virtual void SetToDefault(void* param) const
	{
		((Param_t*)param)->Value = DefaultValue;
	}
	virtual void Copy(void* dstParam, const void* srcParam) const
	{
		((Param_t*)dstParam)->Value = ((Param_t*)srcParam)->Value;
	}
};

class UintParamDesc : public TypedParamDesc<uint32_t>
{
public:
	typedef UintParam Param_t;

	uint32_t Base;

	UintParamDesc() : Base(10) { }
	UintParamDesc& SetDefault(uint32_t defaultValue) { DefaultValue = defaultValue; return *this; }
	UintParamDesc& SetBase(uint32_t base) { Base = base; return *this; }

	virtual size_t GetParamSize() const { return sizeof(Param_t); }
	virtual void SetToDefault(void* param) const
	{
		((Param_t*)param)->Value = DefaultValue;
	}
	virtual void Copy(void* dstParam, const void* srcParam) const
	{
		((Param_t*)dstParam)->Value = ((Param_t*)srcParam)->Value;
	}
};

class FloatParamDesc : public TypedParamDesc<float>
{
public:
	typedef FloatParam Param_t;

	// It only affects the way of displaying value.
	enum FORMAT
	{
		FORMAT_NORMAL,
		FORMAT_PERCENT, // Doesn't limit range to 0..1.
		FORMAT_DB,      // Value must be positive. Otherwise NORMAL is used.
		FORMAT_COUNT
	};
	FORMAT Format;

	FloatParamDesc() : Format(FORMAT_NORMAL) { }
	FloatParamDesc& SetDefault(float defaultValue) { DefaultValue = defaultValue; return *this; }
	FloatParamDesc& SetFormat(FORMAT format) { Format = format; return *this; }

	virtual size_t GetParamSize() const { return sizeof(Param_t); }
	virtual void SetToDefault(void* param) const
	{
		((Param_t*)param)->Value = DefaultValue;
	}
	virtual void Copy(void* dstParam, const void* srcParam) const
	{
		((Param_t*)dstParam)->Value = ((Param_t*)srcParam)->Value;
	}
};

class StringParamDesc : public TypedParamDesc<std::wstring>
{
public:
	typedef StringParam Param_t;

	StringParamDesc& SetDefault(const wchar_t* defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual size_t GetParamSize() const { return sizeof(Param_t); }
	virtual void SetToDefault(void* param) const
	{
		((Param_t*)param)->Value = DefaultValue;
	}
	virtual void Copy(void* dstParam, const void* srcParam) const
	{
		((Param_t*)dstParam)->Value = ((Param_t*)srcParam)->Value;
	}
};

class GameTimeParamDesc : public TypedParamDesc<common::GameTime>
{
public:
	typedef GameTimeParam Param_t;

	GameTimeParamDesc& SetDefault(const common::GameTime& defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual size_t GetParamSize() const { return sizeof(Param_t); }
	virtual void SetToDefault(void* param) const
	{
		((Param_t*)param)->Value = DefaultValue;
	}
	virtual void Copy(void* dstParam, const void* srcParam) const
	{
		((Param_t*)dstParam)->Value = ((Param_t*)srcParam)->Value;
	}
};

class Vec2ParamDesc : public TypedParamDesc<common::VEC2>
{
public:
	typedef Vec2Param Param_t;

	Vec2ParamDesc& SetDefault(const common::VEC2& defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual size_t GetParamSize() const { return sizeof(Param_t); }
	virtual void SetToDefault(void* param) const
	{
		((Param_t*)param)->Value = DefaultValue;
	}
	virtual void Copy(void* dstParam, const void* srcParam) const
	{
		((Param_t*)dstParam)->Value = ((Param_t*)srcParam)->Value;
	}
};

class Vec3ParamDesc : public TypedParamDesc<common::VEC3>
{
public:
	typedef Vec3Param Param_t;

	Vec3ParamDesc& SetDefault(const common::VEC3& defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual size_t GetParamSize() const { return sizeof(Param_t); }
	virtual void SetToDefault(void* param) const
	{
		((Param_t*)param)->Value = DefaultValue;
	}
	virtual void Copy(void* dstParam, const void* srcParam) const
	{
		((Param_t*)dstParam)->Value = ((Param_t*)srcParam)->Value;
	}
};

class Vec4ParamDesc : public TypedParamDesc<common::VEC4>
{
public:
	typedef Vec4Param Param_t;

	Vec4ParamDesc& SetDefault(const common::VEC4& defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual size_t GetParamSize() const { return sizeof(Param_t); }
	virtual void SetToDefault(void* param) const
	{
		((Param_t*)param)->Value = DefaultValue;
	}
	virtual void Copy(void* dstParam, const void* srcParam) const
	{
		((Param_t*)dstParam)->Value = ((Param_t*)srcParam)->Value;
	}
};

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

private:
	std::wstring m_Name;
	size_t m_StructSize;
	const StructDesc* m_BaseStructDesc;
};

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
