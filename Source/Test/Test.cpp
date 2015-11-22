#include <RegScript2.hpp>
#include <RegScript2_TokDoc.hpp>
#include <Common/Tokenizer.hpp>
#include <memory>
#include <cstddef>
#include <gtest/gtest.h>

#ifdef _DEBUG
#pragma comment(lib, "CommonLib_Debug.lib")
#pragma comment(lib, "gtestd.lib")
#else
#pragma comment(lib, "CommonLib_Release.lib")
#pragma comment(lib, "gtest.lib")
#endif

namespace rs2 = RegScript2;

using std::unique_ptr;
using std::wstring;
using common::GameTime;
using common::VEC2;
using common::VEC3;
using common::VEC4;

class Environment : public ::testing::Environment
{
public:
	virtual void SetUp();
	virtual void TearDown();
};

void Environment::SetUp()
{
	common::GameTime::Initialize();
}

void Environment::TearDown()
{
}

class CPrinter : public rs2::IPrinter
{
public:
	virtual void printf(const wchar_t* format, ...);

	bool TextContains(const wchar_t* str) const;

private:
	wstring m_Text;
};

void CPrinter::printf(const wchar_t* format, ...)
{
	va_list argList;
	va_start(argList, format);

	size_t dstLen = (size_t)_vscwprintf(format, argList);
	if(dstLen)
	{
		std::vector<wchar_t> buf(dstLen + 1);
		wchar_t* bufPtr = &buf[0];
		vswprintf(bufPtr, dstLen + 1, format, argList);
		m_Text += bufPtr;
	}
	m_Text += L"\n";

	va_end(argList);
}

bool CPrinter::TextContains(const wchar_t* str) const
{
	return m_Text.find(str) != wstring::npos;
}

TEST(Utils, FriendlyStrToSeconds)
{
	double seconds = 0.;

	EXPECT_TRUE(FriendlyStrToSeconds(seconds, L"0"));
	EXPECT_DOUBLE_EQ(0., seconds);

	EXPECT_TRUE(FriendlyStrToSeconds(seconds, L"0.000"));
	EXPECT_DOUBLE_EQ(0., seconds);

	EXPECT_TRUE(FriendlyStrToSeconds(seconds, L"1.234s"));
	EXPECT_DOUBLE_EQ(1.234, seconds);

	EXPECT_TRUE(FriendlyStrToSeconds(seconds, L"-10e4s"));
	EXPECT_DOUBLE_EQ(-10e4, seconds);

	EXPECT_TRUE(FriendlyStrToSeconds(seconds, L"1.234ms"));
	EXPECT_DOUBLE_EQ(1.234 * 1e-3, seconds);

	EXPECT_TRUE(FriendlyStrToSeconds(seconds, L"-10e4ms"));
	EXPECT_DOUBLE_EQ(-10e4 * 1e-3, seconds);

	EXPECT_TRUE(FriendlyStrToSeconds(seconds, L"1.234us"));
	EXPECT_DOUBLE_EQ(1.234 * 1e-6, seconds);

	EXPECT_TRUE(FriendlyStrToSeconds(seconds, L"-10e4us"));
	EXPECT_DOUBLE_EQ(-10e4 * 1e-6, seconds);

	EXPECT_TRUE(FriendlyStrToSeconds(seconds, L"1.234ns"));
	EXPECT_DOUBLE_EQ(1.234 * 1e-9, seconds);

	EXPECT_TRUE(FriendlyStrToSeconds(seconds, L"-10e4ns"));
	EXPECT_DOUBLE_EQ(-10e4 * 1e-9, seconds);

	EXPECT_TRUE(FriendlyStrToSeconds(seconds, L"2:40"));
	EXPECT_DOUBLE_EQ(2. * 60. + 40., seconds);

	EXPECT_TRUE(FriendlyStrToSeconds(seconds, L"-2:40.5"));
	EXPECT_DOUBLE_EQ(-(2. * 60. + 40.5), seconds);

	EXPECT_TRUE(FriendlyStrToSeconds(seconds, L"100:20:55.5"));
	EXPECT_DOUBLE_EQ((100. * 60. + 20.) * 60. + 55.5, seconds);
}

class SimpleStruct
{
public:
	rs2::BoolParam BoolParam;
	rs2::UintParam UintParam;
	rs2::FloatParam FloatParam;
	rs2::StringParam StringParam;
	rs2::GameTimeParam GameTimeParam;

	void CheckDefaultValues() const;
	void SetCustomValues();
	void CheckCustomValues() const;

	static unique_ptr<rs2::StructDesc> CreateStructDesc();
};

void SimpleStruct::CheckDefaultValues() const
{
	EXPECT_EQ(true, BoolParam.GetConst());
	EXPECT_EQ(123, UintParam.GetConst());
	EXPECT_EQ(3.14f, FloatParam.GetConst());
	wstring str;
	StringParam.GetConst(str);
	EXPECT_EQ(L"StringDefault", str);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), GameTimeParam.GetConst());
}

void SimpleStruct::SetCustomValues()
{
	BoolParam = false;
	UintParam = 124;
	FloatParam = 13.5f;
	StringParam = L"ABC";
	GameTimeParam = common::MillisecondsToGameTime(123);
}

void SimpleStruct::CheckCustomValues() const
{
	EXPECT_FALSE(BoolParam.GetConst());
	EXPECT_EQ(124, UintParam.GetConst());
	EXPECT_EQ(13.5f, FloatParam.GetConst());
	wstring str;
	StringParam.GetConst(str);
	EXPECT_EQ(L"ABC", str);
	EXPECT_EQ(common::MillisecondsToGameTime(123), GameTimeParam.GetConst());
}

unique_ptr<rs2::StructDesc> SimpleStruct::CreateStructDesc()
{
	unique_ptr<rs2::StructDesc> structDesc =
		std::make_unique<rs2::StructDesc>(L"SimpleStruct", sizeof(SimpleStruct));

	structDesc->AddParam(
		L"BoolParam",
		offsetof(SimpleStruct, BoolParam),
		new rs2::BoolParamDesc(rs2::ParamDesc::STORAGE::PARAM, true));
	structDesc->AddParam(
		L"UintParam",
		offsetof(SimpleStruct, UintParam),
		new rs2::UintParamDesc(rs2::ParamDesc::STORAGE::PARAM, 123u));
	structDesc->AddParam(
		L"FloatParam",
		offsetof(SimpleStruct, FloatParam),
		new rs2::FloatParamDesc(rs2::ParamDesc::STORAGE::PARAM, 3.14f));
	structDesc->AddParam(
		L"StringParam",
		offsetof(SimpleStruct, StringParam),
		new rs2::StringParamDesc(rs2::ParamDesc::STORAGE::PARAM, L"StringDefault"));
	structDesc->AddParam(
		L"GameTimeParam",
		offsetof(SimpleStruct, GameTimeParam),
		new rs2::GameTimeParamDesc(rs2::ParamDesc::STORAGE::PARAM, common::MillisecondsToGameTime(1023)));

	return structDesc;
}

class DerivedStruct : public SimpleStruct
{
public:
	rs2::UintParam DerivedUintParam;

	void CheckDefaultValues() const;
	void SetCustomValues();
	void CheckCustomValues() const;

	static unique_ptr<rs2::StructDesc> CreateStructDesc(const rs2::StructDesc* baseStructDesc);
};

void DerivedStruct::CheckDefaultValues() const
{
	SimpleStruct::CheckDefaultValues();
	EXPECT_EQ(555, DerivedUintParam.GetConst());
}

void DerivedStruct::SetCustomValues()
{
	SimpleStruct::SetCustomValues();
	DerivedUintParam = 0xFFFFC0AD;
}

void DerivedStruct::CheckCustomValues() const
{
	SimpleStruct::CheckCustomValues();
	EXPECT_EQ(0xFFFFC0AD, DerivedUintParam.GetConst());
}

unique_ptr<rs2::StructDesc> DerivedStruct::CreateStructDesc(const rs2::StructDesc* baseStructDesc)
{
	unique_ptr<rs2::StructDesc> StructDesc =
		std::make_unique<rs2::StructDesc>(L"DerivedStruct", sizeof(DerivedStruct), baseStructDesc);

	StructDesc->AddParam(
		L"DerivedUintParam",
		offsetof(DerivedStruct, DerivedUintParam),
		new rs2::UintParamDesc(rs2::ParamDesc::STORAGE::PARAM, 555u));

	return StructDesc;
}

class ContainerStruct
{
public:
	SimpleStruct StructParam;
	rs2::UintParam FixedSizeArrayParam[3];

	void CheckDefaultValues() const;
	void SetCustomValues();
	void CheckCustomValues() const;

	static unique_ptr<rs2::StructDesc> CreateStructDesc(const rs2::StructDesc* simpleStructDesc);
};

void ContainerStruct::CheckDefaultValues() const
{
	StructParam.CheckDefaultValues();
	EXPECT_EQ(124, FixedSizeArrayParam[0].GetConst());
	EXPECT_EQ(124, FixedSizeArrayParam[1].GetConst());
	EXPECT_EQ(124, FixedSizeArrayParam[2].GetConst());
}

void ContainerStruct::SetCustomValues()
{
	StructParam.SetCustomValues();
	FixedSizeArrayParam[0] = 0xDEAD;
	FixedSizeArrayParam[1] = 0xDEAE;
	FixedSizeArrayParam[2] = 0xDEAF;
}

void ContainerStruct::CheckCustomValues() const
{
	StructParam.CheckCustomValues();
	EXPECT_EQ(0xDEAD, FixedSizeArrayParam[0].GetConst());
	EXPECT_EQ(0xDEAE, FixedSizeArrayParam[1].GetConst());
	EXPECT_EQ(0xDEAF, FixedSizeArrayParam[2].GetConst());
}

