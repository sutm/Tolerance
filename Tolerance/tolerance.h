#pragma once

#include <string>
#include <type_traits>

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

struct CToleranceBase
{
	template<typename U>
	CToleranceBase(U&& name) :
		m_strName(name),
		m_bEnable(false)
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

	virtual bool IsMinTol() const { return false; }
	virtual bool IsMaxTol() const { return false; }

protected:
	std::string m_strName;
	bool m_bEnable;
	std::string m_strResultCode;
};

template <
	typename T = double,
	template<typename V> class TolType = DevTol
>
class CToleranceImpl :	public CToleranceBase, 
						public TolType<T>
{
public:
	typedef T			value_type;
	typedef TolType<T>	tol_type;

	template<typename U>
	CToleranceImpl(U&& name, T dRejectLow, T dRejectHi) :
		CToleranceBase(std::forward<U>(name)),
		TolType<T>(dRejectLow, dRejectHi)
	{}

	template<typename U>
	CToleranceImpl(U&& name, T dReject) :
		CToleranceBase(std::forward<U>(name)),
		TolType<T>(dReject)
	{}

	bool IsMinTol() const override
	{
		return (std::is_same<MinTol<T>, tol_type>::value ||
				std::is_same<DevTol<T>, tol_type>::value);
	}

	bool IsMaxTol() const override
	{
		return (std::is_same<MaxTol<T>, tol_type>::value ||
				std::is_same<DevTol<T>, tol_type>::value);
	}
};

// typedef for commonly used tolerance types
typedef CToleranceImpl<>				CToleranceDev;
typedef CToleranceImpl<double, MinTol>	CToleranceMin;
typedef CToleranceImpl<double, MaxTol>	CToleranceMax;
typedef CToleranceImpl<char, MaxTol>	CToleranceMaxChar;
