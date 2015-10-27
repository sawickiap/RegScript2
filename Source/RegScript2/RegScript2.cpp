#include "PCH.hpp"
#include "Include/RegScript2.hpp"

namespace RegScript2
{

////////////////////////////////////////////////////////////////////////////////
// class ClassDesc

void ClassDesc::SetObjToDefault(void* obj) const
{
	if(m_BaseClassDesc)
		m_BaseClassDesc->SetObjToDefault(obj);
	for(size_t i = 0, count = Params.size(); i < count; ++i)
		Params[i]->SetToDefault(obj);
}

void ClassDesc::CopyObj(void* dstObj, const void* srcObj) const
{
	if(m_BaseClassDesc)
		m_BaseClassDesc->CopyObj(dstObj, srcObj);
	for(size_t i = 0, count = Params.size(); i < count; ++i)
		Params[i]->Copy(dstObj, srcObj);
}

} // namespace RegScript2