unique_ptr<rs2::StructDesc> ContainerStruct::CreateStructDesc(const rs2::StructDesc* simpleStructDesc)
{
	unique_ptr<rs2::StructDesc> StructDesc =
		std::make_unique<rs2::StructDesc>(L"ContainerStruct", sizeof(ContainerStruct));

	StructDesc->AddParam(
		L"StructParam",
		offsetof(ContainerStruct, StructParam),
		new rs2::StructParamDesc(simpleStructDesc));
	StructDesc->AddParam(
		L"FixedSizeArrayParam",
		offsetof(ContainerStruct, FixedSizeArrayParam),
		new rs2::FixedSizeArrayParamDesc(new rs2::UintParamDesc(rs2::ParamDesc::STORAGE::PARAM, 124), 3));

	return StructDesc;
}

class Fixture1 : public ::testing::Test
{
protected:
	unique_ptr<rs2::StructDesc> m_SimpleStructDesc;
	unique_ptr<rs2::StructDesc> m_DerivedStructDesc;
	unique_ptr<rs2::StructDesc> m_ContainerStructDesc;

	Fixture1();
	~Fixture1() { }
	virtual void SetUp() { }
	virtual void TearDown() { }
};

Fixture1::Fixture1() :
	m_SimpleStructDesc(SimpleStruct::CreateStructDesc()),
	m_DerivedStructDesc(DerivedStruct::CreateStructDesc(m_SimpleStructDesc.get())),
	m_ContainerStructDesc(ContainerStruct::CreateStructDesc(m_SimpleStructDesc.get()))
{
}

TEST_F(Fixture1, SimpleSetDefault)
{
	SimpleStruct obj;
	m_SimpleStructDesc->SetObjToDefault(&obj);
	obj.CheckDefaultValues();
}

TEST_F(Fixture1, SimpleCopyObj)
{
	SimpleStruct obj1, obj2;
	m_SimpleStructDesc->SetObjToDefault(&obj1);
	m_SimpleStructDesc->CopyObj(&obj2, &obj1);
	obj2.CheckDefaultValues();
}

TEST_F(Fixture1, DerivedSetDefault)
{
	DerivedStruct obj;
	m_DerivedStructDesc->SetObjToDefault(&obj);
	obj.CheckDefaultValues();
}

TEST_F(Fixture1, DerivedCopyObj)
{
	DerivedStruct obj1, obj2;
	m_DerivedStructDesc->SetObjToDefault(&obj1);
	m_DerivedStructDesc->CopyObj(&obj2, &obj1);
	obj2.CheckDefaultValues();
}

TEST_F(Fixture1, ContainerSetDefault)
{
	ContainerStruct obj;
	m_ContainerStructDesc->SetObjToDefault(&obj);
	obj.CheckDefaultValues();
}

TEST_F(Fixture1, ContainerCopyObj)
{
	ContainerStruct obj1, obj2;
	m_ContainerStructDesc->SetObjToDefault(&obj1);
	m_ContainerStructDesc->CopyObj(&obj2, &obj1);
	obj2.CheckDefaultValues();
}

TEST_F(Fixture1, SimpleTokDocLoad)
{
	const wchar_t* const DOC =
		L"BoolParam=false;"
		L"UintParam=10056;"
		L"FloatParam=23.67;"
		L"StringParam=\"StringValue\";"
		L"GameTimeParam=10.5;";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	SimpleStruct obj;
	bool ok = rs2::LoadObjFromTokDoc(
		&obj,
		*m_SimpleStructDesc,
		rootNode,
		rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED));
	EXPECT_TRUE(ok);

	EXPECT_EQ(false, obj.BoolParam.GetConst());
	EXPECT_EQ(10056, obj.UintParam.GetConst());
	EXPECT_FLOAT_EQ(23.67f, obj.FloatParam.GetConst());
	wstring str;
	obj.StringParam.GetConst(str);
	EXPECT_EQ(L"StringValue", str);
	EXPECT_EQ(common::SecondsToGameTime(10.5f), obj.GameTimeParam.GetConst());
}

TEST_F(Fixture1, SimpleTokDocLoadAlternative)
{
	const wchar_t* const DOC =
		L"  BoolParam = 0;"
		L" StringParam =\t\"\n\n\\\"\\\\\";"
		L"\tUintParam = 0x2748;"
		L"\t\t  FloatParam = 1.23e5;   \t"
		L"GameTimeParam = -1e-3;  // A comment... ";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	SimpleStruct obj;
	bool ok = rs2::LoadObjFromTokDoc(
		&obj,
		*m_SimpleStructDesc,
		rootNode,
		rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED));
	EXPECT_TRUE(ok);

	EXPECT_EQ(false, obj.BoolParam.GetConst());
	EXPECT_EQ(0x2748, obj.UintParam.GetConst());
	EXPECT_FLOAT_EQ(1.23e5f, obj.FloatParam.GetConst());
	wstring str;
	obj.StringParam.GetConst(str);
	EXPECT_EQ(L"\n\n\"\\", str);
	EXPECT_EQ(common::SecondsToGameTime(-1e-3f), obj.GameTimeParam.GetConst());
}

TEST_F(Fixture1, SimpleTokDocLoadNegativeNotFound)
{
	const wchar_t* const DOC =
		L"BoolParam=false;"
		L"UintParam=10056;"
		L"GameTimeParam=10.5;";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	SimpleStruct obj;
	EXPECT_THROW(
		rs2::LoadObjFromTokDoc(
			&obj,
			*m_SimpleStructDesc,
			rootNode,
			rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED)),
		common::Error);
}

TEST_F(Fixture1, SimpleTokDocLoadNegativeIncorrect)
{
	const wchar_t* const DOC =
		L"BoolParam=false;"
		L"UintParam=10056;"
		L"FloatParam=\"abcd\";"
		L"GameTimeParam=10.5;";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	SimpleStruct obj;
	EXPECT_THROW(
		rs2::LoadObjFromTokDoc(
			&obj,
			*m_SimpleStructDesc,
			rootNode,
			rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED)),
		common::Error);
}

TEST_F(Fixture1, SimpleTokDocLoadNotFoundWarnings)
{
	const wchar_t* const DOC = L"";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	CPrinter printer;
	SimpleStruct obj;
	bool ok = rs2::LoadObjFromTokDoc(
		&obj,
		*m_SimpleStructDesc,
		rootNode,
		rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_OPTIONAL_CORRECT, &printer));
	EXPECT_FALSE(ok);
	EXPECT_TRUE(printer.TextContains(L"BoolParam"));
	EXPECT_TRUE(printer.TextContains(L"UintParam"));
	EXPECT_TRUE(printer.TextContains(L"FloatParam"));
	EXPECT_TRUE(printer.TextContains(L"StringParam"));
	EXPECT_TRUE(printer.TextContains(L"GameTimeParam"));
}

TEST_F(Fixture1, SimpleTokDocLoadInvalidWarnings)
{
	const wchar_t* const DOC =
		L"BoolParam=\"abc\";"
		L"UintParam=\"abc\";"
		L"FloatParam=\"abc\";"
		L"GameTimeParam=\"abc\";";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	CPrinter printer;
	SimpleStruct obj;
	bool ok = rs2::LoadObjFromTokDoc(
		&obj,
		*m_SimpleStructDesc,
		rootNode,
		rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_OPTIONAL, &printer));
	EXPECT_FALSE(ok);
	EXPECT_TRUE(printer.TextContains(L"BoolParam"));
	EXPECT_TRUE(printer.TextContains(L"UintParam"));
	EXPECT_TRUE(printer.TextContains(L"FloatParam"));
	EXPECT_TRUE(printer.TextContains(L"GameTimeParam"));
}

TEST_F(Fixture1, ContainerTokDocLoad)
{
	const wchar_t* const DOC =
		L"StructParam = {"
		L"BoolParam=false;"
		L"UintParam=10056;"
		L"FloatParam=23.67;"
		L"StringParam=\"StringValue\";"
		L"GameTimeParam=10.5;"
		L"};"
		L"FixedSizeArrayParam={9,8,7};";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	ContainerStruct obj;
	bool ok = rs2::LoadObjFromTokDoc(
		&obj,
		*m_ContainerStructDesc,
		rootNode,
		rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED));
	EXPECT_TRUE(ok);

	EXPECT_EQ(false, obj.StructParam.BoolParam.GetConst());
	EXPECT_EQ(10056, obj.StructParam.UintParam.GetConst());
	EXPECT_FLOAT_EQ(23.67f, obj.StructParam.FloatParam.GetConst());
	EXPECT_EQ(common::SecondsToGameTime(10.5f), obj.StructParam.GameTimeParam.GetConst());
	EXPECT_EQ(9, obj.FixedSizeArrayParam[0].GetConst());
	EXPECT_EQ(8, obj.FixedSizeArrayParam[1].GetConst());
	EXPECT_EQ(7, obj.FixedSizeArrayParam[2].GetConst());
}

TEST_F(Fixture1, ContainerTokDocLoadOptionalCorrectDefault)
{
	const wchar_t* const DOC = L"";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	ContainerStruct obj;
	bool ok = rs2::LoadObjFromTokDoc(
		&obj,
		*m_ContainerStructDesc,
		rootNode,
		rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_OPTIONAL_CORRECT | rs2::TOKDOC_FLAG_DEFAULT));
	EXPECT_FALSE(ok);

	EXPECT_EQ(true, obj.StructParam.BoolParam.GetConst());
	EXPECT_EQ(123, obj.StructParam.UintParam.GetConst());
	EXPECT_EQ(3.14f, obj.StructParam.FloatParam.GetConst());
	wstring str;
	obj.StructParam.StringParam.GetConst(str);
	EXPECT_EQ(L"StringDefault", str);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj.StructParam.GameTimeParam.GetConst());
	EXPECT_EQ(124, obj.FixedSizeArrayParam[0].GetConst());
	EXPECT_EQ(124, obj.FixedSizeArrayParam[1].GetConst());
	EXPECT_EQ(124, obj.FixedSizeArrayParam[2].GetConst());
}

