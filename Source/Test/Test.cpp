#include <RegScript2.hpp>
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

class SimpleFixture : public ::testing::Test
{
protected:
	std::unique_ptr<rs2::ClassDesc> m_SimpleClassDesc;

	SimpleFixture();
	~SimpleFixture() { }
	virtual void SetUp() { }
	virtual void TearDown() { }
};

SimpleFixture::SimpleFixture() :
	m_SimpleClassDesc(SimpleClass::CreateClassDesc())
{
}

TEST_F(SimpleFixture, SetDefault)
{
	SimpleClass obj;
	m_SimpleClassDesc->SetObjToDefault(&obj);

	EXPECT_EQ(true, obj.BoolParam.Value);
	EXPECT_EQ(123, obj.UintParam.Value);
	EXPECT_EQ(3.14f, obj.FloatParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj.GameTimeParam.Value);
}

TEST_F(SimpleFixture, CopyObj)
{
	SimpleClass obj1, obj2;
	m_SimpleClassDesc->SetObjToDefault(&obj1);
	m_SimpleClassDesc->CopyObj(&obj2, &obj1);

	EXPECT_EQ(true, obj2.BoolParam.Value);
	EXPECT_EQ(123, obj2.UintParam.Value);
	EXPECT_EQ(3.14f, obj2.FloatParam.Value);
	EXPECT_EQ(common::MillisecondsToGameTime(1023), obj2.GameTimeParam.Value);
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

class ContainerFixture : public ::testing::Test
{
protected:
	std::unique_ptr<rs2::ClassDesc> m_SimpleClassDesc;
	std::unique_ptr<rs2::ClassDesc> m_ContainerClassDesc;

	ContainerFixture();
	~ContainerFixture() { }
	virtual void SetUp() { }
	virtual void TearDown() { }
};

ContainerFixture::ContainerFixture() :
	m_SimpleClassDesc(SimpleClass::CreateClassDesc()),
	m_ContainerClassDesc(ContainerClass::CreateClassDesc(m_SimpleClassDesc.get()))
{
}

TEST_F(ContainerFixture, SetDefault)
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

TEST_F(ContainerFixture, CopyObj)
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

int wmain(int argc, wchar_t** argv)
{
	::testing::AddGlobalTestEnvironment(new Environment());
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
