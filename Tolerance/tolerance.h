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

namespace ToleranceCategory
{
	const int TolCategory2D = 1;
	const int TolCategory3D = 2;
	const int TolCategory2D3D = TolCategory2D | TolCategory3D;
};

namespace RelativeMode
{
	const int Relative = 1;
	const int NonRelative = 2;
	const int RelativeAny = Relative | NonRelative;
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

	virtual bool IsRelative() const = 0;
	virtual bool IsNonRelative() const = 0;

	virtual bool Is2D() const = 0;
	virtual bool Is3D() const = 0;

	virtual bool IsMinTol() const = 0;
	virtual bool IsMaxTol() const = 0;
	
protected:
	const std::string m_strName;
	std::string m_strDesc;
	bool m_bEnable;
	std::string m_strResultCode;
	int m_nPriority;
};

// template class for tolerance
// parameterized by the following:
// - T: value type of tolerance (double, int, char, string)
// - TolCheck: tolerance checker (min, max)
// - TolCategory: 2D, 3D tolerance
template <
	typename T = double,
	template<typename V> class TolCheck = DevTol,
	int TolCategory = ToleranceCategory::TolCategory2D3D,
	int RelMode = RelativeMode::RelativeAny
>
class CToleranceImpl :	public CToleranceBase, 
						public TolCheck<T>
{
public:
	typedef T			value_type;
	typedef TolCheck<T>	tol_check;

	CToleranceImpl(	std::string name, std::string desc, T dRejectLow, T dRejectHi) :
		CToleranceBase(std::move(name), std::move(desc)),
		TolCheck<T>(dRejectLow, dRejectHi)
	{}

	CToleranceImpl(std::string name, std::string desc, T dReject) :
		CToleranceBase(std::move(name), std::move(desc)),
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

	bool Is2D() const override
	{
		return (TolCategory & ToleranceCategory::TolCategory2D) != 0;
	}

	bool Is3D() const override
	{
		return (TolCategory & ToleranceCategory::TolCategory3D) != 0;
	}

	bool IsRelative() const override
	{
		return (RelMode & RelativeMode::Relative) != 0;
	}

	bool IsNonRelative() const override
	{
		return (RelMode & RelativeMode::NonRelative) != 0;
	}
};

// typedef for commonly used tolerance types
typedef CToleranceImpl<>						CToleranceDev;
typedef CToleranceImpl<double, MinTol>			CToleranceMin;
typedef CToleranceImpl<double, MaxTol>			CToleranceMax;
typedef CToleranceImpl<char, MaxTol>			CToleranceMaxChar;