TEST_F(Fixture1, ContainerTokDocLoadOptionalIncorrectDefault)
{
	const wchar_t* const DOC =
		L"StructParam = {"
		L"BoolParam=\"dupa\";"
		L"UintParam=\"dupa\";"
		L"FloatParam=\"dupa\";"
		L"GameTimeParam=\"dupa\";"
		L"};"
		L"FixedSizeArrayParam={\"dupa\", \"dupa\"};";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	ContainerStruct obj;
	bool ok = rs2::LoadObjFromTokDoc(
		&obj,
		*m_ContainerStructDesc,
		rootNode,
		rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_OPTIONAL | rs2::TOKDOC_FLAG_DEFAULT));
	EXPECT_FALSE(ok);

	EXPECT_EQ(true, obj.StructParam.BoolParam.GetConst());
	EXPECT_EQ(123, obj.StructParam.UintParam.GetConst());
	EXPECT_EQ(3.14f, obj.StructParam.FloatParam.GetConst());
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj.StructParam.GameTimeParam.GetConst());
	EXPECT_EQ(124, obj.FixedSizeArrayParam[0].GetConst());
	EXPECT_EQ(124, obj.FixedSizeArrayParam[1].GetConst());
	EXPECT_EQ(124, obj.FixedSizeArrayParam[2].GetConst());
}

class MathStruct
{
public:
	rs2::Vec2Param Vec2Param;
	rs2::Vec3Param Vec3Param;
	rs2::Vec4Param Vec4Param;

	void CheckDefaultValues() const;
	void SetCustomValues();
	void CheckCustomValues() const;

	static unique_ptr<rs2::StructDesc> CreateStructDesc();
};

void MathStruct::CheckDefaultValues() const
{
	VEC2 v2;
	VEC3 v3;
	VEC4 v4;
	Vec2Param.GetConst(v2);
	Vec3Param.GetConst(v3);
	Vec4Param.GetConst(v4);
	EXPECT_EQ(VEC2(1.f, 2.f), v2);
	EXPECT_EQ(VEC3(1.f, 2.f, 3.f), v3);
	EXPECT_EQ(VEC4(1.f, 2.f, 3.f, 4.f), v4);
}

void MathStruct::SetCustomValues()
{
	Vec2Param = VEC2(11.f, 22.f);
	Vec3Param = VEC3(11.f, 22.f, 33.f);
	Vec4Param = VEC4(11.f, 22.f, 33.f, 44.f);
}

void MathStruct::CheckCustomValues() const
{
	VEC2 v2;
	VEC3 v3;
	VEC4 v4;
	Vec2Param.GetConst(v2);
	Vec3Param.GetConst(v3);
	Vec4Param.GetConst(v4);
	EXPECT_EQ(VEC2(11.f, 22.f), v2);
	EXPECT_EQ(VEC3(11.f, 22.f, 33.f), v3);
	EXPECT_EQ(VEC4(11.f, 22.f, 33.f, 44.f), v4);
}

unique_ptr<rs2::StructDesc> MathStruct::CreateStructDesc()
{
	unique_ptr<rs2::StructDesc> structDesc =
		std::make_unique<rs2::StructDesc>(L"MathStruct", sizeof(MathStruct));

	structDesc->AddParam(
		L"Vec2Param",
		offsetof(MathStruct, Vec2Param),
		new rs2::Vec2ParamDesc(rs2::ParamDesc::STORAGE::PARAM, VEC2(1.f, 2.f)));
	structDesc->AddParam(
		L"Vec3Param",
		offsetof(MathStruct, Vec3Param),
		new rs2::Vec3ParamDesc(rs2::ParamDesc::STORAGE::PARAM, VEC3(1.f, 2.f, 3.f)));
	structDesc->AddParam(
		L"Vec4Param",
		offsetof(MathStruct, Vec4Param),
		new rs2::Vec4ParamDesc(rs2::ParamDesc::STORAGE::PARAM, VEC4(1.f, 2.f, 3.f, 4.f)));

	return structDesc;
}

TEST(Math, SetObjToDefault)
{
	unique_ptr<rs2::StructDesc> structDesc = MathStruct::CreateStructDesc();
	MathStruct obj;
	structDesc->SetObjToDefault(&obj);
	obj.CheckDefaultValues();
}

TEST(Math, CopyObj)
{
	unique_ptr<rs2::StructDesc> structDesc = MathStruct::CreateStructDesc();
	MathStruct obj1, obj2;
	obj1.SetCustomValues();
	structDesc->CopyObj(&obj2, &obj1);
	obj2.CheckCustomValues();
}

TEST(Math, TokDocLoad)
{
	const wchar_t* const DOC =
		L"Vec2Param={11, 12};"
		L"Vec3Param={11.0, 12.0, 13.0};"
		L"Vec4Param={11., 12., 13., 14.};";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	unique_ptr<rs2::StructDesc> structDesc = MathStruct::CreateStructDesc();
	MathStruct obj;
	bool ok = rs2::LoadObjFromTokDoc(
		&obj,
		*structDesc,
		rootNode,
		rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED));
	EXPECT_TRUE(ok);

	VEC2 v2;
	VEC3 v3;
	VEC4 v4;
	obj.Vec2Param.GetConst(v2);
	obj.Vec3Param.GetConst(v3);
	obj.Vec4Param.GetConst(v4);
	EXPECT_EQ(VEC2(11.f, 12.f), v2);
	EXPECT_EQ(VEC3(11.f, 12.f, 13.f), v3);
	EXPECT_EQ(VEC4(11.f, 12.f, 13.f, 14.f), v4);
}

TEST(Math, TokDocLoadAlternative)
{
	const wchar_t* const DOC =
		L"Vec2Param={ \"11\",\"12\"  };"
		L"Vec3Param={\n11.0,\n12.0,\n\n 13.0\n\t};"
		L"Vec4Param={ \"11.\", \"12.\", \"13.\", \"14.\" };";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	unique_ptr<rs2::StructDesc> structDesc = MathStruct::CreateStructDesc();
	MathStruct obj;
	bool ok = rs2::LoadObjFromTokDoc(
		&obj,
		*structDesc,
		rootNode,
		rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED));
	EXPECT_TRUE(ok);

	VEC2 v2;
	VEC3 v3;
	VEC4 v4;
	obj.Vec2Param.GetConst(v2);
	obj.Vec3Param.GetConst(v3);
	obj.Vec4Param.GetConst(v4);
	EXPECT_EQ(VEC2(11.f, 12.f), v2);
	EXPECT_EQ(VEC3(11.f, 12.f, 13.f), v3);
	EXPECT_EQ(VEC4(11.f, 12.f, 13.f, 14.f), v4);
}

TEST(Math, TokDocLoadRequiredButNotFound)
{
	const wchar_t* const DOC = L"";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	unique_ptr<rs2::StructDesc> structDesc = MathStruct::CreateStructDesc();
	MathStruct obj;
	EXPECT_THROW(
		rs2::LoadObjFromTokDoc(
			&obj,
			*structDesc,
			rootNode,
			rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED)),
		common::Error);
}

// Unfortunately common::tokdoc::NodeTo returns true even if parsing "Foo" as VEC2.
TEST(Math, DISABLED_TokDocLoadOptionalCorrectButNotCorrect)
{
	const wchar_t* const DOC =
		L"Vec2Param=\"Foo\";"
		L"Vec3Param={11.0, 12.0, 13.0};"
		L"Vec4Param={11., 12., 13., 14.};";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	unique_ptr<rs2::StructDesc> structDesc = MathStruct::CreateStructDesc();
	MathStruct obj;
	EXPECT_THROW(
		rs2::LoadObjFromTokDoc(
			&obj,
			*structDesc,
			rootNode,
			rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_OPTIONAL_CORRECT)),
		common::Error);
}

TEST(Math, TokDocLoadOptionalAndNotFound)
{
	const wchar_t* const DOC = L"";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	unique_ptr<rs2::StructDesc> structDesc = MathStruct::CreateStructDesc();
	MathStruct obj;
	CPrinter printer;
	bool ok = rs2::LoadObjFromTokDoc(
		&obj,
		*structDesc,
		rootNode,
		rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_OPTIONAL_CORRECT, &printer));
	EXPECT_FALSE(ok);
	EXPECT_TRUE(printer.TextContains(L"Vec2Param"));
	EXPECT_TRUE(printer.TextContains(L"Vec3Param"));
	EXPECT_TRUE(printer.TextContains(L"Vec4Param"));
}

TEST(TokDoc, SimpleStructTokDocSaveLoad)
{
	unique_ptr<rs2::StructDesc> structDesc = SimpleStruct::CreateStructDesc();
	common::tokdoc::Node rootNode;
	{
		SimpleStruct obj;
		obj.SetCustomValues();
		rs2::SaveObjToTokDoc(rootNode, &obj, *structDesc);
	}
	{
		SimpleStruct obj;
		bool ok = rs2::LoadObjFromTokDoc(&obj, *structDesc, rootNode,
			rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED));
		EXPECT_TRUE(ok);
		obj.CheckCustomValues();
	}
}

TEST(TokDoc, DerivedStructTokDocSaveLoad)
{
	unique_ptr<rs2::StructDesc> simpleStructDesc = SimpleStruct::CreateStructDesc();
	unique_ptr<rs2::StructDesc> derivedStructDesc = DerivedStruct::CreateStructDesc(simpleStructDesc.get());
	common::tokdoc::Node rootNode;
	{
		DerivedStruct obj;
		obj.SetCustomValues();
		rs2::SaveObjToTokDoc(rootNode, &obj, *derivedStructDesc);
	}
	{
		DerivedStruct obj;
		bool ok = rs2::LoadObjFromTokDoc(&obj, *derivedStructDesc, rootNode,
			rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED));
		EXPECT_TRUE(ok);
		obj.CheckCustomValues();
	}
}

