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

class SimpleStruct
{
public:
	rs2::BoolParam BoolParam;
	rs2::UintParam UintParam;
	rs2::FloatParam FloatParam;
	rs2::StringParam StringParam;
	rs2::GameTimeParam GameTimeParam;

	static unique_ptr<rs2::StructDesc> CreateStructDesc();
};

unique_ptr<rs2::StructDesc> SimpleStruct::CreateStructDesc()
{
	unique_ptr<rs2::StructDesc> structDesc =
		std::make_unique<rs2::StructDesc>(L"SimpleStruct", sizeof(SimpleStruct));

	structDesc->AddParam(
		L"BoolParam",
		offsetof(SimpleStruct, BoolParam),
		rs2::BoolParamDesc().SetDefault(true));
	structDesc->AddParam(
		L"UintParam",
		offsetof(SimpleStruct, UintParam),
		rs2::UintParamDesc().SetDefault(123));
	structDesc->AddParam(
		L"FloatParam",
		offsetof(SimpleStruct, FloatParam),
		rs2::FloatParamDesc().SetDefault(3.14f));
	structDesc->AddParam(
		L"StringParam",
		offsetof(SimpleStruct, StringParam),
		rs2::StringParamDesc().SetDefault(L"StringDefault"));
	structDesc->AddParam(
		L"GameTimeParam",
		offsetof(SimpleStruct, GameTimeParam),
		rs2::GameTimeParamDesc().SetDefault(common::MillisecondsToGameTime(1023)));

	return structDesc;
}

class DerivedStruct : public SimpleStruct
{
public:
	rs2::UintParam DerivedUintParam;

	static unique_ptr<rs2::StructDesc> CreateStructDesc(const rs2::StructDesc* baseStructDesc);
};

unique_ptr<rs2::StructDesc> DerivedStruct::CreateStructDesc(const rs2::StructDesc* baseStructDesc)
{
	unique_ptr<rs2::StructDesc> StructDesc =
		std::make_unique<rs2::StructDesc>(L"DerivedStruct", sizeof(DerivedStruct), baseStructDesc);

	StructDesc->AddParam(
		L"DerivedUintParam",
		offsetof(DerivedStruct, DerivedUintParam),
		rs2::UintParamDesc().SetDefault(555));

	return StructDesc;
}

class ContainerStruct
{
public:
	rs2::StructParam<SimpleStruct> StructParam;
	rs2::FixedSizeArrayParam<rs2::UintParam, 3> FixedSizeArrayParam;

	static unique_ptr<rs2::StructDesc> CreateStructDesc(const rs2::StructDesc* simpleStructDesc);
};

unique_ptr<rs2::StructDesc> ContainerStruct::CreateStructDesc(const rs2::StructDesc* simpleStructDesc)
{
	unique_ptr<rs2::StructDesc> StructDesc =
		std::make_unique<rs2::StructDesc>(L"ContainerStruct", sizeof(ContainerStruct));

	StructDesc->AddParam(
		L"StructParam",
		offsetof(ContainerStruct, StructParam),
		rs2::StructParamDesc(simpleStructDesc));
	StructDesc->AddParam(
		L"FixedSizeArrayParam",
		offsetof(ContainerStruct, FixedSizeArrayParam),
		rs2::FixedSizeArrayParamDesc(new rs2::UintParamDesc(rs2::UintParamDesc().SetDefault(124)), 3));

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

	EXPECT_EQ(true, obj.BoolParam.Value);
	EXPECT_EQ(123, obj.UintParam.Value);
	EXPECT_EQ(3.14f, obj.FloatParam.Value);
	EXPECT_EQ(L"StringDefault", obj.StringParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj.GameTimeParam.Value);
}

TEST_F(Fixture1, SimpleCopyObj)
{
	SimpleStruct obj1, obj2;
	m_SimpleStructDesc->SetObjToDefault(&obj1);
	m_SimpleStructDesc->CopyObj(&obj2, &obj1);

	EXPECT_EQ(true, obj2.BoolParam.Value);
	EXPECT_EQ(123, obj2.UintParam.Value);
	EXPECT_EQ(3.14f, obj2.FloatParam.Value);
	EXPECT_EQ(L"StringDefault", obj2.StringParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj2.GameTimeParam.Value);
}

