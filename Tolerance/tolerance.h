#pragma once

#include <string>
#include <type_traits>

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
};

namespace ToleranceEnum
{
	enum ToleranceCategory
	{
		TolCategory2D = 0,
		TolCategory3D,
		TolCategory2D3D
	};

	enum RelativeMode
	{
		RelativeAny = 0,
		Relative,
		NonRelative
	};
};

// Abstract base class for tolerance
struct CToleranceBase
{
	template<typename U>
	CToleranceBase(	U&& name,  
					ToleranceEnum::ToleranceCategory tolCategory,
					ToleranceEnum::RelativeMode relmode) :
		m_strName(name),
		m_bEnable(false),
		m_nPriority(0),
		m_RelativeMode(relmode),
		m_TolCategory(tolCategory)
	{ }

	std::string GetName() const
	{
		return m_strName;
	}
	
	bool IsEnabled() const
	{
		return m_bEnable;
	}

	void SetEnabled(bool bEnable)
	{
		m_bEnable = bEnable;
	}

	std::string GetResultCode() const
	{
		return m_strResultCode;
	}

	void SetResultCode(const std::string& resultCode)
	{
		m_strResultCode = resultCode;
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

	bool IsRelative() const { return m_RelativeMode==ToleranceEnum::Relative || m_RelativeMode==ToleranceEnum::RelativeAny; }
	bool IsNonRelative() const { return m_RelativeMode==ToleranceEnum::NonRelative || m_RelativeMode==ToleranceEnum::RelativeAny; }

	bool Is2D() const { return m_TolCategory==ToleranceEnum::TolCategory2D || m_TolCategory==ToleranceEnum::TolCategory2D3D; }
	bool Is3D() const { return m_TolCategory==ToleranceEnum::TolCategory3D || m_TolCategory==ToleranceEnum::TolCategory2D3D; }

	virtual bool IsMinTol() const = 0;
	virtual bool IsMaxTol() const = 0;
	
protected:
	std::string m_strName;
	bool m_bEnable;
	std::string m_strResultCode;
	int m_nPriority;
	const ToleranceEnum::ToleranceCategory m_TolCategory;
	const ToleranceEnum::RelativeMode m_RelativeMode;
};


// template class for tolerance
// parameterized by the following:
// - T: value type of tolerance (double, int, char, string)
// - TolCheck: tolerance checker (min, max)
template <
	typename T = double,
	template<typename V> class TolCheck = DevTol
>
class CToleranceImpl :	public CToleranceBase, 
						public TolCheck<T>
{
public:
	typedef T			value_type;
	typedef TolCheck<T>	tol_check;

	template<typename U>
	CToleranceImpl(	U&& name, T dRejectLow, T dRejectHi,
					ToleranceEnum::ToleranceCategory tolCategory=ToleranceEnum::TolCategory2D,
					ToleranceEnum::RelativeMode relmode=ToleranceEnum::RelativeAny) :
		CToleranceBase(std::forward<U>(name), tolCategory, relmode),
		TolCheck<T>(dRejectLow, dRejectHi)
	{}

	template<typename U>
	CToleranceImpl(	U&& name, T dReject, 
					ToleranceEnum::ToleranceCategory tolCategory=ToleranceEnum::TolCategory2D,
					ToleranceEnum::RelativeMode relmode=ToleranceEnum::RelativeAny) :
		CToleranceBase(std::forward<U>(name), tolCategory, relmode),
		TolCheck<T>(dReject)
	{}

	bool IsMinTol() const override
	{
		return (std::is_same<MinTol<T>, tol_check>::value ||
				std::is_same<DevTol<T>, tol_check>::value);
	}

	bool IsMaxTol() const override
	{
		return (std::is_same<MaxTol<T>, tol_check>::value ||
				std::is_same<DevTol<T>, tol_check>::value);
	}
};

// typedef for commonly used tolerance types
typedef CToleranceImpl<>						CToleranceDev;
typedef CToleranceImpl<double, MinTol>			CToleranceMin;
typedef CToleranceImpl<double, MaxTol>			CToleranceMax;
typedef CToleranceImpl<char, MaxTol>			CToleranceMaxChar;
