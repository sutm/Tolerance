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
	virtual std::string GetName() const = 0;
	
	virtual bool IsEnabled() const = 0;
	virtual void SetEnabled(bool bEnable) = 0;

	virtual bool IsMinTol() const = 0;
	virtual bool IsMaxTol() const = 0;

	virtual std::string GetResultCode() const = 0;
	virtual void SetResultCode(const std::string& resultCode) = 0;
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
		m_strName(name),
		m_bEnable(false),
		TolType<T>(dRejectLow, dRejectHi)
	{}

	template<typename U>
	CToleranceImpl(U&& name, T dReject) :
		m_strName(name),
		m_bEnable(false),
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

	std::string GetName() const override
	{
		return m_strName;
	}

	bool IsEnabled() const override
	{
		return m_bEnable;
	}

	void SetEnabled(bool bEnable) override
	{
		m_bEnable = bEnable;
	}
	
	std::string GetResultCode() const override
	{
		return m_strResultCode;
	}

	void SetResultCode(const std::string& resultCode) override
	{
		m_strResultCode = resultCode;
	}

private:
	std::string m_strName;
	bool m_bEnable;
	std::string m_strResultCode;
};

// typedef for commonly used tolerance types
typedef CToleranceImpl<>				CToleranceDev;
typedef CToleranceImpl<double, MinTol>	CToleranceMin;
typedef CToleranceImpl<double, MaxTol>	CToleranceMax;
typedef CToleranceImpl<char, MaxTol>	CToleranceMaxChar;
