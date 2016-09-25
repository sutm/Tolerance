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

// Tolerance Categories 
// takes one of the following form:
// - Tol2D: 2D tolerance (default)
// - Tol3D: 3D tolerance
// - Tol2D3D: 2D tolerance, or 3D tolerance if 3D is available
//
struct Tol2D
{ };

struct Tol3D
{ };

struct Tol2D3D : public Tol2D, public Tol3D
{ };

// Tolerance Relative Mode 
// takes one of the following form:
// - Relative: can be relative or non-relative (default)
// - NonRelativeOnly: non-relative only
// - RelativeOnly: relative only
//
struct RelativeOnly
{ };

struct NonRelativeOnly
{ };

struct Relative : public RelativeOnly, public NonRelativeOnly
{ };


// Abstract base class for tolerance
struct CToleranceBase
{
	template<typename U>
	CToleranceBase(U&& name) :
		m_strName(name),
		m_bEnable(false),
		m_nPriority(0)
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

	virtual bool IsMinTol() const = 0;
	virtual bool IsMaxTol() const = 0;
	virtual bool Is2DTol() const = 0;
	virtual bool Is3DTol() const = 0;

protected:
	std::string m_strName;
	bool m_bEnable;
	std::string m_strResultCode;
	int m_nPriority;
};


// template class for tolerance
// parameterized by the following:
// - T: value type of tolerance (double, int, char, string)
// - TolCheck: tolerance checker (min, max)
// - TolCateogry: tolerance category (2D, 3D)
// - RelativeMode: relative mode (relative, non-relative)
template <
	typename T = double,
	template<typename V> class TolCheck = DevTol,
	typename TolCategory = Tol2D
>
class CToleranceImpl :	public CToleranceBase, 
						public TolCheck<T>
{
public:
	typedef T			value_type;
	typedef TolCheck<T>	tol_check;
	typedef TolCategory tol_category;

	template<typename U>
	CToleranceImpl(U&& name, T dRejectLow, T dRejectHi) :
		CToleranceBase(std::forward<U>(name)),
		TolCheck<T>(dRejectLow, dRejectHi)
	{}

	template<typename U>
	CToleranceImpl(U&& name, T dReject) :
		CToleranceBase(std::forward<U>(name)),
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

	bool Is2DTol() const override
	{
		return (std::is_base_of<Tol2D, tol_category>::value);
	}

	bool Is3DTol() const override
	{
		return (std::is_base_of<Tol3D, tol_category>::value);
	}
};

// typedef for commonly used tolerance types
typedef CToleranceImpl<>						CToleranceDev;
typedef CToleranceImpl<double, MinTol>			CToleranceMin;
typedef CToleranceImpl<double, MaxTol>			CToleranceMax;
typedef CToleranceImpl<char, MaxTol>			CToleranceMaxChar;

typedef CToleranceImpl<double, DevTol, Tol3D>	CToleranceDev3D;
typedef CToleranceImpl<double, MaxTol, Tol3D>	CToleranceMax3D;

typedef CToleranceImpl<double, DevTol, Tol2D3D>	CToleranceDev2D3D;
typedef CToleranceImpl<double, MaxTol, Tol2D3D>	CToleranceMax2D3D;