TEST(TokDoc, ContainerStructTokDocSaveLoad)
{
	unique_ptr<rs2::StructDesc> simpleStructDesc = SimpleStruct::CreateStructDesc();
	unique_ptr<rs2::StructDesc> containerStructDesc = ContainerStruct::CreateStructDesc(simpleStructDesc.get());
	common::tokdoc::Node rootNode;
	{
		ContainerStruct obj;
		obj.SetCustomValues();
		rs2::SaveObjToTokDoc(rootNode, &obj, *containerStructDesc);
	}
	{
		ContainerStruct obj;
		bool ok = rs2::LoadObjFromTokDoc(&obj, *containerStructDesc, rootNode,
			rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED));
		EXPECT_TRUE(ok);
		obj.CheckCustomValues();
	}
}

TEST(TokDoc, MathStructTokDocSaveLoad)
{
	unique_ptr<rs2::StructDesc> structDesc = MathStruct::CreateStructDesc();
	common::tokdoc::Node rootNode;
	{
		MathStruct obj;
		obj.SetCustomValues();
		rs2::SaveObjToTokDoc(rootNode, &obj, *structDesc);
	}
	{
		MathStruct obj;
		bool ok = rs2::LoadObjFromTokDoc(&obj, *structDesc, rootNode,
			rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED));
		EXPECT_TRUE(ok);
		obj.CheckCustomValues();
	}
}

TEST(TokDoc, SimpleStructTokDocStringSaveLoad)
{
	unique_ptr<rs2::StructDesc> structDesc = SimpleStruct::CreateStructDesc();
	wstring doc;
	{
		SimpleStruct obj;
		obj.SetCustomValues();
		common::tokdoc::Node rootNode;
		rs2::SaveObjToTokDoc(rootNode, &obj, *structDesc);
		common::TokenWriter tokenWriter(&doc);
		rootNode.SaveChildren(tokenWriter);
	}
	{
		common::tokdoc::Node rootNode;
		common::Tokenizer tokenizer(&doc, common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
		SimpleStruct obj;
		bool ok = rs2::LoadObjFromTokDoc(&obj, *structDesc, rootNode,
			rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED));
		EXPECT_TRUE(ok);
		obj.CheckCustomValues();
	}
}

TEST(TokDoc, DerivedStructTokDocStringSaveLoad)
{
	unique_ptr<rs2::StructDesc> simpleStructDesc = SimpleStruct::CreateStructDesc();
	unique_ptr<rs2::StructDesc> derivedStructDesc = DerivedStruct::CreateStructDesc(simpleStructDesc.get());
	wstring doc;
	{
		DerivedStruct obj;
		obj.SetCustomValues();
		common::tokdoc::Node rootNode;
		rs2::SaveObjToTokDoc(rootNode, &obj, *derivedStructDesc);
		common::TokenWriter tokenWriter(&doc);
		rootNode.SaveChildren(tokenWriter);
	}
	{
		common::tokdoc::Node rootNode;
		common::Tokenizer tokenizer(&doc, common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
		DerivedStruct obj;
		bool ok = rs2::LoadObjFromTokDoc(&obj, *derivedStructDesc, rootNode,
			rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED));
		EXPECT_TRUE(ok);
		obj.CheckCustomValues();
	}
}

TEST(TokDoc, ContainerStructTokDocStringSaveLoad)
{
	unique_ptr<rs2::StructDesc> simpleStructDesc = SimpleStruct::CreateStructDesc();
	unique_ptr<rs2::StructDesc> containerStructDesc = ContainerStruct::CreateStructDesc(simpleStructDesc.get());
	wstring doc;
	{
		ContainerStruct obj;
		obj.SetCustomValues();
		common::tokdoc::Node rootNode;
		rs2::SaveObjToTokDoc(rootNode, &obj, *containerStructDesc);
		common::TokenWriter tokenWriter(&doc);
		rootNode.SaveChildren(tokenWriter);
	}
	{
		common::tokdoc::Node rootNode;
		common::Tokenizer tokenizer(&doc, common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
		ContainerStruct obj;
		bool ok = rs2::LoadObjFromTokDoc(&obj, *containerStructDesc, rootNode,
			rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED));
		EXPECT_TRUE(ok);
		obj.CheckCustomValues();
	}
}

