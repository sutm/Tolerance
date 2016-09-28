#pragma once

#include <string>
#include <array>
#include <vector>
#include <functional>
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
		if (std::find(m_FailTolerances.begin(), m_FailTolerances.end(), pTol) != m_FailTolerances.end()) // already exists
			return;

		m_FailTolerances.emplace_back(pTol);
		m_ResultDescs.emplace(make_pair(pTol->GetName(), strResultDesc));
	}

	int GetFirstFailResultId()
	{
		if (m_FailTolerances.empty())
			return INSP_PASS;

		std::nth_element(m_FailTolerances.begin(), m_FailTolerances.begin(), m_FailTolerances.end(), CToleranceBase::tolerance_by_priority);
		int nResultId = GetResultIdByTolName(m_FailTolerances.front());

		return nResultId;
	}

	std::vector<int> GetFailResultIds()
	{
		std::vector<int> vResultIds;
		if (m_FailTolerances.empty())
			return vResultIds;

		using namespace std::placeholders;
		std::sort(m_FailTolerances.begin(), m_FailTolerances.end(), CToleranceBase::tolerance_by_priority);
		std::transform(m_FailTolerances.begin(), m_FailTolerances.end(), std::back_inserter(vResultIds), std::bind(&CModuleResult<RESULT_COUNT>::GetResultIdByTolName, this, _1));
		
		return vResultIds;
	}

private:
	int GetResultIdByTolName(const CToleranceBase* pTol) const
	{
		auto strName = pTol->GetName();
		auto itr = std::find_if(m_ResultIds.begin(), m_ResultIds.end(), [strName](const Result& result)
		{
			return result.m_strTolName == strName;
		});
		return itr->m_nResultId;
	}

	const std::array<Result, RESULT_COUNT>& m_ResultIds;
	std::vector<CToleranceBase*> m_FailTolerances;
	//typedef decltype(CToleranceBase::GetName()) tol_name;
	std::map<std::string, std::string> m_ResultDescs;
};