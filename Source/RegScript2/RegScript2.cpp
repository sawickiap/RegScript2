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

} // namespace RegScript2
