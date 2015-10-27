#pragma once

#include <string>
#include <vector>
#include <memory>

#include <cassert>

namespace RegScript2
{

class ClassDesc;

class BoolParam
{
public:
	bool Value;
};

class FloatParam
{
public:
	float Value;
};

template<typename Class_t>
class ClassParam
{
public:
	// This member must be first because we cast this object directly to value.
	Class_t Value;
};

template<typename Element_t, size_t Size>
class FixedSizeArrayParam
{
public:
	Element_t Values[Size];
};

class ParamDesc
{
public:
	ParamDesc(const wchar_t* name, size_t offset) : m_Name(name), m_Offset(offset) { }
	virtual ~ParamDesc() { }
	const wchar_t* GetName() const { return m_Name.c_str(); }
	size_t GetOffset() const { return m_Offset; }

	virtual void SetToDefault(void* obj) const = 0;
	virtual void Copy(void* dstObj, const void* srcObj) const = 0;

protected:
	char* AccessRawParam(void* obj) const { return (char*)obj + GetOffset(); }
	const char* AccessRawParam(const void* obj) const { return (const char*)obj + GetOffset(); }

	template<typename Param_t>
	Param_t* AccessParam(void* obj) const { return (Param_t*)AccessRawParam(obj); }
	template<typename Param_t>
	const Param_t* AccessParam(const void* obj) const { return (const Param_t*)AccessRawParam(obj); }

private:
	std::wstring m_Name;
	size_t m_Offset;
};

class ClassParamDesc : public ParamDesc
{
public:
	ClassParamDesc(const wchar_t* name, size_t offset, const ClassDesc* classDesc) : ParamDesc(name, offset), m_ClassDesc(classDesc)
	{
		assert(classDesc != nullptr);
	}
	const ClassDesc* GetClassDesc() const { return m_ClassDesc; }

	inline virtual void SetToDefault(void* obj) const;
	inline virtual void Copy(void* dstObj, const void* srcObj) const;

private:
	const ClassDesc* m_ClassDesc;
};

class FixedSizeArrayParamDesc : public ParamDesc
{
public:
	// Takes ownership of elementParamDesc.
	FixedSizeArrayParamDesc(const wchar_t* name, size_t offset, const ParamDesc* elementParamDesc, size_t size) : ParamDesc(name, offset), m_ElementParamDesc(elementParamDesc), m_Size(size)
	{
		assert(elementParamDesc != nullptr);
	}
	const ParamDesc* GetElementParamDesc() const { return m_ElementParamDesc.get(); }
	size_t GetSize() const { return m_Size; }

	inline virtual void SetToDefault(void* obj) const;
	inline virtual void Copy(void* dstObj, const void* srcObj) const;

private:
	std::unique_ptr<const ParamDesc> m_ElementParamDesc;
	size_t m_Size;
};

template<typename Value_t>
class TypedParamDesc : public ParamDesc
{
public:
	Value_t DefaultValue;

	TypedParamDesc(const wchar_t* name, size_t offset) : ParamDesc(name, offset), DefaultValue() { }

private:
};

class BoolParamDesc : public TypedParamDesc<bool>
{
public:
	BoolParamDesc(const wchar_t* name, size_t offset) : TypedParamDesc<bool>(name, offset) { }

	BoolParamDesc& SetDefault(bool defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual void SetToDefault(void* obj) const
	{
		AccessParam<BoolParam>(obj)->Value = DefaultValue;
	}
	virtual void Copy(void* dstObj, const void* srcObj) const
	{
		AccessParam<BoolParam>(dstObj)->Value = AccessParam<BoolParam>(srcObj)->Value;
	}
};

class FloatParamDesc : public TypedParamDesc<float>
{
public:
	FloatParamDesc(const wchar_t* name, size_t offset) : TypedParamDesc<float>(name, offset) { }

	FloatParamDesc& SetDefault(float defaultValue) { DefaultValue = defaultValue; return *this; }

	virtual void SetToDefault(void* obj) const
	{
		AccessParam<FloatParam>(obj)->Value = DefaultValue;
	}
	virtual void Copy(void* dstObj, const void* srcObj) const
	{
		AccessParam<FloatParam>(dstObj)->Value = AccessParam<FloatParam>(srcObj)->Value;
	}
};

class ClassDesc
{
public:
	std::vector<std::shared_ptr<ParamDesc>> Params;

	ClassDesc(const wchar_t* name, const ClassDesc* baseClassDesc = nullptr) : m_Name(name), m_BaseClassDesc(baseClassDesc) { }
	const wchar_t* GetName() const { return m_Name.c_str(); }
	const ClassDesc* GetBaseClassDesc() const { return m_BaseClassDesc; }

	template<typename ParamDesc_t>
	void AddParam(const ParamDesc_t& param)
	{
		Params.emplace_back(std::make_shared<ParamDesc_t>(param));
	}

	void SetObjToDefault(void* obj) const;
	void CopyObj(void* dstObj, const void* srcObj) const;

private:
	std::wstring m_Name;
	const ClassDesc* m_BaseClassDesc;
};

inline void ClassParamDesc::SetToDefault(void* obj) const
{
	m_ClassDesc->SetObjToDefault(AccessRawParam(obj));
}

inline void ClassParamDesc::Copy(void* dstObj, const void* srcObj) const
{
	m_ClassDesc->CopyObj(AccessRawParam(dstObj), AccessRawParam(srcObj));
}

inline void FixedSizeArrayParamDesc::SetToDefault(void* obj) const
{
	for(size_t i = 0; i < m_Size; ++i)
		m_ElementParamDesc->SetToDefault(xxx);
}

inline void FixedSizeArrayParamDesc::Copy(void* dstObj, const void* srcObj) const
{
	for(size_t i = 0; i < m_Size; ++i)
		m_ElementParamDesc->CopyObj(xxx, yyy);
}

} // namespace RegScript2
