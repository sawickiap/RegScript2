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

class SimpleClass
{
public:
	rs2::BoolParam BoolParam;
	rs2::UintParam UintParam;
	rs2::FloatParam FloatParam;
	rs2::GameTimeParam GameTimeParam;

	static std::unique_ptr<rs2::ClassDesc> CreateClassDesc();
};

std::unique_ptr<rs2::ClassDesc> SimpleClass::CreateClassDesc()
{
	std::unique_ptr<rs2::ClassDesc> classDesc =
		std::make_unique<rs2::ClassDesc>(L"SimpleClass", sizeof(SimpleClass));

	classDesc->AddParam(
		L"BoolParam",
		offsetof(SimpleClass, BoolParam),
		rs2::BoolParamDesc().SetDefault(true));
	classDesc->AddParam(
		L"UintParam",
		offsetof(SimpleClass, UintParam),
		rs2::UintParamDesc().SetDefault(123));
	classDesc->AddParam(
		L"FloatParam",
		offsetof(SimpleClass, FloatParam),
		rs2::FloatParamDesc().SetDefault(3.14f));
	classDesc->AddParam(
		L"GameTimeParam",
		offsetof(SimpleClass, GameTimeParam),
		rs2::GameTimeParamDesc().SetDefault(common::MillisecondsToGameTime(1023)));

	return classDesc;
}

class ContainerClass
{
public:
	rs2::ClassParam<SimpleClass> ClassParam;
	rs2::FixedSizeArrayParam<rs2::UintParam, 3> FixedSizeArrayParam;

	static std::unique_ptr<rs2::ClassDesc> CreateClassDesc(const rs2::ClassDesc* simpleClassDesc);
};

std::unique_ptr<rs2::ClassDesc> ContainerClass::CreateClassDesc(const rs2::ClassDesc* simpleClassDesc)
{
	std::unique_ptr<rs2::ClassDesc> classDesc =
		std::make_unique<rs2::ClassDesc>(L"ContainerClass", sizeof(ContainerClass));

	classDesc->AddParam(
		L"ClassParam",
		offsetof(ContainerClass, ClassParam),
		rs2::ClassParamDesc(simpleClassDesc));
	classDesc->AddParam(
		L"FixedSizeArrayParam",
		offsetof(ContainerClass, FixedSizeArrayParam),
		rs2::FixedSizeArrayParamDesc(new rs2::UintParamDesc(rs2::UintParamDesc().SetDefault(124)), 3));

	return classDesc;
}

class Fixture1 : public ::testing::Test
{
protected:
	std::unique_ptr<rs2::ClassDesc> m_SimpleClassDesc;
	std::unique_ptr<rs2::ClassDesc> m_ContainerClassDesc;

	Fixture1();
	~Fixture1() { }
	virtual void SetUp() { }
	virtual void TearDown() { }
};

Fixture1::Fixture1() :
	m_SimpleClassDesc(SimpleClass::CreateClassDesc()),
	m_ContainerClassDesc(ContainerClass::CreateClassDesc(m_SimpleClassDesc.get()))
{
}

TEST_F(Fixture1, SimpleSetDefault)
{
	SimpleClass obj;
	m_SimpleClassDesc->SetObjToDefault(&obj);

	EXPECT_EQ(true, obj.BoolParam.Value);
	EXPECT_EQ(123, obj.UintParam.Value);
	EXPECT_EQ(3.14f, obj.FloatParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj.GameTimeParam.Value);
}

TEST_F(Fixture1, SimpleCopyObj)
{
	SimpleClass obj1, obj2;
	m_SimpleClassDesc->SetObjToDefault(&obj1);
	m_SimpleClassDesc->CopyObj(&obj2, &obj1);

	EXPECT_EQ(true, obj2.BoolParam.Value);
	EXPECT_EQ(123, obj2.UintParam.Value);
	EXPECT_EQ(3.14f, obj2.FloatParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj2.GameTimeParam.Value);
}

TEST_F(Fixture1, ContainerSetDefault)
{
	ContainerClass obj;
	m_ContainerClassDesc->SetObjToDefault(&obj);

	EXPECT_EQ(true, obj.ClassParam.Value.BoolParam.Value);
	EXPECT_EQ(123, obj.ClassParam.Value.UintParam.Value);
	EXPECT_EQ(3.14f, obj.ClassParam.Value.FloatParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj.ClassParam.Value.GameTimeParam.Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[0].Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[1].Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[2].Value);
}