TEST(TokDoc, MathStructTokDocStringSaveLoad)
{
	unique_ptr<rs2::StructDesc> structDesc = MathStruct::CreateStructDesc();
	wstring doc;
	{
		MathStruct obj;
		obj.SetCustomValues();
		common::tokdoc::Node rootNode;
		rs2::SaveObjToTokDoc(rootNode, &obj, *structDesc);
		common::TokenWriter tokenWriter(&doc);
		rootNode.SaveChildren(tokenWriter);
	}
	{
		common::tokdoc::Node rootNode;
		common::Tokenizer tokenizer(&doc, common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
		MathStruct obj;
		bool ok = rs2::LoadObjFromTokDoc(&obj, *structDesc, rootNode,
			rs2::STokDocLoadConfig(rs2::TOKDOC_FLAG_REQUIRED));
		EXPECT_TRUE(ok);
		obj.CheckCustomValues();
	}
}

struct PolymorphicBaseStruct
{
	rs2::UintParam BaseUintParam;

	virtual ~PolymorphicBaseStruct() { }
	void SetCustomValues();
	void CheckCustomValues() const;

	static unique_ptr<rs2::StructDesc> CreateStructDesc();
};

void PolymorphicBaseStruct::SetCustomValues()
{
	BaseUintParam = 666;
}

void PolymorphicBaseStruct::CheckCustomValues() const
{
	EXPECT_EQ(666, BaseUintParam.GetConst());
}

unique_ptr<rs2::StructDesc> PolymorphicBaseStruct::CreateStructDesc()
{
	unique_ptr<rs2::StructDesc> structDesc =
		std::make_unique<rs2::StructDesc>(L"PolymorphicBaseStruct", sizeof(PolymorphicBaseStruct));
	structDesc->AddParam(
		L"BaseUintParam",
		offsetof(PolymorphicBaseStruct, BaseUintParam),
		new rs2::UintParamDesc(rs2::ParamDesc::STORAGE::PARAM, 555));
	return structDesc;
}

struct PolymorphicDerivedStruct : public PolymorphicBaseStruct
{
	rs2::UintParam DerivedUintParam;

	void SetCustomValues();
	void CheckCustomValues() const;

	static unique_ptr<rs2::StructDesc> CreateStructDesc(const rs2::StructDesc* baseStructDesc);
};

void PolymorphicDerivedStruct::SetCustomValues()
{
	PolymorphicBaseStruct::SetCustomValues();
	DerivedUintParam = 444;
}

void PolymorphicDerivedStruct::CheckCustomValues() const
{
	PolymorphicBaseStruct::CheckCustomValues();
	EXPECT_EQ(444, DerivedUintParam.GetConst());
}

unique_ptr<rs2::StructDesc> PolymorphicDerivedStruct::CreateStructDesc(const rs2::StructDesc* baseStructDesc)
{
	unique_ptr<rs2::StructDesc> structDesc =
		std::make_unique<rs2::StructDesc>(L"PolymorphicDerivedStruct", sizeof(PolymorphicDerivedStruct), baseStructDesc);
	structDesc->AddParam(
		L"DerivedUintParam",
		offsetof(PolymorphicDerivedStruct, DerivedUintParam),
		new rs2::UintParamDesc(rs2::ParamDesc::STORAGE::PARAM, 333));
	return structDesc;
}

TEST(PolymorphicStruct, SetAndGet)
{
	unique_ptr<rs2::StructDesc> baseStructDesc = PolymorphicBaseStruct::CreateStructDesc();
	unique_ptr<rs2::StructDesc> derivedStructDesc = PolymorphicDerivedStruct::CreateStructDesc(baseStructDesc.get());
	unique_ptr<PolymorphicDerivedStruct> obj = std::make_unique<PolymorphicDerivedStruct>();
	obj->SetCustomValues();
	obj->CheckCustomValues();
}

TEST(StringConversion, Bool)
{
	unique_ptr<rs2::StructDesc> structDesc = SimpleStruct::CreateStructDesc();
	size_t paramIndex = structDesc->Find(L"BoolParam");
	ASSERT_NE((size_t)-1, paramIndex);
	const rs2::ParamDesc* paramDesc = structDesc->GetParamDesc(paramIndex);
	ASSERT_NE(nullptr, paramDesc);

	SimpleStruct s;
	wstring valueStr;
	
	s.BoolParam = false;
	EXPECT_TRUE( paramDesc->ToString(valueStr, &s.BoolParam) );
	EXPECT_EQ(L"false", valueStr);
	
	s.BoolParam = true;
	EXPECT_TRUE( paramDesc->ToString(valueStr, &s.BoolParam) );
	EXPECT_EQ(L"true", valueStr);

	EXPECT_TRUE( paramDesc->Parse(&s.BoolParam, L"false") );
	EXPECT_EQ(false, s.BoolParam.GetConst());

	EXPECT_TRUE( paramDesc->Parse(&s.BoolParam, L"true") );
	EXPECT_EQ(true, s.BoolParam.GetConst());

	EXPECT_TRUE( paramDesc->Parse(&s.BoolParam, L"0") );
	EXPECT_EQ(false, s.BoolParam.GetConst());

	EXPECT_TRUE( paramDesc->Parse(&s.BoolParam, L"1") );
	EXPECT_EQ(true, s.BoolParam.GetConst());
}

TEST(StringConversion, Uint)
{
	unique_ptr<rs2::StructDesc> structDesc = SimpleStruct::CreateStructDesc();
	size_t paramIndex = structDesc->Find(L"UintParam");
	ASSERT_NE((size_t)-1, paramIndex);
	const rs2::ParamDesc* paramDesc = structDesc->GetParamDesc(paramIndex);
	ASSERT_NE(nullptr, paramDesc);

	SimpleStruct s;
	wstring valueStr;

	s.UintParam = 123;
	EXPECT_TRUE( paramDesc->ToString(valueStr, &s.UintParam) );
	EXPECT_EQ(L"123", valueStr);

	EXPECT_TRUE( paramDesc->Parse(&s.UintParam, L"65536") );
	EXPECT_EQ(65536, s.UintParam.GetConst());

	EXPECT_TRUE( paramDesc->Parse(&s.UintParam, L"0xDEADC0DE") );
	EXPECT_EQ(0xDEADC0DE, s.UintParam.GetConst());
}

TEST(StringConversion, Float)
{
	unique_ptr<rs2::StructDesc> structDesc = SimpleStruct::CreateStructDesc();
	size_t paramIndex = structDesc->Find(L"FloatParam");
	ASSERT_NE((size_t)-1, paramIndex);
	const rs2::ParamDesc* paramDesc = structDesc->GetParamDesc(paramIndex);
	ASSERT_NE(nullptr, paramDesc);

	SimpleStruct s;
	wstring valueStr;

	s.FloatParam = 123.f;
	EXPECT_TRUE( paramDesc->ToString(valueStr, &s.FloatParam) );
	EXPECT_EQ(L"123", valueStr);

	s.FloatParam = -6.25f;
	EXPECT_TRUE( paramDesc->ToString(valueStr, &s.FloatParam) );
	EXPECT_EQ(L"-6.25", valueStr);

	EXPECT_TRUE( paramDesc->Parse(&s.FloatParam, L"13.5") );
	EXPECT_FLOAT_EQ(13.5f, s.FloatParam.GetConst());

	EXPECT_TRUE( paramDesc->Parse(&s.FloatParam, L"-1.2345e-3") );
	EXPECT_FLOAT_EQ(-1.2345e-3f, s.FloatParam.GetConst());
}

TEST(StringConversion, String)
{
	unique_ptr<rs2::StructDesc> structDesc = SimpleStruct::CreateStructDesc();
	size_t paramIndex = structDesc->Find(L"StringParam");
	ASSERT_NE((size_t)-1, paramIndex);
	const rs2::ParamDesc* paramDesc = structDesc->GetParamDesc(paramIndex);
	ASSERT_NE(nullptr, paramDesc);

	SimpleStruct s;
	wstring valueStr;

	s.StringParam = L"aaa";
	EXPECT_TRUE( paramDesc->ToString(valueStr, &s.StringParam) );
	EXPECT_EQ(L"aaa", valueStr);

	EXPECT_TRUE( paramDesc->Parse(&s.StringParam, L"\r\n\t<>&%") );
	s.StringParam.GetConst(valueStr);
	EXPECT_EQ(L"\r\n\t<>&%", valueStr);
}

TEST(StringConversion, GameTime)
{
	unique_ptr<rs2::StructDesc> structDesc = SimpleStruct::CreateStructDesc();
	size_t paramIndex = structDesc->Find(L"GameTimeParam");
	ASSERT_NE((size_t)-1, paramIndex);
	const rs2::ParamDesc* paramDesc = structDesc->GetParamDesc(paramIndex);
	ASSERT_NE(nullptr, paramDesc);

	SimpleStruct s;
	wstring valueStr;

	s.GameTimeParam = common::SecondsToGameTime(12.5);
	EXPECT_TRUE( paramDesc->ToString(valueStr, &s.GameTimeParam) );
	EXPECT_EQ(L"12.5s", valueStr);

	EXPECT_TRUE( paramDesc->Parse(&s.GameTimeParam, L"10:25") );
	EXPECT_DOUBLE_EQ(10. * 60. + 25., s.GameTimeParam.GetConst().ToSeconds_d());
}

TEST(StringConversion, Vec2)
{
	unique_ptr<rs2::StructDesc> structDesc = MathStruct::CreateStructDesc();
	size_t paramIndex = structDesc->Find(L"Vec2Param");
	ASSERT_NE((size_t)-1, paramIndex);
	const rs2::ParamDesc* paramDesc = structDesc->GetParamDesc(paramIndex);
	ASSERT_NE(nullptr, paramDesc);

	MathStruct s;
	wstring valueStr;

	s.Vec2Param = VEC2(1.f, 2.f);
	EXPECT_TRUE( paramDesc->ToString(valueStr, &s.Vec2Param) );
	EXPECT_EQ(L"1,2", valueStr);

	EXPECT_TRUE( paramDesc->Parse(&s.Vec2Param, L"-1.25,10") );
	VEC2 v2;
	s.Vec2Param.GetConst(v2);
	EXPECT_FLOAT_EQ(-1.25f, v2.x);
	EXPECT_FLOAT_EQ(10.f, v2.y);
}

TEST(StringConversion, Vec3)
{
	unique_ptr<rs2::StructDesc> structDesc = MathStruct::CreateStructDesc();
	size_t paramIndex = structDesc->Find(L"Vec3Param");
	ASSERT_NE((size_t)-1, paramIndex);
	const rs2::ParamDesc* paramDesc = structDesc->GetParamDesc(paramIndex);
	ASSERT_NE(nullptr, paramDesc);

	MathStruct s;
	wstring valueStr;

	s.Vec3Param = VEC3(1.f, 2.f, 3.f);
	EXPECT_TRUE( paramDesc->ToString(valueStr, &s.Vec3Param) );
	EXPECT_EQ(L"1,2,3", valueStr);

	EXPECT_TRUE( paramDesc->Parse(&s.Vec3Param, L"-1.25,10,0") );
	VEC3 v3;
	s.Vec3Param.GetConst(v3);
	EXPECT_FLOAT_EQ(-1.25f, v3.x);
	EXPECT_FLOAT_EQ(10.f, v3.y);
	EXPECT_FLOAT_EQ(0.f, v3.z);
}

TEST(StringConversion, Vec4)
{
	unique_ptr<rs2::StructDesc> structDesc = MathStruct::CreateStructDesc();
	size_t paramIndex = structDesc->Find(L"Vec4Param");
	ASSERT_NE((size_t)-1, paramIndex);
	const rs2::ParamDesc* paramDesc = structDesc->GetParamDesc(paramIndex);
	ASSERT_NE(nullptr, paramDesc);

	MathStruct s;
	wstring valueStr;

	s.Vec4Param = VEC4(1.f, 2.f, 3.f, -10.f);
	EXPECT_TRUE( paramDesc->ToString(valueStr, &s.Vec4Param) );
	EXPECT_EQ(L"1,2,3,-10", valueStr);

	EXPECT_TRUE( paramDesc->Parse(&s.Vec4Param, L"-1.25,10,0,1e-6") );
	VEC4 v4;
	s.Vec4Param.GetConst(v4);
	EXPECT_FLOAT_EQ(-1.25f, v4.x);
	EXPECT_FLOAT_EQ(10.f, v4.y);
	EXPECT_FLOAT_EQ(0.f, v4.z);
	EXPECT_FLOAT_EQ(1e-6f, v4.w);
}

TEST(FindObjParamByPath, Simple)
{
	unique_ptr<rs2::StructDesc> structDesc = SimpleStruct::CreateStructDesc();
	SimpleStruct s;
	s.UintParam = 123;

	void* param = nullptr;
	const rs2::ParamDesc* paramDesc = nullptr;
	ASSERT_TRUE( rs2::FindObjParamByPath(
		param, paramDesc,
		&s, *structDesc,
		L"UintParam") );
	ASSERT_TRUE(param != nullptr);
	ASSERT_TRUE(paramDesc != nullptr);
	ASSERT_TRUE(typeid(rs2::UintParamDesc) == typeid(*paramDesc));
	rs2::UintParam* uintParam = (rs2::UintParam*)param;
	EXPECT_EQ(123, uintParam->GetConst());
}

TEST(FindObjParamByPath, SubStruct)
{
	unique_ptr<rs2::StructDesc> simpleStructDesc = SimpleStruct::CreateStructDesc();
	unique_ptr<rs2::StructDesc> containerStructDesc = ContainerStruct::CreateStructDesc(simpleStructDesc.get());
	ContainerStruct s;
	s.StructParam.UintParam = 123;

	void* param = nullptr;
	const rs2::ParamDesc* paramDesc = nullptr;
	ASSERT_TRUE( rs2::FindObjParamByPath(
		param, paramDesc,
		&s, *containerStructDesc,
		L"StructParam\\UintParam") );
	ASSERT_TRUE(param != nullptr);
	ASSERT_TRUE(paramDesc != nullptr);
	ASSERT_TRUE(typeid(rs2::UintParamDesc) == typeid(*paramDesc));
	rs2::UintParam* uintParam = (rs2::UintParam*)param;
	EXPECT_EQ(123, uintParam->GetConst());
}

TEST(FindObjParamByPath, FixedSizeArray)
{
	unique_ptr<rs2::StructDesc> simpleStructDesc = SimpleStruct::CreateStructDesc();
	unique_ptr<rs2::StructDesc> containerStructDesc = ContainerStruct::CreateStructDesc(simpleStructDesc.get());
	ContainerStruct s;
	s.FixedSizeArrayParam[2] = 123;

	void* param = nullptr;
	const rs2::ParamDesc* paramDesc = nullptr;
	ASSERT_TRUE( rs2::FindObjParamByPath(
		param, paramDesc,
		&s, *containerStructDesc,
		L"FixedSizeArrayParam[2]") );
	ASSERT_TRUE(param != nullptr);
	ASSERT_TRUE(paramDesc != nullptr);
	ASSERT_TRUE(typeid(rs2::UintParamDesc) == typeid(*paramDesc));
	rs2::UintParam* uintParam = (rs2::UintParam*)param;
	EXPECT_EQ(123, uintParam->GetConst());
}

TEST(FindObjParamByPath, Negative)
{
	unique_ptr<rs2::StructDesc> simpleStructDesc = SimpleStruct::CreateStructDesc();
	unique_ptr<rs2::StructDesc> containerStructDesc = ContainerStruct::CreateStructDesc(simpleStructDesc.get());
	ContainerStruct s;

	void* param = nullptr;
	const rs2::ParamDesc* paramDesc = nullptr;
	EXPECT_FALSE( rs2::FindObjParamByPath(
		param, paramDesc,
		&s, *containerStructDesc,
		L"NonExistingParam") );
	EXPECT_FALSE( rs2::FindObjParamByPath(
		param, paramDesc,
		&s, *containerStructDesc,
		L"StructParam\\") );
	EXPECT_FALSE( rs2::FindObjParamByPath(
		param, paramDesc,
		&s, *containerStructDesc,
		L"\\StructParam") );
	EXPECT_FALSE( rs2::FindObjParamByPath(
		param, paramDesc,
		&s, *containerStructDesc,
		L"FixedSizeArrayParam[1000000]") );
	EXPECT_FALSE( rs2::FindObjParamByPath(
		param, paramDesc,
		&s, *containerStructDesc,
		L"StructParam\\NonExistingParam") );
	EXPECT_FALSE( rs2::FindObjParamByPath(
		param, paramDesc,
		&s, *containerStructDesc,
		L"FixedSizeArrayParam[1") );
	EXPECT_FALSE( rs2::FindObjParamByPath(
		param, paramDesc,
		&s, *containerStructDesc,
		L"FixedSizeArrayParam[1][0]") );
	EXPECT_FALSE( rs2::FindObjParamByPath(
		param, paramDesc,
		&s, *containerStructDesc,
		L"FixedSizeArrayParam[1]\\") );
	EXPECT_FALSE( rs2::FindObjParamByPath(
		param, paramDesc,
		&s, *containerStructDesc,
		L"FixedSizeArrayParam[1]\\NoParam") );
	EXPECT_FALSE( rs2::FindObjParamByPath(
		param, paramDesc,
		&s, *containerStructDesc,
		L"StructParam\\NonExistingParam") );
}

struct RawValuesStruct
{
	bool BoolValue;
	uint32_t UintValue;
	float FloatValue;
	wstring StringValue;
	GameTime GameTimeValue;
	VEC2 Vec2Value;
	VEC3 Vec3Value;
	VEC4 Vec4Value;

	static unique_ptr<rs2::StructDesc> CreateStructDesc(uint32_t additionalFlags = 0);
};

unique_ptr<rs2::StructDesc> RawValuesStruct::CreateStructDesc(uint32_t additionalFlags)
{
	unique_ptr<rs2::StructDesc> structDesc =
		std::make_unique<rs2::StructDesc>(L"RawValuesStruct", sizeof(RawValuesStruct));

	structDesc->AddParam(
		L"BoolValue",
		offsetof(RawValuesStruct, BoolValue),
		new rs2::BoolParamDesc(rs2::ParamDesc::STORAGE::RAW, true));
	structDesc->AddParam(
		L"UintValue",
		offsetof(RawValuesStruct, UintValue),
		new rs2::UintParamDesc(rs2::ParamDesc::STORAGE::RAW, 123, 0, rs2::UintParamDesc::FORMAT_DEC, 100, 200));
	structDesc->AddParam(
		L"FloatValue",
		offsetof(RawValuesStruct, FloatValue),
		new rs2::FloatParamDesc(rs2::ParamDesc::STORAGE::RAW, 3.14f, 0, rs2::FloatParamDesc::FORMAT_NORMAL, 100.f, 200.f));
	structDesc->AddParam(
		L"StringValue",
		offsetof(RawValuesStruct, StringValue),
		new rs2::StringParamDesc(rs2::ParamDesc::STORAGE::RAW, L"StringDefault"));
	structDesc->AddParam(
		L"GameTimeValue",
		offsetof(RawValuesStruct, GameTimeValue),
		new rs2::GameTimeParamDesc(rs2::ParamDesc::STORAGE::RAW,
			common::MillisecondsToGameTime(1023),
			0,
			common::SecondsToGameTime(100.),
			common::SecondsToGameTime(200.)));
	structDesc->AddParam(
		L"Vec2Value",
		offsetof(RawValuesStruct, Vec2Value),
		new rs2::Vec2ParamDesc(rs2::ParamDesc::STORAGE::RAW,
			VEC2(1.f, 2.f),
			0,
			VEC2(100.f, 100.f),
			VEC2(200.f, 200.f)));
	structDesc->AddParam(
		L"Vec3Value",
		offsetof(RawValuesStruct, Vec3Value),
		new rs2::Vec3ParamDesc(rs2::ParamDesc::STORAGE::RAW,
			VEC3(1.f, 2.f, 3.f),
			0,
			VEC3(100.f, 100.f, 100.f),
			VEC3(200.f, 200.f, 200.f)));
	structDesc->AddParam(
		L"Vec4Value",
		offsetof(RawValuesStruct, Vec4Value),
		new rs2::Vec4ParamDesc(rs2::ParamDesc::STORAGE::RAW,
			VEC4(1.f, 2.f, 3.f, 4.f),
			0,
			VEC4(100.f, 100.f, 100.f, 100.f),
			VEC4(200.f, 200.f, 200.f, 200.f)));

	for(auto paramPtr : structDesc->Params)
		paramPtr->Flags |= additionalFlags;

	return structDesc;
}

TEST(RawValues, IsConst)
{
	unique_ptr<rs2::StructDesc> rawValuesStructDesc = RawValuesStruct::CreateStructDesc();
	RawValuesStruct obj;

	rawValuesStructDesc->SetObjToDefault(&obj);

	EXPECT_TRUE(rawValuesStructDesc->Params[0]->IsConst(&obj.BoolValue));
	EXPECT_TRUE(rawValuesStructDesc->Params[1]->IsConst(&obj.UintValue));
	EXPECT_TRUE(rawValuesStructDesc->Params[2]->IsConst(&obj.FloatValue));
	EXPECT_TRUE(rawValuesStructDesc->Params[3]->IsConst(&obj.StringValue));
	EXPECT_TRUE(rawValuesStructDesc->Params[4]->IsConst(&obj.GameTimeValue));
	EXPECT_TRUE(rawValuesStructDesc->Params[5]->IsConst(&obj.Vec2Value));
	EXPECT_TRUE(rawValuesStructDesc->Params[6]->IsConst(&obj.Vec3Value));
	EXPECT_TRUE(rawValuesStructDesc->Params[7]->IsConst(&obj.Vec4Value));
}

TEST(RawValues, SetGetConst)
{
	unique_ptr<rs2::StructDesc> rawValuesStructDesc = RawValuesStruct::CreateStructDesc();
	RawValuesStruct obj;

	const rs2::BoolParamDesc* boolParamDesc = (const rs2::BoolParamDesc*)rawValuesStructDesc->Params[0].get();
	const rs2::UintParamDesc* uintParamDesc = (const rs2::UintParamDesc*)rawValuesStructDesc->Params[1].get();
	const rs2::FloatParamDesc* floatParamDesc = (const rs2::FloatParamDesc*)rawValuesStructDesc->Params[2].get();
	const rs2::StringParamDesc* stringParamDesc = (const rs2::StringParamDesc*)rawValuesStructDesc->Params[3].get();
	const rs2::GameTimeParamDesc* gameTimeParamDesc = (const rs2::GameTimeParamDesc*)rawValuesStructDesc->Params[4].get();
	const rs2::Vec2ParamDesc* vec2ParamDesc = (const rs2::Vec2ParamDesc*)rawValuesStructDesc->Params[5].get();
	const rs2::Vec3ParamDesc* vec3ParamDesc = (const rs2::Vec3ParamDesc*)rawValuesStructDesc->Params[6].get();
	const rs2::Vec4ParamDesc* vec4ParamDesc = (const rs2::Vec4ParamDesc*)rawValuesStructDesc->Params[7].get();

	GameTime gameTime = common::MillisecondsToGameTime(43556);

	boolParamDesc->SetConst(&obj.BoolValue, false);
	uintParamDesc->SetConst(&obj.UintValue, 543);
	floatParamDesc->SetConst(&obj.FloatValue, 12354.f);
	stringParamDesc->SetConst(&obj.StringValue, L"Testing string");
	gameTimeParamDesc->SetConst(&obj.GameTimeValue, gameTime);
	vec2ParamDesc->SetConst(&obj.Vec2Value, VEC2(11.f, 22.f));
	vec3ParamDesc->SetConst(&obj.Vec3Value, VEC3(11.f, 22.f, 33.f));
	vec4ParamDesc->SetConst(&obj.Vec4Value, VEC4(11.f, 22.f, 33.f, 44.f));

	EXPECT_EQ(false, obj.BoolValue);
	EXPECT_EQ(543, obj.UintValue);
	EXPECT_EQ(12354.f, obj.FloatValue);
	EXPECT_EQ(L"Testing string", obj.StringValue);
	EXPECT_EQ(gameTime, obj.GameTimeValue);
	EXPECT_EQ(VEC2(11.f, 22.f), obj.Vec2Value);
	EXPECT_EQ(VEC3(11.f, 22.f, 33.f), obj.Vec3Value);
	EXPECT_EQ(VEC4(11.f, 22.f, 33.f, 44.f), obj.Vec4Value);
}

TEST(RawValues, ReadOnly)
{
	unique_ptr<rs2::StructDesc> rawValuesStructDesc = RawValuesStruct::CreateStructDesc(
		rs2::ParamDesc::FLAG_READ_ONLY);
	RawValuesStruct obj;

	const rs2::BoolParamDesc* boolParamDesc = (const rs2::BoolParamDesc*)rawValuesStructDesc->Params[0].get();
	const rs2::UintParamDesc* uintParamDesc = (const rs2::UintParamDesc*)rawValuesStructDesc->Params[1].get();
	const rs2::FloatParamDesc* floatParamDesc = (const rs2::FloatParamDesc*)rawValuesStructDesc->Params[2].get();
	const rs2::StringParamDesc* stringParamDesc = (const rs2::StringParamDesc*)rawValuesStructDesc->Params[3].get();
	const rs2::GameTimeParamDesc* gameTimeParamDesc = (const rs2::GameTimeParamDesc*)rawValuesStructDesc->Params[4].get();
	const rs2::Vec2ParamDesc* vec2ParamDesc = (const rs2::Vec2ParamDesc*)rawValuesStructDesc->Params[5].get();
	const rs2::Vec3ParamDesc* vec3ParamDesc = (const rs2::Vec3ParamDesc*)rawValuesStructDesc->Params[6].get();
	const rs2::Vec4ParamDesc* vec4ParamDesc = (const rs2::Vec4ParamDesc*)rawValuesStructDesc->Params[7].get();

	GameTime gameTime = common::MillisecondsToGameTime(43556);

	EXPECT_FALSE( boolParamDesc->TrySetConst(&obj.BoolValue, false) );
	EXPECT_FALSE( uintParamDesc->TrySetConst(&obj.UintValue, 543) );
	EXPECT_FALSE( floatParamDesc->TrySetConst(&obj.FloatValue, 12354.f) );
	EXPECT_FALSE( stringParamDesc->TrySetConst(&obj.StringValue, L"Testing string") );
	EXPECT_FALSE( gameTimeParamDesc->TrySetConst(&obj.GameTimeValue, gameTime) );
	EXPECT_FALSE( vec2ParamDesc->TrySetConst(&obj.Vec2Value, VEC2(11.f, 22.f)) );
	EXPECT_FALSE( vec3ParamDesc->TrySetConst(&obj.Vec3Value, VEC3(11.f, 22.f, 33.f)) );
	EXPECT_FALSE( vec4ParamDesc->TrySetConst(&obj.Vec4Value, VEC4(11.f, 22.f, 33.f, 44.f)) );
}

TEST(RawValues, WriteOnly)
{
	unique_ptr<rs2::StructDesc> rawValuesStructDesc = RawValuesStruct::CreateStructDesc(
		rs2::ParamDesc::FLAG_WRITE_ONLY);
	RawValuesStruct obj;

	const rs2::BoolParamDesc* boolParamDesc = (const rs2::BoolParamDesc*)rawValuesStructDesc->Params[0].get();
	const rs2::UintParamDesc* uintParamDesc = (const rs2::UintParamDesc*)rawValuesStructDesc->Params[1].get();
	const rs2::FloatParamDesc* floatParamDesc = (const rs2::FloatParamDesc*)rawValuesStructDesc->Params[2].get();
	const rs2::StringParamDesc* stringParamDesc = (const rs2::StringParamDesc*)rawValuesStructDesc->Params[3].get();
	const rs2::GameTimeParamDesc* gameTimeParamDesc = (const rs2::GameTimeParamDesc*)rawValuesStructDesc->Params[4].get();
	const rs2::Vec2ParamDesc* vec2ParamDesc = (const rs2::Vec2ParamDesc*)rawValuesStructDesc->Params[5].get();
	const rs2::Vec3ParamDesc* vec3ParamDesc = (const rs2::Vec3ParamDesc*)rawValuesStructDesc->Params[6].get();
	const rs2::Vec4ParamDesc* vec4ParamDesc = (const rs2::Vec4ParamDesc*)rawValuesStructDesc->Params[7].get();

	bool b;
	EXPECT_FALSE( boolParamDesc->TryGetConst(b, &obj.BoolValue) );
	uint32_t u;
	EXPECT_FALSE( uintParamDesc->TryGetConst(u, &obj.UintValue) );
	float f;
	EXPECT_FALSE( floatParamDesc->TryGetConst(f, &obj.FloatValue) );
	wstring str;
	EXPECT_FALSE( stringParamDesc->TryGetConst(str, &obj.StringValue) );
	GameTime gameTime;
	EXPECT_FALSE( gameTimeParamDesc->TryGetConst(gameTime, &obj.GameTimeValue) );
	VEC2 v2;
	EXPECT_FALSE( vec2ParamDesc->TryGetConst(v2, &obj.Vec2Value) );
	VEC3 v3;
	EXPECT_FALSE( vec3ParamDesc->TryGetConst(v3, &obj.Vec3Value) );
	VEC4 v4;
	EXPECT_FALSE( vec4ParamDesc->TryGetConst(v4, &obj.Vec4Value) );
}

TEST(RawValues, SetDefault)
{
	unique_ptr<rs2::StructDesc> rawValuesStructDesc = RawValuesStruct::CreateStructDesc();
	RawValuesStruct obj;

	rawValuesStructDesc->SetObjToDefault(&obj);

	EXPECT_EQ(true, obj.BoolValue);
	EXPECT_EQ(123, obj.UintValue);
	EXPECT_EQ(3.14f, obj.FloatValue);
	EXPECT_EQ(L"StringDefault", obj.StringValue);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj.GameTimeValue);
	EXPECT_EQ(VEC2(1.f, 2.f), obj.Vec2Value);
	EXPECT_EQ(VEC3(1.f, 2.f, 3.f), obj.Vec3Value);
	EXPECT_EQ(VEC4(1.f, 2.f, 3.f, 4.f), obj.Vec4Value);
}

