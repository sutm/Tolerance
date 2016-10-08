#pragma once

#include <string>
#include <array>
#include <vector>
#include <functional>
#include <map>

struct CToleranceBase;

struct ResultFormat
{
	enum ERejectType
	{
		RT_Measure = 1,		// Measurement results	eg. ball pitch, ball height
		RT_Text,			// Text results			eg. Datamatrix, OCR
		RT_Error,			// Error results		eg. Missing ball, Location Error
		RT_PVI				// PVI-like results which contain width, length, area	eg. PVI, Postseal defects
	};
	ERejectType m_RejectType;
};

class CModuleResult
{
public:
	CModuleResult(const std::map<std::string, INSP_RESULT_ID>& resultIds) :
		m_ResultIds(resultIds)
	{ }

	void AddFailResult(CToleranceBase* pTol, std::string strResultDesc)
	{
		if (std::find(m_FailTolerances.begin(), m_FailTolerances.end(), pTol) != m_FailTolerances.end()) // already exists
			return;

		m_FailTolerances.emplace_back(pTol);
		m_ResultDescs.emplace(make_pair(pTol->GetName(), strResultDesc));
	}

	// returns Result and Description of the first failed tolerance
	std::tuple<std::string, INSP_RESULT_ID, std::string> GetFirstFailResult()
	{
		if (m_FailTolerances.empty())
			return std::make_tuple("", INSP_PASS, "");

		std::nth_element(m_FailTolerances.begin(), m_FailTolerances.begin(), m_FailTolerances.end(), CToleranceBase::tolerance_by_priority);
		auto strName = m_FailTolerances.front()->GetName();
		return std::make_tuple(strName, GetResultIdByTolName(strName), m_ResultDescs[strName]);
	}

	std::vector<int> GetFailResultIds()
	{
		std::vector<int> vResultIds;
		if (m_FailTolerances.empty())
			return vResultIds;

		using namespace std::placeholders;
		std::sort(m_FailTolerances.begin(), m_FailTolerances.end(), CToleranceBase::tolerance_by_priority);
		std::transform(m_FailTolerances.begin(), m_FailTolerances.end(), std::back_inserter(vResultIds), [&](const CToleranceBase* pTol)
		{
			return GetResultIdByTolName(pTol->GetName());
		});
		
		return vResultIds;
	}

private:
	INSP_RESULT_ID GetResultIdByTolName(const std::string& strName) const
	{
		return m_ResultIds.at(strName);
	}

	const std::map<std::string, INSP_RESULT_ID>& m_ResultIds;
	std::vector<CToleranceBase*> m_FailTolerances;
	//typedef decltype(CToleranceBase::GetName()) tol_name;
	std::map<std::string, std::string> m_ResultDescs;
};