TEST_F(Fixture1, ContainerCopyObj)
{
	ContainerClass obj1, obj2;
	m_ContainerClassDesc->SetObjToDefault(&obj1);
	m_ContainerClassDesc->CopyObj(&obj2, &obj1);

	EXPECT_EQ(true, obj2.ClassParam.Value.BoolParam.Value);
	EXPECT_EQ(123, obj2.ClassParam.Value.UintParam.Value);
	EXPECT_EQ(3.14f, obj2.ClassParam.Value.FloatParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj2.ClassParam.Value.GameTimeParam.Value);
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
		L"GameTimeParam=10.5;";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	SimpleClass obj;
	bool ok = rs2::LoadObjFromTokDoc(&obj, *m_SimpleClassDesc, rootNode, rs2::TOKDOC_FLAG_REQUIRED);
	EXPECT_TRUE(ok);

	EXPECT_EQ(false, obj.BoolParam.Value);
	EXPECT_EQ(10056, obj.UintParam.Value);
	EXPECT_FLOAT_EQ(23.67f, obj.FloatParam.Value);
	EXPECT_EQ(common::SecondsToGameTime(10.5f), obj.GameTimeParam.Value);
}

TEST_F(Fixture1, SimpleTokDocLoadAlternative)
{
	const wchar_t* const DOC =
		L"  BoolParam = 0;"
		L"\tUintParam = 0x2748;"
		L"\t\t  FloatParam = 1.23e5;   \t"
		L"GameTimeParam = -1e-3;  // A comment... ";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	SimpleClass obj;
	bool ok = rs2::LoadObjFromTokDoc(&obj, *m_SimpleClassDesc, rootNode, rs2::TOKDOC_FLAG_REQUIRED);
	EXPECT_TRUE(ok);

	EXPECT_EQ(false, obj.BoolParam.Value);
	EXPECT_EQ(0x2748, obj.UintParam.Value);
	EXPECT_FLOAT_EQ(1.23e5f, obj.FloatParam.Value);
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

	SimpleClass obj;
	EXPECT_THROW(
		rs2::LoadObjFromTokDoc(&obj, *m_SimpleClassDesc, rootNode, rs2::TOKDOC_FLAG_REQUIRED),
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

	SimpleClass obj;
	EXPECT_THROW(
		rs2::LoadObjFromTokDoc(&obj, *m_SimpleClassDesc, rootNode, rs2::TOKDOC_FLAG_REQUIRED),
		common::Error);
}

TEST_F(Fixture1, ContainerTokDocLoad)
{
	const wchar_t* const DOC =
		L"ClassParam = {"
		L"BoolParam=false;"
		L"UintParam=10056;"
		L"FloatParam=23.67;"
		L"GameTimeParam=10.5;"
		L"};"
		L"FixedSizeArrayParam={9,8,7};";

	common::tokdoc::Node rootNode;
	{
		common::Tokenizer tokenizer(DOC, wcslen(DOC), common::Tokenizer::FLAG_MULTILINE_STRINGS);
		tokenizer.Next();
		rootNode.LoadChildren(tokenizer);
	}

	ContainerClass obj;
	bool ok = rs2::LoadObjFromTokDoc(&obj, *m_ContainerClassDesc, rootNode, rs2::TOKDOC_FLAG_REQUIRED);
	EXPECT_TRUE(ok);

	EXPECT_EQ(false, obj.ClassParam.Value.BoolParam.Value);
	EXPECT_EQ(10056, obj.ClassParam.Value.UintParam.Value);
	EXPECT_FLOAT_EQ(23.67f, obj.ClassParam.Value.FloatParam.Value);
	EXPECT_EQ(common::SecondsToGameTime(10.5f), obj.ClassParam.Value.GameTimeParam.Value);
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

	ContainerClass obj;
	bool ok = rs2::LoadObjFromTokDoc(&obj, *m_ContainerClassDesc, rootNode,
		rs2::TOKDOC_FLAG_OPTIONAL_CORRECT | rs2::TOKDOC_FLAG_DEFAULT);
	EXPECT_FALSE(ok);

	EXPECT_EQ(true, obj.ClassParam.Value.BoolParam.Value);
	EXPECT_EQ(123, obj.ClassParam.Value.UintParam.Value);
	EXPECT_EQ(3.14f, obj.ClassParam.Value.FloatParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj.ClassParam.Value.GameTimeParam.Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[0].Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[1].Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[2].Value);
}

TEST_F(Fixture1, ContainerTokDocLoadOptionalIncorrectDefault)
{
	const wchar_t* const DOC =
		L"ClassParam = {"
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

	ContainerClass obj;
	bool ok = rs2::LoadObjFromTokDoc(&obj, *m_ContainerClassDesc, rootNode,
		rs2::TOKDOC_FLAG_OPTIONAL | rs2::TOKDOC_FLAG_DEFAULT);
	EXPECT_FALSE(ok);

	EXPECT_EQ(true, obj.ClassParam.Value.BoolParam.Value);
	EXPECT_EQ(123, obj.ClassParam.Value.UintParam.Value);
	EXPECT_EQ(3.14f, obj.ClassParam.Value.FloatParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj.ClassParam.Value.GameTimeParam.Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[0].Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[1].Value);
	EXPECT_EQ(124, obj.FixedSizeArrayParam.Values[2].Value);
}

int wmain(int argc, wchar_t** argv)
{
	::testing::AddGlobalTestEnvironment(new Environment());
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
