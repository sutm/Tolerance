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

void MinMaxTol()
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

	cout << "MinMaxTol test" << endl;
	for (auto itr = tolerances.begin(); itr != tolerances.end(); ++itr)
	{
		auto tol = *itr;
		cout << left << setw(20) << tol->GetName() << ": " <<
			"MinTol=" << boolalpha << setw(5) << tol->IsMinTol() << ", " <<
			"MaxTol=" << boolalpha << setw(5) << tol->IsMaxTol() << endl;
	}
}

void EnablingTolPriority()
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

	cout << "EnablingTolPriority test" << endl;
	for (auto itr = enabled_tolerances.begin(); itr != enabled_tolerances.end(); ++itr)
	{
		auto tol = *itr;
		cout << left << setw(20) << tol->GetName() << ": " << "enabled" << endl;
	}
}

void Tol2D3DCategory()
{
	vector<CToleranceBase*> tolerances;

	CToleranceDev2D3D tol1("Ball Height", 5.0, 100.0);
	tolerances.push_back(&tol1);

	CToleranceMax3D tol2("Warpage", 5.0);
	tolerances.push_back(&tol2);

	CToleranceDev tol3("Ball Pitch", 80.0, 100.0);
	tolerances.push_back(&tol3);

	cout << "Tol2D3DCategory test" << endl;
	for (auto itr = tolerances.begin(); itr != tolerances.end(); ++itr)
	{
		auto tol = *itr;
		cout << left << setw(20) << tol->GetName() << ": " <<
			"2D=" << boolalpha << setw(5) << tol->Is2DTol() << ", " <<
			"3D=" << boolalpha << setw(5) << tol->Is3DTol() << endl;
	}
}

int main()
{
	MinMaxTol();
	EnablingTolPriority();
	Tol2D3DCategory();
	return 0;
}