TEST(RawValues, Copy)
{
	unique_ptr<rs2::StructDesc> rawValuesStructDesc = RawValuesStruct::CreateStructDesc();
	RawValuesStruct obj1, obj2;

	rawValuesStructDesc->SetObjToDefault(&obj1);
	rawValuesStructDesc->CopyObj(&obj2, &obj1);

	EXPECT_EQ(true, obj2.BoolValue);
	EXPECT_EQ(123, obj2.UintValue);
	EXPECT_EQ(3.14f, obj2.FloatValue);
	EXPECT_EQ(L"StringDefault", obj2.StringValue);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj2.GameTimeValue);
	EXPECT_EQ(VEC2(1.f, 2.f), obj2.Vec2Value);
	EXPECT_EQ(VEC3(1.f, 2.f, 3.f), obj2.Vec3Value);
	EXPECT_EQ(VEC4(1.f, 2.f, 3.f, 4.f), obj2.Vec4Value);
}

TEST(RawValues, MinMaxClampOnSet)
{
	unique_ptr<rs2::StructDesc> rawValuesStructDesc = RawValuesStruct::CreateStructDesc(
		rs2::ParamDesc::FLAG_MINMAX_CLAMP_ON_SET);
	RawValuesStruct obj;

	const rs2::UintParamDesc* uintParamDesc = (const rs2::UintParamDesc*)rawValuesStructDesc->Params[1].get();
	const rs2::FloatParamDesc* floatParamDesc = (const rs2::FloatParamDesc*)rawValuesStructDesc->Params[2].get();
	const rs2::GameTimeParamDesc* gameTimeParamDesc = (const rs2::GameTimeParamDesc*)rawValuesStructDesc->Params[4].get();
	const rs2::Vec2ParamDesc* vec2ParamDesc = (const rs2::Vec2ParamDesc*)rawValuesStructDesc->Params[5].get();
	const rs2::Vec3ParamDesc* vec3ParamDesc = (const rs2::Vec3ParamDesc*)rawValuesStructDesc->Params[6].get();
	const rs2::Vec4ParamDesc* vec4ParamDesc = (const rs2::Vec4ParamDesc*)rawValuesStructDesc->Params[7].get();

	uintParamDesc->SetConst(&obj.UintValue, 10);
	floatParamDesc->SetConst(&obj.FloatValue, 10.f);
	gameTimeParamDesc->SetConst(&obj.GameTimeValue, common::SecondsToGameTime(10.));
	vec2ParamDesc->SetConst(&obj.Vec2Value, VEC2(10.f, 10.f));
	vec3ParamDesc->SetConst(&obj.Vec3Value, VEC3(10.f, 10.f, 10.f));
	vec4ParamDesc->SetConst(&obj.Vec4Value, VEC4(10.f, 10.f, 10.f, 10.f));

	EXPECT_EQ(100, obj.UintValue);
	EXPECT_EQ(100.f, obj.FloatValue);
	EXPECT_EQ(common::SecondsToGameTime(100.), obj.GameTimeValue);
	EXPECT_EQ(VEC2(100.f, 100.f), obj.Vec2Value);
	EXPECT_EQ(VEC3(100.f, 100.f, 100.f), obj.Vec3Value);
	EXPECT_EQ(VEC4(100.f, 100.f, 100.f, 100.f), obj.Vec4Value);

	uintParamDesc->SetConst(&obj.UintValue, 1000);
	floatParamDesc->SetConst(&obj.FloatValue, 1000.f);
	gameTimeParamDesc->SetConst(&obj.GameTimeValue, common::SecondsToGameTime(1000.));
	vec2ParamDesc->SetConst(&obj.Vec2Value, VEC2(1000.f, 1000.f));
	vec3ParamDesc->SetConst(&obj.Vec3Value, VEC3(1000.f, 1000.f, 1000.f));
	vec4ParamDesc->SetConst(&obj.Vec4Value, VEC4(1000.f, 1000.f, 1000.f, 1000.f));

	EXPECT_EQ(200, obj.UintValue);
	EXPECT_EQ(200.f, obj.FloatValue);
	EXPECT_EQ(common::SecondsToGameTime(200.), obj.GameTimeValue);
	EXPECT_EQ(VEC2(200.f, 200.f), obj.Vec2Value);
	EXPECT_EQ(VEC3(200.f, 200.f, 200.f), obj.Vec3Value);
	EXPECT_EQ(VEC4(200.f, 200.f, 200.f, 200.f), obj.Vec4Value);
}

