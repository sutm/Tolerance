// Tolerance.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <assert.h>
#include <type_traits>
#include <algorithm>
#include <iterator>
#include <vector>
#include <iostream>
#include <iomanip>
#include "tolerance.h"

using namespace std;

void TestMinMax()
{
	vector<CToleranceBase*> tolerances;

	CToleranceDev tol1("CToleranceDev", 5.0, 100.0);
	tolerances.push_back(&tol1);
	assert(tol1.CheckTolerance(5.0));
	assert(tol1.CheckTolerance(100.0));
	assert(!tol1.CheckTolerance(4.9));
	assert(!tol1.CheckTolerance(100.1));

	CToleranceMin tol2("CToleranceMin", 5.0);
	tolerances.push_back(&tol2);
	assert(tol2.CheckTolerance(5.0));
	assert(!tol2.CheckTolerance(4.9));

	CToleranceMax tol3("CToleranceMax", 100.0);
	tolerances.push_back(&tol3);
	assert(tol3.CheckTolerance(100.0));
	assert(!tol3.CheckTolerance(100.1));

	CToleranceMaxChar tol4("CToleranceMaxChar", 'B');
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

void TestEnablePriority()
{
	vector<CToleranceBase*> tolerances;

	CToleranceDev tol1("CToleranceDev", 5.0, 100.0);
	tolerances.push_back(&tol1);

	CToleranceMin tol2("CToleranceMin", 5.0);
	tolerances.push_back(&tol2);

	CToleranceMax tol3("CToleranceMax", 100.0);
	tolerances.push_back(&tol3);

	CToleranceMaxChar tol4("CToleranceMaxChar", 'B');
	tolerances.push_back(&tol4);

	for (auto itr = tolerances.begin(); itr != tolerances.end(); ++itr)
	{
		auto tol = *itr;
		cout << left << setw(20) << tol->GetName() << ": " <<
			"MinTol=" << boolalpha << setw(5) << tol->IsMinTol() << ", " <<
			"MaxTol=" << boolalpha << setw(5) << tol->IsMaxTol() << endl;
	}

	tol1.SetEnabled(true);
	tol3.SetEnabled(true);
	tol1.SetPriority(1);
	tol3.SetPriority(0);

	vector<CToleranceBase*> enabled_tolerances;
	copy_if(tolerances.begin(), tolerances.end(), back_inserter(enabled_tolerances), CToleranceBase::enabled_tolerance);
	sort(enabled_tolerances.begin(), enabled_tolerances.end(), CToleranceBase::tolerance_by_priority);

	cout << "TestEnablePriority" << endl;
	for (auto itr = enabled_tolerances.begin(); itr != enabled_tolerances.end(); ++itr)
	{
		auto tol = *itr;
		cout << left << setw(20) << tol->GetName() << ": " << "enabled" << endl;
	}
}

void Test2D3DCategory()
{
	vector<CToleranceBase*> tolerances;

	CToleranceDev tol1("Ball Height", 5.0, 100.0, ToleranceEnum::TolCategory2D3D);
	tolerances.push_back(&tol1);

	CToleranceMax tol2("Warpage", 5.0, ToleranceEnum::TolCategory3D);
	tolerances.push_back(&tol2);

	CToleranceDev tol3("Ball Pitch", 80.0, 100.0, ToleranceEnum::TolCategory2D);
	tolerances.push_back(&tol3);

	cout << "Test2D3DCategory" << endl;
	for (auto itr = tolerances.begin(); itr != tolerances.end(); ++itr)
	{
		auto tol = *itr;
		cout << left << setw(20) << tol->GetName() << ": " <<
			"2D=" << boolalpha << setw(5) << tol->Is2D() << ", " <<
			"3D=" << boolalpha << setw(5) << tol->Is3D() << endl;
	}
}

void TestRelativeMode()
{
	vector<CToleranceBase*> tolerances;

	CToleranceDev tol1("Pad Size", 80.0, 100.0, ToleranceEnum::TolCategory2D, ToleranceEnum::Relative);
	tolerances.push_back(&tol1);

	CToleranceMin tol2("Ball Quality", 90.0, ToleranceEnum::TolCategory2D, ToleranceEnum::NonRelative);
	tolerances.push_back(&tol2);

	CToleranceDev tol3("Ball Pitch", 80.0, 100.0, ToleranceEnum::TolCategory2D, ToleranceEnum::RelativeAny);
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

int main()
{
	TestMinMax();
	TestEnablePriority();
	Test2D3DCategory();
	TestRelativeMode();
	return 0;
}