TEST_F(Fixture1, DerivedSetDefault)
{
	DerivedStruct obj;
	m_DerivedStructDesc->SetObjToDefault(&obj);

	EXPECT_EQ(true, obj.BoolParam.Value);
	EXPECT_EQ(123, obj.UintParam.Value);
	EXPECT_EQ(3.14f, obj.FloatParam.Value);
	EXPECT_EQ(L"StringDefault", obj.StringParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj.GameTimeParam.Value);
	EXPECT_EQ(555, obj.DerivedUintParam.Value);
}

TEST_F(Fixture1, DerivedCopyObj)
{
	DerivedStruct obj1, obj2;
	m_DerivedStructDesc->SetObjToDefault(&obj1);
	m_DerivedStructDesc->CopyObj(&obj2, &obj1);

	EXPECT_EQ(true, obj2.BoolParam.Value);
	EXPECT_EQ(123, obj2.UintParam.Value);
	EXPECT_EQ(3.14f, obj2.FloatParam.Value);
	EXPECT_EQ(L"StringDefault", obj2.StringParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj2.GameTimeParam.Value);
	EXPECT_EQ(555, obj2.DerivedUintParam.Value);
}

TEST_F(Fixture1, ContainerSetDefault)
{
	ContainerStruct obj;
	m_ContainerStructDesc->SetObjToDefault(&obj);

	EXPECT_EQ(true, obj.StructParam.Value.BoolParam.Value);
	EXPECT_EQ(123, obj.StructParam.Value.UintParam.Value);
	EXPECT_EQ(3.14f, obj.StructParam.Value.FloatParam.Value);
	EXPECT_EQ(L"StringDefault", obj.StructParam.Value.StringParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj.StructParam.Value.GameTimeParam.Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[0].Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[1].Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[2].Value);
}

TEST_F(Fixture1, ContainerCopyObj)
{
	ContainerStruct obj1, obj2;
	m_ContainerStructDesc->SetObjToDefault(&obj1);
	m_ContainerStructDesc->CopyObj(&obj2, &obj1);

	EXPECT_EQ(true, obj2.StructParam.Value.BoolParam.Value);
	EXPECT_EQ(123, obj2.StructParam.Value.UintParam.Value);
	EXPECT_EQ(3.14f, obj2.StructParam.Value.FloatParam.Value);
	EXPECT_EQ(L"StringDefault", obj2.StructParam.Value.StringParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj2.StructParam.Value.GameTimeParam.Value);
	EXPECT_EQ(124, obj2.FixedSizeArrayParam.Values[0].Value);
	EXPECT_EQ(124, obj2.FixedSizeArrayParam.Values[1].Value);
	EXPECT_EQ(124, obj2.FixedSizeArrayParam.Values[2].Value);
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

	EXPECT_EQ(false, obj.BoolParam.Value);
	EXPECT_EQ(10056, obj.UintParam.Value);
	EXPECT_FLOAT_EQ(23.67f, obj.FloatParam.Value);
	EXPECT_EQ(L"StringValue", obj.StringParam.Value);
	EXPECT_EQ(common::SecondsToGameTime(10.5f), obj.GameTimeParam.Value);
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

	EXPECT_EQ(false, obj.BoolParam.Value);
	EXPECT_EQ(0x2748, obj.UintParam.Value);
	EXPECT_FLOAT_EQ(1.23e5f, obj.FloatParam.Value);
	EXPECT_EQ(L"\n\n\"\\", obj.StringParam.Value);
	EXPECT_EQ(common::SecondsToGameTime(-1e-3f), obj.GameTimeParam.Value);
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

	EXPECT_EQ(false, obj.StructParam.Value.BoolParam.Value);
	EXPECT_EQ(10056, obj.StructParam.Value.UintParam.Value);
	EXPECT_FLOAT_EQ(23.67f, obj.StructParam.Value.FloatParam.Value);
	EXPECT_EQ(common::SecondsToGameTime(10.5f), obj.StructParam.Value.GameTimeParam.Value);
	EXPECT_EQ(9, obj.FixedSizeArrayParam.Values[0].Value);
	EXPECT_EQ(8, obj.FixedSizeArrayParam.Values[1].Value);
	EXPECT_EQ(7, obj.FixedSizeArrayParam.Values[2].Value);
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

	EXPECT_EQ(true, obj.StructParam.Value.BoolParam.Value);
	EXPECT_EQ(123, obj.StructParam.Value.UintParam.Value);
	EXPECT_EQ(3.14f, obj.StructParam.Value.FloatParam.Value);
	EXPECT_EQ(L"StringDefault", obj.StructParam.Value.StringParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj.StructParam.Value.GameTimeParam.Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[0].Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[1].Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[2].Value);
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

	EXPECT_EQ(true, obj.StructParam.Value.BoolParam.Value);
	EXPECT_EQ(123, obj.StructParam.Value.UintParam.Value);
	EXPECT_EQ(3.14f, obj.StructParam.Value.FloatParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj.StructParam.Value.GameTimeParam.Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[0].Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[1].Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[2].Value);
}

