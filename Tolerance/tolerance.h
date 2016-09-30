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

namespace ToleranceEnum
{
	enum RelativeMode
	{
		Relative = 1,
		NonRelative,
		RelativeAny,
	};
};

// Abstract base class for tolerance
struct CToleranceBase
{
	CToleranceBase(	std::string name,
					std::string desc,
					ToleranceEnum::RelativeMode relmode) :
		m_strName(std::move(name)),
		m_strDesc(std::move(desc)),
		m_bEnable(false),
		m_nPriority(0),
		m_RelativeMode(relmode)
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

	bool IsRelative() const { return m_RelativeMode==ToleranceEnum::Relative || m_RelativeMode==ToleranceEnum::RelativeAny; }
	bool IsNonRelative() const { return m_RelativeMode==ToleranceEnum::NonRelative || m_RelativeMode==ToleranceEnum::RelativeAny; }

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
	const ToleranceEnum::RelativeMode m_RelativeMode;
};

template<int TolCategory>
struct TolTraits
{
	static const bool is_2D = (TolCategory & ToleranceCategory::TolCategory2D) != 0;
	static const bool is_3D = (TolCategory & ToleranceCategory::TolCategory3D) != 0;
};

// template class for tolerance
// parameterized by the following:
// - T: value type of tolerance (double, int, char, string)
// - TolCheck: tolerance checker (min, max)
// - TolCategory: 2D, 3D tolerance
template <
	typename T = double,
	template<typename V> class TolCheck = DevTol,
	int TolCategory = ToleranceCategory::TolCategory2D3D
>
class CToleranceImpl :	public CToleranceBase, 
						public TolCheck<T>,
						private TolTraits<TolCategory>
{
public:
	typedef T			value_type;
	typedef TolCheck<T>	tol_check;

	CToleranceImpl(	std::string name, std::string desc, T dRejectLow, T dRejectHi,
					ToleranceEnum::RelativeMode relmode=ToleranceEnum::RelativeAny) :
		CToleranceBase(std::move(name), std::move(desc), relmode),
		TolCheck<T>(dRejectLow, dRejectHi)
	{}

	CToleranceImpl(std::string name, std::string desc, T dReject,
					ToleranceEnum::RelativeMode relmode=ToleranceEnum::RelativeAny) :
		CToleranceBase(std::move(name), std::move(desc), relmode),
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
		return is_2D;
	}

	bool Is3D() const override
	{
		return is_3D;
	}
};

// typedef for commonly used tolerance types
typedef CToleranceImpl<>						CToleranceDev;
typedef CToleranceImpl<double, MinTol>			CToleranceMin;
typedef CToleranceImpl<double, MaxTol>			CToleranceMax;
typedef CToleranceImpl<char, MaxTol>			CToleranceMaxChar;
