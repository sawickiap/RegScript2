#include "PCH.hpp"
#include "Include/RegScript2.hpp"

namespace RegScript2
{

////////////////////////////////////////////////////////////////////////////////
// class ClassDesc


void ClassDesc::SetParamToDefault(void* obj, size_t paramIndex) const
{
	Params[paramIndex]->SetToDefault(AccessRawParam(obj, paramIndex));
}

void ClassDesc::CopyParam(void* dstObj, const void* srcObj, size_t paramIndex) const
{
	Params[paramIndex]->Copy(AccessRawParam(dstObj, paramIndex), AccessRawParam(srcObj, paramIndex));
}

void ClassDesc::SetObjToDefault(void* obj) const
{
	if(m_BaseClassDesc)
		m_BaseClassDesc->SetObjToDefault(obj);
	for(size_t i = 0, count = Params.size(); i < count; ++i)
		SetParamToDefault(obj, i);
}

void ClassDesc::CopyObj(void* dstObj, const void* srcObj) const
{
	if(m_BaseClassDesc)
		m_BaseClassDesc->CopyObj(dstObj, srcObj);
	for(size_t i = 0, count = Params.size(); i < count; ++i)
		CopyParam(dstObj, srcObj, i);
}

////////////////////////////////////////////////////////////////////////////////
// class FixedSizeArrayParamDesc

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

} // namespace RegScript2
