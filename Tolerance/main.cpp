// Tolerance.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <assert.h>
#include <type_traits>
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

enum INSP_RESULT_ID
{
	INSP_PASS=0,
	INSP_FAIL_BALL_HEIGHT,
	INSP_FAIL_BALL_COPLAN,
	INSP_FAIL_BALL_PITCH,
	INSP_FAIL_BALL_QUALITY,
	INSP_FAIL_WARPAGE,
	INSP_FAIL_PAD_SIZE
};

const int INSP_TOL_COUNT = 6;
static const array<Result, INSP_TOL_COUNT> g_results = 
{{
	{"Ball Height",		INSP_FAIL_BALL_HEIGHT	},
	{"Coplan",			INSP_FAIL_BALL_COPLAN	},
	{"Ball Pitch",		INSP_FAIL_BALL_PITCH	},
	{"Ball Quality",	INSP_FAIL_BALL_QUALITY	},
	{"Warpage",			INSP_FAIL_WARPAGE		},
	{"Pad Size",		INSP_FAIL_PAD_SIZE		}
}};

static const array<ToleranceProperties, INSP_TOL_COUNT> g_tolproperties = 
{{
	{"Ball Height",		ToleranceProperties::Tol2D3D,	ToleranceProperties::RelativeAny	},
	{"Coplan",			ToleranceProperties::Tol3D,		ToleranceProperties::RelativeNot	},
	{"Ball Pitch",		ToleranceProperties::Tol2D,		ToleranceProperties::RelativeAny	},
	{"Ball Quality",	ToleranceProperties::Tol2D,		ToleranceProperties::RelativeNot	},
	{"Warpage",			ToleranceProperties::Tol3D,		ToleranceProperties::RelativeNot	},
	{"Pad Size",		ToleranceProperties::Tol2D,		ToleranceProperties::Relative		}
}};

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

	cout << "TestMinMax" << endl;
	for (auto itr = tolerances.begin(); itr != tolerances.end(); ++itr)
	{
		auto tol = *itr;
		cout << left << setw(20) << tol->GetName() << ": " <<
			"MinTol=" << boolalpha << setw(5) << tol->IsMinTol() << ", " <<
			"MaxTol=" << boolalpha << setw(5) << tol->IsMaxTol() << endl;
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
	
	cout << "TestEnable" << endl;
	for (auto itr = enabled_tolerances.begin(); itr != enabled_tolerances.end(); ++itr)
	{
		auto tol = *itr;
		cout << left << setw(20) << tol->GetName() << ": " << "enabled" << endl;
	}
}

void Test2D3DCategory()
{
	vector<CToleranceBase*> tolerances;

	CToleranceDevT<double>  tol1("Ball Height", "", 5.0, 100.0);
	tolerances.push_back(&tol1);

	CToleranceMaxT<double> tol2("Warpage", "", 5.0);
	tolerances.push_back(&tol2);

	CToleranceDevT<double> tol3("Ball Pitch", "", 80.0, 100.0);
	tolerances.push_back(&tol3);

	cout << "Test2D3DCategory" << endl;
	for (auto itr = tolerances.begin(); itr != tolerances.end(); ++itr)
	{
		auto tol = *itr;
		auto strName = tol->GetName();
		auto tolprop = find_if(g_tolproperties.begin(), g_tolproperties.end(), [strName](const ToleranceProperties& prop)
		{
			return prop.m_strTolName == strName; 
		});
		
		cout << left << setw(20) << tolprop->m_strTolName << ": " <<
			"2D=" << boolalpha << setw(5) << tolprop->m_category << ", " <<
			"3D=" << boolalpha << setw(5) << tolprop->m_category << endl;
	}
}

void TestRelativeMode()
{
	vector<CToleranceBase*> tolerances;

	CToleranceDevT<double> tol1("Pad Size", "", 80.0, 100.0);
	tolerances.push_back(&tol1);

	CToleranceMinT<double> tol2("Ball Quality", "", 90.0);
	tolerances.push_back(&tol2);

	CToleranceDevT<double> tol3("Ball Pitch", "", 80.0, 100.0);
	tolerances.push_back(&tol3);

	cout << "TestRelativeMode" << endl;
	for (auto itr = tolerances.begin(); itr != tolerances.end(); ++itr)
	{
		auto tol = *itr;
		cout << left << setw(20) << tol->GetName() << ": " <<
			"Relative=" << boolalpha << setw(5) << tol->IsRelative() << ", " <<
			"NonRelative=" << boolalpha << setw(5) << tol->IsNonRelative() << endl;
	}
}

void TestFailResult()
{
	CModuleResult<INSP_TOL_COUNT> moduleResult(g_results);

	CToleranceDev tol1("Pad Size", "", 80.0, 100.0);
	tol1.SetPriority(0);
	moduleResult.AddFailResult(&tol1, "Pad Size: 40 (80.0, 100), Fail");

	CToleranceMin tol2("Ball Quality", "", 90.0);
	tol2.SetPriority(1);
	moduleResult.AddFailResult(&tol2, "Ball Quality: 40 < 90.0, Fail");

	CToleranceDev tol3("Ball Pitch", "", 80.0, 100.0);
	tol3.SetPriority(2);
	moduleResult.AddFailResult(&tol3, "Ball Pitch: 101 (80.0, 100), Fail");
		
	cout << "TestFailResult" << endl;
	Result result;
	string strResultDesc;
	tie(result, strResultDesc) = moduleResult.GetFirstFailResult();
	cout << left << setw(20) << result.m_strTolName << ": " << result.m_nResultId << ", " << strResultDesc << endl;

	auto resultIds = moduleResult.GetFailResultIds();
	for (auto itr = resultIds.begin(); itr != resultIds.end(); ++itr)
		cout << left << setw(20) << "Fail Tolerance Id: " << *itr << endl;
}

int main()
{
	TestMinMax();
	TestEnable();
	Test2D3DCategory();
	TestRelativeMode();
	TestFailResult();
	return 0;
}