class MathStruct
{
public:
	rs2::Vec2Param Vec2Param;
	rs2::Vec3Param Vec3Param;
	rs2::Vec4Param Vec4Param;

	static unique_ptr<rs2::StructDesc> CreateStructDesc();
};

unique_ptr<rs2::StructDesc> MathStruct::CreateStructDesc()
{
	unique_ptr<rs2::StructDesc> structDesc =
		std::make_unique<rs2::StructDesc>(L"MathStruct", sizeof(MathStruct));

	structDesc->AddParam(
		L"Vec2Param",
		offsetof(MathStruct, Vec2Param),
		rs2::Vec2ParamDesc().SetDefault(VEC2(1.f, 2.f)));
	structDesc->AddParam(
		L"Vec3Param",
		offsetof(MathStruct, Vec3Param),
		rs2::Vec3ParamDesc().SetDefault(VEC3(1.f, 2.f, 3.f)));
	structDesc->AddParam(
		L"Vec4Param",
		offsetof(MathStruct, Vec4Param),
		rs2::Vec4ParamDesc().SetDefault(VEC4(1.f, 2.f, 3.f, 4.f)));

	return structDesc;
}

TEST(Math, SetObjToDefault)
{
	unique_ptr<rs2::StructDesc> structDesc = MathStruct::CreateStructDesc();
	MathStruct obj;
	structDesc->SetObjToDefault(&obj);
	EXPECT_EQ(VEC2(1.f, 2.f), obj.Vec2Param.Value);
	EXPECT_EQ(VEC3(1.f, 2.f, 3.f), obj.Vec3Param.Value);
	EXPECT_EQ(VEC4(1.f, 2.f, 3.f, 4.f), obj.Vec4Param.Value);
}

TEST(Math, CopyObj)
{
	unique_ptr<rs2::StructDesc> structDesc = MathStruct::CreateStructDesc();
	MathStruct obj1, obj2;
	obj1.Vec2Param.Value = VEC2(11.f, 22.f);
	obj1.Vec3Param.Value = VEC3(11.f, 22.f, 33.f);
	obj1.Vec4Param.Value = VEC4(11.f, 22.f, 33.f, 44.f);
	structDesc->CopyObj(&obj2, &obj1);
	EXPECT_EQ(VEC2(11.f, 22.f), obj2.Vec2Param.Value);
	EXPECT_EQ(VEC3(11.f, 22.f, 33.f), obj2.Vec3Param.Value);
	EXPECT_EQ(VEC4(11.f, 22.f, 33.f, 44.f), obj2.Vec4Param.Value);
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

	EXPECT_EQ(VEC2(11.f, 12.f), obj.Vec2Param.Value);
	EXPECT_EQ(VEC3(11.f, 12.f, 13.f), obj.Vec3Param.Value);
	EXPECT_EQ(VEC4(11.f, 12.f, 13.f, 14.f), obj.Vec4Param.Value);
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

	EXPECT_EQ(VEC2(11.f, 12.f), obj.Vec2Param.Value);
	EXPECT_EQ(VEC3(11.f, 12.f, 13.f), obj.Vec3Param.Value);
	EXPECT_EQ(VEC4(11.f, 12.f, 13.f, 14.f), obj.Vec4Param.Value);
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

int wmain(int argc, wchar_t** argv)
{
	::testing::AddGlobalTestEnvironment(new Environment());
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
