// Tolerance.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <assert.h>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>
#include <map>
#include <array>
#include <tuple>
#include <iostream>
#include <iomanip>
#include "tolerance.h"
#include "result.h"

using namespace std;

#if _MSC_VER < 1700
map<string, INSP_RESULT_ID> make_result_ids()
{
	pair<string, INSP_RESULT_ID> _resultIds[] =
	{
		make_pair("Ball Height",	INSP_FAIL_BALL_HEIGHT),
		make_pair("Coplan",			INSP_FAIL_BALL_COPLAN),
		make_pair("Ball Pitch",		INSP_FAIL_BALL_PITCH),
		make_pair("Ball Quality",	INSP_FAIL_BALL_QUALITY),
		make_pair("Warpage",		INSP_FAIL_WARPAGE),
		make_pair("Pad Size",		INSP_FAIL_PAD_SIZE),
		make_pair("Matrix Code",	INSP_FAIL_MATRIX_CODE),
		make_pair("PVI Defect1",	INSP_FAIL_PVI_DEFECT1)
	};

	return map<string, INSP_RESULT_ID>(begin(_resultIds), end(_resultIds));
}

map<int, ResultFormat> make_result_formats()
{
	auto make_result_format = [](	ResultFormat::ERejectType rejectType) 
									-> ResultFormat
	{
		ResultFormat resultFormat = {rejectType};
		return resultFormat;
	};

	pair<int, ResultFormat> _resultFormats[] =
	{
		make_pair(INSP_FAIL_BALL_HEIGHT,	make_result_format(ResultFormat::RT_Measure)),
		make_pair(INSP_FAIL_BALL_COPLAN,	make_result_format(ResultFormat::RT_Measure)),
		make_pair(INSP_FAIL_BALL_PITCH,		make_result_format(ResultFormat::RT_Measure)),
		make_pair(INSP_FAIL_BALL_QUALITY,	make_result_format(ResultFormat::RT_Measure)),
		make_pair(INSP_FAIL_WARPAGE,		make_result_format(ResultFormat::RT_Measure)),
		make_pair(INSP_FAIL_PAD_SIZE,		make_result_format(ResultFormat::RT_Measure)),
		make_pair(INSP_FAIL_MATRIX_CODE,	make_result_format(ResultFormat::RT_Text)),
		make_pair(INSP_FAIL_PVI_DEFECT1,	make_result_format(ResultFormat::RT_PVI))
	};

	return map<int, ResultFormat>(begin(_resultFormats), end(_resultFormats));
}
#endif

static const map<string, INSP_RESULT_ID> g_resultIds =
#if _MSC_VER < 1700
	make_result_ids();
#else
{
	{"Ball Height",		INSP_FAIL_BALL_HEIGHT	},
	{"Coplan",			INSP_FAIL_BALL_COPLAN	},
	{"Ball Pitch",		INSP_FAIL_BALL_PITCH	},
	{"Ball Quality",	INSP_FAIL_BALL_QUALITY	},
	{"Warpage",			INSP_FAIL_WARPAGE		},
	{"Pad Size",		INSP_FAIL_PAD_SIZE		},
	{"Matrix Code",		INSP_FAIL_MATRIX_CODE	},
	{"PVI Defect1",		INSP_FAIL_PVI_DEFECT1	}
};
#endif

static const map<int, ResultFormat> g_resultFormats = 
#if _MSC_VER < 1700
	make_result_formats();
#else
	{
		{ INSP_FAIL_BALL_HEIGHT,	{ ResultFormat::RT_Measure	}),
		{ INSP_FAIL_BALL_COPLAN,	{ ResultFormat::RT_Measure	}),
		{ INSP_FAIL_BALL_PITCH,		{ ResultFormat::RT_Measure	}),
		{ INSP_FAIL_BALL_QUALITY,	{ ResultFormat::RT_Measure	}),
		{ INSP_FAIL_WARPAGE,		{ ResultFormat::RT_Measure	}),
		{ INSP_FAIL_PAD_SIZE,		{ ResultFormat::RT_Measure	}),
		{ INSP_FAIL_MATRIX_CODE,	{ ResultFormat::RT_Text		}),
		{ INSP_FAIL_PVI_DEFECT1,	{ ResultFormat::RT_PVI		})
	};
