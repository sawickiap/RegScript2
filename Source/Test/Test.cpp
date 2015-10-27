#include <RegScript2.hpp>
#include <cstddef>

#ifdef _DEBUG
#pragma comment(lib, "CommonLib_Debug.lib")
#else
#pragma comment(lib, "CommonLib_Release.lib")
#endif

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
	rs2::FixedSizeArrayParam<NestedClass1, 3> NestedArray;
};

int main()
{
	rs2::ClassDesc myNestedClassDesc(L"NestedClass1", sizeof(NestedClass1));
	myNestedClassDesc.AddParam(L"ParamA", offsetof(NestedClass1, ParamA), rs2::FloatParamDesc().SetDefault(10.f));

	rs2::ClassDesc myClass1Desc(L"MyClass1", sizeof(MyClass1));
	myClass1Desc.AddParam(L"Param1", offsetof(MyClass1, Param1), rs2::BoolParamDesc().SetDefault(true));
	myClass1Desc.AddParam(L"Param2", offsetof(MyClass1, Param2), rs2::FloatParamDesc().SetDefault(2.f));

	rs2::ClassDesc myClass2Desc(L"MyClass2", sizeof(MyClass2), &myClass1Desc);
	myClass2Desc.AddParam(L"Param3", offsetof(MyClass2, Param3), rs2::FloatParamDesc().SetDefault(3.f));
	myClass2Desc.AddParam(L"NestedClassParam", offsetof(MyClass2, NestedClassParam), rs2::ClassParamDesc(&myNestedClassDesc));
	myClass2Desc.AddParam(L"NestedArray", offsetof(MyClass2, NestedArray), rs2::FixedSizeArrayParamDesc(new rs2::ClassParamDesc(&myNestedClassDesc), 3));

	MyClass2 obj2;
	myClass2Desc.SetObjToDefault(&obj2);

	MyClass2 obj3;
	myClass2Desc.CopyObj(&obj3, &obj2);
}
