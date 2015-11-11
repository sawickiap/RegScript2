#pragma once

#include "RegScript2.hpp"
#include <Common/TokDoc.hpp>

namespace RegScript2
{

void LoadParamFromTokDoc(void* dstParam, const BoolParamDesc& paramDesc, const common::tokdoc::Node& srcNode);
void LoadParamFromTokDoc(void* dstParam, const UintParamDesc& paramDesc, const common::tokdoc::Node& srcNode);
void LoadParamFromTokDoc(void* dstParam, const FloatParamDesc& paramDesc, const common::tokdoc::Node& srcNode);
void LoadParamFromTokDoc(void* dstParam, const GameTimeParamDesc& paramDesc, const common::tokdoc::Node& srcNode);
void LoadParamFromTokDoc(void* dstParam, const ClassParamDesc& paramDesc, const common::tokdoc::Node& srcNode);
void LoadParamFromTokDoc(void* dstParam, const FixedSizeArrayParamDesc& paramDesc, const common::tokdoc::Node& srcNode);
// ADD NEW PARAMETER TYPES HERE.

void LoadParamFromTokDoc(void* dstParam, const ParamDesc& paramDesc, const common::tokdoc::Node& srcNode);

void LoadObjFromTokDoc(void* dstObj, const ClassDesc& classDesc, const common::tokdoc::Node& srcNode);

} // namespace RegScript2