#endif

void TestMinMax()
{
	vector<CToleranceBase*> tolerances;

	CToleranceDev tol1("CToleranceDev", "", 5.0, 100.0);
	tolerances.push_back(&tol1);
	assert(tol1.CheckTolerance(5.0));
	assert(tol1.CheckTolerance(100.0));
	assert(!tol1.CheckTolerance(4.9));
	assert(!tol1.CheckTolerance(100.1));

	CToleranceMin tol2("CToleranceMin", "", 5.0);
	tolerances.push_back(&tol2);
	assert(tol2.CheckTolerance(5.0));
	assert(!tol2.CheckTolerance(4.9));

	CToleranceMax tol3("CToleranceMax", "", 100.0);
	tolerances.push_back(&tol3);
	assert(tol3.CheckTolerance(100.0));
	assert(!tol3.CheckTolerance(100.1));

	CToleranceMaxT<char> tol4("CToleranceMaxChar", "", 'B');
	tolerances.push_back(&tol4);
	assert(tol4.CheckTolerance('B'));
	assert(!tol4.CheckTolerance('C'));

	cout << "\nTestMinMax\n";
	for (auto itr = tolerances.begin(); itr != tolerances.end(); ++itr)
	{
		auto tol = *itr;
		cout << left << setw(20) << tol->GetName() << ": " <<
			"MinTol=" << boolalpha << setw(5) << tol->IsMinTol() << ", " <<
			"MaxTol=" << boolalpha << setw(5) << tol->IsMaxTol() << ", " <<
			"DevTol=" << boolalpha << setw(5) << tol->IsDevTol() << endl;
	}
}

void TestEnable()
{
	vector<CToleranceBase*> tolerances;

	CToleranceDev tol1("CToleranceDev", "", 5.0, 100.0);
	tolerances.push_back(&tol1);

	CToleranceMin tol2("CToleranceMin", "", 5.0);
	tolerances.push_back(&tol2);

	CToleranceMax tol3("CToleranceMax", "", 100.0);
	tolerances.push_back(&tol3);

	CToleranceMaxT<char> tol4("CToleranceMaxChar", "", 'B');
	tolerances.push_back(&tol4);

	tol1.SetEnabled(true);
	tol3.SetEnabled(true);

	vector<CToleranceBase*> enabled_tolerances;
	copy_if(tolerances.begin(), tolerances.end(), back_inserter(enabled_tolerances), CToleranceBase::enabled_tolerance);
	
	cout << "\nTestEnable\n";
	for (auto itr = enabled_tolerances.begin(); itr != enabled_tolerances.end(); ++itr)
	{
		auto tol = *itr;
		cout << left << setw(20) << tol->GetName() << ": " << "enabled" << endl;
	}
}

void Test2D3D()
{
	vector<CToleranceBase*> tolerances;

	CToleranceDev  tol1("Ball Height", "", 5.0, 100.0);
	tolerances.push_back(&tol1);

	CToleranceMaxT<double> tol2("Warpage", "", 5.0, false, true);
	tolerances.push_back(&tol2);

	CToleranceDev tol3("Ball Pitch", "", 80.0, 100.0);
	tolerances.push_back(&tol3);

	cout << "\nTest2D3D\n";
	for (auto itr = tolerances.begin(); itr != tolerances.end(); ++itr)
	{
		auto tol = *itr;
		auto strName = tol->GetName();
		cout << left << setw(20) << strName << ": " <<
			"3D Only=" << boolalpha << setw(5) << tol->IsTrue3DOnly() << endl;
	}
}