TEST(RawValues, MinMaxFailOnSet)
{
	unique_ptr<rs2::StructDesc> rawValuesStructDesc = RawValuesStruct::CreateStructDesc(
		rs2::ParamDesc::FLAG_MINMAX_FAIL_ON_SET);
	RawValuesStruct obj;

	const rs2::UintParamDesc* uintParamDesc = (const rs2::UintParamDesc*)rawValuesStructDesc->Params[1].get();
	const rs2::FloatParamDesc* floatParamDesc = (const rs2::FloatParamDesc*)rawValuesStructDesc->Params[2].get();
	const rs2::GameTimeParamDesc* gameTimeParamDesc = (const rs2::GameTimeParamDesc*)rawValuesStructDesc->Params[4].get();
	const rs2::Vec2ParamDesc* vec2ParamDesc = (const rs2::Vec2ParamDesc*)rawValuesStructDesc->Params[5].get();
	const rs2::Vec3ParamDesc* vec3ParamDesc = (const rs2::Vec3ParamDesc*)rawValuesStructDesc->Params[6].get();
	const rs2::Vec4ParamDesc* vec4ParamDesc = (const rs2::Vec4ParamDesc*)rawValuesStructDesc->Params[7].get();

	EXPECT_FALSE( uintParamDesc->TrySetConst(&obj.UintValue, 10) );
	EXPECT_FALSE( floatParamDesc->TrySetConst(&obj.FloatValue, 10.f) );
	EXPECT_FALSE( gameTimeParamDesc->TrySetConst(&obj.GameTimeValue, common::SecondsToGameTime(10.)) );
	EXPECT_FALSE( vec2ParamDesc->TrySetConst(&obj.Vec2Value, VEC2(10.f, 10.f)) );
	EXPECT_FALSE( vec3ParamDesc->TrySetConst(&obj.Vec3Value, VEC3(10.f, 10.f, 10.f)) );
	EXPECT_FALSE( vec4ParamDesc->TrySetConst(&obj.Vec4Value, VEC4(10.f, 10.f, 10.f, 10.f)) );
}

