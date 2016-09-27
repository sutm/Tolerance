#pragma once

#include <string>
#include <array>
#include <vector>
#include <map>

struct Result
{
	std::string m_strTolName;
	int m_nResultId;
};

struct CToleranceBase;

template<int RESULT_COUNT>
class CModuleResult
{
public:
	CModuleResult(const std::array<Result, RESULT_COUNT>& resultIds) :
		m_ResultIds(resultIds)
	{ }

	void AddFailResult(CToleranceBase* pTol, std::string strResultDesc)
	{
		if (std::find(m_Tolerances.begin(), m_Tolerances.end(), pTol) != m_Tolerances.end()) // already exists
			return;

		m_Tolerances.emplace_back(pTol);
		m_ResultDescs.emplace(make_pair(pTol->GetName(), strResultDesc));
	}

	int GetFirstFailResultId() const
	{
		if (m_Tolerances.empty())
			return INSP_PASS;

		auto pTol = std::nth_element(m_Tolerances.begin(), m_Tolerances.end(), CToleranceBase::tolerance_by_priority);
		auto itr = std::find_if(m_ResultIds.begin(), m_ResultIds.end(), [(*pTol)->GetName()](const Result& result)
		{
			return result.m_strTolName == strName;
		});

		return itr->m_nResultId;
	}

	std::vector<int> GetFailResults() const;

private:
	const std::array<Result, RESULT_COUNT>& m_ResultIds;
	std::vector<CToleranceBase*> m_Tolerances;
	//typedef decltype(CToleranceBase::GetName()) tol_name;
	std::map<std::string, std::string> m_ResultDescs;
};