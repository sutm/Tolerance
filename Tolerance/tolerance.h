#pragma once

template<typename T>
struct MinTol
{
	MinTol(T dRejectLo) :
		m_dRejectLo(dRejectLo)
	{}

	// check tolerance and return true if passes
	bool CheckTolerance(T value)
	{
		return !(value < m_dRejectLo);
	}

	T m_dRejectLo;
};

template<typename T>
struct MaxTol 
{
	MaxTol(T dRejectHi) :
		m_dRejectHi(dRejectHi)
	{}

	// check tolerance and return true if passes
	bool CheckTolerance(T value)
	{
		return !(value > m_dRejectHi);
	}

	T m_dRejectHi;
};

template<typename T>
struct MinMaxTol
{
	MinMaxTol(T dRejectLo, T dRejectHi) : 
		m_dRejectLo(dRejectLo),
		m_dRejectHi(dRejectHi)
	{}

	// check tolerance and return true if passes
	bool CheckTolerance(T value)
	{
		return !(value < m_dRejectLo || value > m_dRejectHi);
	}

	T m_dRejectLo;
	T m_dRejectHi;
};

template <
	typename T = double,
	template<typename V> class TolType = MinMaxTol
>
class CToleranceBase : public TolType<T>
{
	template<typename... U>
	CToleranceBase(U... limits) :
		TolType(limits...)
	{}
};

using CTolerance	= CToleranceBase<>;
using CToleranceMin	= CToleranceBase<double, MinTol>;
using CToleranceMax	= CToleranceBase<double, MaxTol>;

using CToleranceCharMax = CToleranceBase<char, MaxTol>;