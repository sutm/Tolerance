#pragma once

#include <string>
#include <map>
#include <type_traits>
#include "defines.h"

// Tolerance Checkers can be one of the following:
// - DevTol: check both min and max limits (default)
// - MinTol: check min limit only
// - MaxTol: check max limit only
//
template<typename T>
struct DevTol
{
public:
	DevTol(T dRejectLo, T dRejectHi) : 
		m_dRejectLo(dRejectLo),
		m_dRejectHi(dRejectHi)
	{}

	// check tolerance and return true if passes
	bool CheckTolerance(T value) const
	{
		return !(value < m_dRejectLo || value > m_dRejectHi);
	}

	static const bool min_value = true;
	static const bool max_value = true;

protected:
	T m_dRejectLo;
	T m_dRejectHi;
};

template<typename T>
struct MinTol : public DevTol<T>
{
public:
	MinTol(T dRejectLo) : DevTol(dRejectLo, T())
	{}

	// check tolerance and return true if passes
	bool CheckTolerance(T value) const
	{
		return !(value < m_dRejectLo);
	}

	static const bool min_value = true;
	static const bool max_value = false;

};

template<typename T>
struct MaxTol : public DevTol<T>
{
public:
	MaxTol(T dRejectHi) : DevTol(T(), dRejectHi)
	{}

	// check tolerance and return true if passes
	bool CheckTolerance(T value) const
	{
		return !(value > m_dRejectHi);
	}

	static const bool min_value = false;
	static const bool max_value = true;

};

// Abstract base class for tolerance
struct CToleranceBase
{
	CToleranceBase(	std::string name,
					std::string desc) :
		m_strName(std::move(name)),
		m_strDesc(std::move(desc)),
		m_bEnable(false),
		m_nPriority(0)
	{ }

	std::string GetName() const
	{
		return m_strName;
	}
	
	std::string GetDesc() const
	{
		return m_strDesc;
	}
	
	void SetDesc(std::string desc)
	{
		m_strDesc = std::move(desc);
	}

	bool IsEnabled() const
	{
		return m_bEnable;
	}

	void SetEnabled(bool bEnable)
	{
		m_bEnable = bEnable;
	}

	void SetPriority(int nPriority)
	{
		m_nPriority = nPriority;
	}

	int GetPriority() const
	{
		return m_nPriority;
	}

	static bool enabled_tolerance(const CToleranceBase* pTol)
	{
		return pTol->m_bEnable;
	}

	static bool tolerance_by_priority(const CToleranceBase* pTol1, const CToleranceBase* pTol2)
	{
		return pTol1->m_nPriority < pTol2->m_nPriority;
	}

	bool IsDevTol() const { return IsMinTol() && IsMaxTol(); }
	virtual bool IsMinTol() const = 0;
	virtual bool IsMaxTol() const = 0;

protected:
	const std::string m_strName;
	std::string m_strDesc;
	bool m_bEnable;
	int m_nPriority;
};

// template class for tolerance
// parameterized by the following:
// - T: value type of tolerance (double, int, char, string)
// - TolCheck: tolerance checker (min, max)
template <
	typename T = double,
	template <typename U> class TolCheck = DevTol
>
class CToleranceImpl : public CToleranceBase, public TolCheck<T>
{
public:

	CToleranceImpl(	std::string name, std::string desc, T reject) :
		CToleranceBase(std::move(name), std::move(desc)),
		TolCheck<T>(reject)
	{}
		
	CToleranceImpl(	std::string name, std::string desc, T rejectLo, T rejectHi) :
		CToleranceBase(std::move(name), std::move(desc)),
		TolCheck<T>(rejectLo, rejectHi)
	{}

	bool IsMinTol() const override
	{
		return TolCheck<T>::min_value;
	};
		
	bool IsMaxTol() const override
	{
		return TolCheck<T>::max_value;
	};
};

#if _MSC_VER < 1700
template <
	typename T = double
>
class CToleranceMinT :	public CToleranceImpl<T, MinTol>
{
public:
	CToleranceMinT(	std::string name, std::string desc, T rejectLo) :
		CToleranceImpl(std::move(name), std::move(desc), rejectLo)
	{}
};

template <
	typename T = double
>
class CToleranceMaxT :	public CToleranceImpl<T, MaxTol>
{
public:
	CToleranceMaxT(	std::string name, std::string desc, T rejectHi) :
	  CToleranceImpl(std::move(name), std::move(desc), rejectHi)
	  {}
};

template <
	typename T = double
>
class CToleranceDevT :	public CToleranceImpl<T, DevTol>
{
public:
	CToleranceDevT(	std::string name, std::string desc, T rejectLo, T rejectHi) :
	  CToleranceImpl(std::move(name), std::move(desc), rejectLo, rejectHi)
	  {}
};

#else
using CToleranceMinT = CToleranceImpl<T, MinTol>;
using CToleranceMaxT = CToleranceImpl<T, MaxTol>;
using CToleranceDevT = CToleranceImpl<T, DevTol>;
#endif

// typedef for commonly used tolerance types
typedef CToleranceDevT<>			CToleranceDev;
typedef CToleranceMinT<double>		CToleranceMin;
typedef CToleranceMaxT<double>		CToleranceMax;

struct ToleranceProperties
{
	enum ETolCategory
	{
		Tol2D = 1,
		Tol3D,
		Tol2D3D
	};
	ETolCategory m_TolCategory;

	enum ERelativeMode
	{
		RelativeOnly = 1,
		RelativeNA,
		RelativeAny
	};
	ERelativeMode m_RelativeMode;

	bool m_bHasPerPin;

	static bool Is2D(const ToleranceProperties& p)
	{
		return (p.m_TolCategory & Tol2D) != 0;
	}

	static bool Is3D(const ToleranceProperties& p)
	{
		return (p.m_TolCategory & Tol3D) != 0;
	}

	static bool IsRelative(const ToleranceProperties& p)
	{
		return (p.m_RelativeMode & RelativeOnly) != 0;
	}

	static bool IsNonRelative(const ToleranceProperties& p)
	{
		return (p.m_RelativeMode & RelativeNA) != 0;
	}
};
