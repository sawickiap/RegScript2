#include <RegScript2.hpp>
#include <cstddef>

namespace rs2 = RegScript2;

class NestedClass1
{
public:
	rs2::FloatParam ParamA;
};

class MyClass1
{
public:
	rs2::BoolParam Param1;
	rs2::FloatParam Param2;
};

class MyClass2 : public MyClass1
{
public:
	rs2::FloatParam Param3;
	rs2::ClassParam<NestedClass1> NestedClassParam;
};

int main()
{
	rs2::ClassDesc myNestedClassDesc(L"NestedClass1");
	myNestedClassDesc.AddParam(rs2::FloatParamDesc(L"ParamA", offsetof(NestedClass1, ParamA)).SetDefault(10.f));

	rs2::ClassDesc myClass1Desc(L"MyClass1");
	myClass1Desc.AddParam(rs2::BoolParamDesc(L"Param1", offsetof(MyClass1, Param1)).SetDefault(true));
	myClass1Desc.AddParam(rs2::FloatParamDesc(L"Param2", offsetof(MyClass1, Param2)).SetDefault(2.f));

	rs2::ClassDesc myClass2Desc(L"MyClass2", &myClass1Desc);
	myClass2Desc.AddParam(rs2::FloatParamDesc(L"Param3", offsetof(MyClass2, Param3)).SetDefault(3.f));
	myClass2Desc.AddParam(rs2::ClassParamDesc(L"NestedClassParam", offsetof(MyClass2, NestedClassParam), &myNestedClassDesc));

	MyClass2 obj2;
	myClass2Desc.SetObjToDefault(&obj2);

	MyClass2 obj3;
	myClass2Desc.CopyObj(&obj3, &obj2);
}