void TestRelativeMode()
{
	vector<CToleranceBase*> tolerances;

	CToleranceDevT_NoNominal<double> tol1("Pad Size", "", 80.0, 100.0);
	tolerances.push_back(&tol1);

	CToleranceMinT_NoNominal<double> tol2("Ball Quality", "", 90.0);
	tolerances.push_back(&tol2);
	
	CToleranceDevT<double> tol3("Ball Pitch", "", 80.0, 100.0);
	tolerances.push_back(&tol3);
	tol3.SetNominal(90.0);

	cout << "\nTestRelativeMode\n";
	for (auto itr = tolerances.begin(); itr != tolerances.end(); ++itr)
	{
		auto tol = *itr;
		auto strName = tol->GetName();
		cout << left << setw(20) << strName << ": " <<
			"Has Relative=" << boolalpha << setw(5) << tol->HasRelativeMode() << endl;
	}
}

void TestFailResult()
{
	CModuleResult moduleResult(g_resultIds);

	CToleranceDev tol1("Pad Size", "", 80.0, 100.0);
	tol1.SetPriority(0);
	moduleResult.AddFailResult(&tol1, "Pad Size: 40 (80.0, 100), Fail");

	CToleranceMin tol2("Ball Quality", "", 90.0);
	tol2.SetPriority(1);
	moduleResult.AddFailResult(&tol2, "Ball Quality: 40 < 90.0, Fail");

	CToleranceDev tol3("Ball Pitch", "", 80.0, 100.0);
	tol3.SetPriority(2);
	moduleResult.AddFailResult(&tol3, "Ball Pitch: 101 (80.0, 100), Fail");
		
	cout << "\nTestFailResult\n";
	INSP_RESULT_ID resultId;
	string resultName, resultDesc;
	tie(resultName, resultId, resultDesc) = moduleResult.GetFirstFailResult();

	cout << left << setw(20) << resultName << ": " << resultId << ", " << resultDesc << endl;

	auto resultIds = moduleResult.GetFailResultIds();
	for (auto itr = resultIds.begin(); itr != resultIds.end(); ++itr)
		cout << left << setw(20) << "Fail Tolerance Id: " << *itr << endl;
}

void TestRejectType()
{
	vector<CToleranceBase*> tolerances;

	CToleranceDev tol1("Pad Size", "", 80.0, 100.0);
	tolerances.push_back(&tol1);
	
	CToleranceMin tol2("Matrix Code", "", 90.0);
	tolerances.push_back(&tol2);
	
	CToleranceDev tol3("PVI Defect1", "", 80.0, 100.0);
	tolerances.push_back(&tol3);
	
	cout << "\nTestRejectType\n";
	
	for (auto itr = tolerances.begin(); itr != tolerances.end(); ++itr)
	{
		auto tol = *itr;
		auto strName = tol->GetName();
		int id = g_resultIds.at(strName);
		const auto& resultFormat = g_resultFormats.at(id);
		cout << left << setw(20) << strName << ": " << resultFormat.m_RejectType << endl;
	}
}

void TestHasPerPin()
{
	vector<CToleranceBase*> tolerances;

	CToleranceDev tol1("Ball Height", "", 80.0, 100.0);
	tolerances.push_back(&tol1);

	CToleranceMin tol2("Matrix Code", "", 90.0, false);
	tolerances.push_back(&tol2);

	CToleranceDev tol3("PVI Defect1", "", 80.0, 100.0, false);
	tolerances.push_back(&tol3);

	cout << "\nTestHasPerPin\n";

	for (auto itr = tolerances.begin(); itr != tolerances.end(); ++itr)
	{
		auto tol = *itr;
		auto strName = tol->GetName();
		cout << left << setw(20) << strName << ": " << boolalpha << tol->HasPerPin() << endl;
	}
}

int main()
{
	TestMinMax();
	TestEnable();
	Test2D3D();
	TestRelativeMode();
	TestFailResult();
	TestRejectType();
	TestHasPerPin();

	return 0;
}