TEST(RawValues, MinMaxClampOnGet)
{
	unique_ptr<rs2::StructDesc> rawValuesStructDesc = RawValuesStruct::CreateStructDesc(
		rs2::ParamDesc::FLAG_MINMAX_CLAMP_ON_GET);
	RawValuesStruct obj;

	const rs2::UintParamDesc* uintParamDesc = (const rs2::UintParamDesc*)rawValuesStructDesc->Params[1].get();
	const rs2::FloatParamDesc* floatParamDesc = (const rs2::FloatParamDesc*)rawValuesStructDesc->Params[2].get();
	const rs2::GameTimeParamDesc* gameTimeParamDesc = (const rs2::GameTimeParamDesc*)rawValuesStructDesc->Params[4].get();
	const rs2::Vec2ParamDesc* vec2ParamDesc = (const rs2::Vec2ParamDesc*)rawValuesStructDesc->Params[5].get();
	const rs2::Vec3ParamDesc* vec3ParamDesc = (const rs2::Vec3ParamDesc*)rawValuesStructDesc->Params[6].get();
	const rs2::Vec4ParamDesc* vec4ParamDesc = (const rs2::Vec4ParamDesc*)rawValuesStructDesc->Params[7].get();

	obj.UintValue = 10;
	obj.FloatValue = 10.f;
	obj.GameTimeValue = common::SecondsToGameTime(10.);
	obj.Vec2Value = VEC2(10.f, 10.f);
	obj.Vec3Value = VEC3(10.f, 10.f, 10.f);
	obj.Vec4Value = VEC4(10.f, 10.f, 10.f, 10.f);

	VEC2 v2;
	VEC3 v3;
	VEC4 v4;

	EXPECT_EQ(100, uintParamDesc->GetConst(&obj.UintValue));
	EXPECT_EQ(100.f, floatParamDesc->GetConst(&obj.FloatValue));
	EXPECT_EQ(common::SecondsToGameTime(100.), gameTimeParamDesc->GetConst(&obj.GameTimeValue));
	vec2ParamDesc->GetConst(v2, &obj.Vec2Value);
	vec3ParamDesc->GetConst(v3, &obj.Vec3Value);
	vec4ParamDesc->GetConst(v4, &obj.Vec4Value);
	EXPECT_EQ(VEC2(100.f, 100.f), v2);
	EXPECT_EQ(VEC3(100.f, 100.f, 100.f), v3);
	EXPECT_EQ(VEC4(100.f, 100.f, 100.f, 100.f), v4);

	obj.UintValue = 1000;
	obj.FloatValue = 1000.f;
	obj.GameTimeValue = common::SecondsToGameTime(1000.);
	obj.Vec2Value = VEC2(1000.f, 1000.f);
	obj.Vec3Value = VEC3(1000.f, 1000.f, 1000.f);
	obj.Vec4Value = VEC4(1000.f, 1000.f, 1000.f, 1000.f);

	EXPECT_EQ(200, uintParamDesc->GetConst(&obj.UintValue));
	EXPECT_EQ(200.f, floatParamDesc->GetConst(&obj.FloatValue));
	EXPECT_EQ(common::SecondsToGameTime(200.), gameTimeParamDesc->GetConst(&obj.GameTimeValue));
	vec2ParamDesc->GetConst(v2, &obj.Vec2Value);
	vec3ParamDesc->GetConst(v3, &obj.Vec3Value);
	vec4ParamDesc->GetConst(v4, &obj.Vec4Value);
	EXPECT_EQ(VEC2(200.f, 200.f), v2);
	EXPECT_EQ(VEC3(200.f, 200.f, 200.f), v3);
	EXPECT_EQ(VEC4(200.f, 200.f, 200.f, 200.f), v4);
}

TEST(Funcs, SetObjToDefault_GetSet)
{
	RawValuesStruct obj;

	rs2::StructDesc structDesc(L"FuncStruct", 0);

	rs2::BoolParamDesc* boolParamDesc;
	rs2::UintParamDesc* uintParamDesc;
	rs2::FloatParamDesc* floatParamDesc;
	rs2::StringParamDesc* stringParamDesc;
	rs2::GameTimeParamDesc* gameTimeParamDesc;
	rs2::Vec4ParamDesc* vec4ParamDesc;

	structDesc.AddParam(L"BoolParam", 0, boolParamDesc = new rs2::BoolParamDesc(
		[&obj](bool& outValue, const void* obj) -> bool
		{
			outValue = ((const RawValuesStruct*)obj)->BoolValue; return true;
		},
		[&obj](void* obj, bool value) -> bool
		{
			((RawValuesStruct*)obj)->BoolValue = value; return true;
		},
		true));
	structDesc.AddParam(L"UintParam", 0, uintParamDesc = new rs2::UintParamDesc(
		[&obj](uint& outValue, const void* obj) -> bool
		{
			outValue = ((const RawValuesStruct*)obj)->UintValue; return true;
		},
		[&obj](void* obj, uint32_t value) -> bool
		{
			((RawValuesStruct*)obj)->UintValue = value; return true;
		},
		890));
	structDesc.AddParam(L"FloatParam", 0, floatParamDesc = new rs2::FloatParamDesc(
		[&obj](float& outValue, const void* obj) -> bool
		{
			outValue = ((const RawValuesStruct*)obj)->FloatValue; return true;
		},
		[&obj](void* obj, float value) -> bool
		{
			((RawValuesStruct*)obj)->FloatValue = value; return true;
		},
		10.5f));
	structDesc.AddParam(L"StringParam", 0, stringParamDesc = new rs2::StringParamDesc(
		[&obj](wstring& outValue, const void* obj) -> bool
		{
			outValue = ((const RawValuesStruct*)obj)->StringValue; return true;
		},
		[&obj](void* obj, const wstring& value) -> bool
		{
			((RawValuesStruct*)obj)->StringValue = value; return true;
		},
		L"String default"));
	structDesc.AddParam(L"GameTimeParam", 0, gameTimeParamDesc = new rs2::GameTimeParamDesc(
		[&obj](GameTime& outValue, const void* obj) -> bool
		{
			outValue = ((const RawValuesStruct*)obj)->GameTimeValue; return true;
		},
		[&obj](void* obj, GameTime value) -> bool
		{
			((RawValuesStruct*)obj)->GameTimeValue = value; return true;
		},
		GameTime(-1000000)));
	structDesc.AddParam(L"Vec4Param", 0, vec4ParamDesc = new rs2::Vec4ParamDesc(
		[&obj](VEC4& outValue, const void* obj) -> bool
		{
			outValue = ((const RawValuesStruct*)obj)->Vec4Value; return true;
		},
		[&obj](void* obj, const VEC4& value) -> bool
		{
			((RawValuesStruct*)obj)->Vec4Value = value; return true;
		},
		VEC4(4.f, 3.f, 2.f, 1.f)));

	structDesc.SetObjToDefault(&obj);

	EXPECT_EQ(true, obj.BoolValue);
	EXPECT_EQ(890, obj.UintValue);
	EXPECT_EQ(10.5f, obj.FloatValue);
	EXPECT_EQ(L"String default", obj.StringValue);
	EXPECT_EQ(GameTime(-1000000), obj.GameTimeValue);
	EXPECT_EQ(VEC4(4.f, 3.f, 2.f, 1.f), obj.Vec4Value);

	boolParamDesc->SetConst(&obj, false);
	uintParamDesc->SetConst(&obj, 456);
	floatParamDesc->SetConst(&obj, 256.f);
	stringParamDesc->SetConst(&obj, L"Foo");
	gameTimeParamDesc->SetConst(&obj, GameTime(123));
	vec4ParamDesc->SetConst(&obj, VEC4(1.f, 2.f, 1.f, 54.f));

	EXPECT_EQ(false, obj.BoolValue);
	EXPECT_EQ(456, obj.UintValue);
	EXPECT_EQ(256.f, obj.FloatValue);
	EXPECT_EQ(L"Foo", obj.StringValue);
	EXPECT_EQ(GameTime(123), obj.GameTimeValue);
	EXPECT_EQ(VEC4(1.f, 2.f, 1.f, 54.f), obj.Vec4Value);
}

int wmain(int argc, wchar_t** argv)
{
	::testing::AddGlobalTestEnvironment(new Environment());
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
