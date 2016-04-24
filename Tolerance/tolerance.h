#pragma once

#include <string>
#include <type_traits>

template<typename T>
struct MinTol
{
	MinTol(T dRejectLo) :
		m_dRejectLo(dRejectLo)
	{}

	// check tolerance and return true if passes
	bool CheckTolerance(T value) const
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
	bool CheckTolerance(T value) const
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
	bool CheckTolerance(T value) const
	{
		return !(value < m_dRejectLo || value > m_dRejectHi);
	}

	T m_dRejectLo;
	T m_dRejectHi;
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
	template<typename V> class TolType = MinMaxTol
>
class CToleranceImpl :	public CToleranceBase, 
						public TolType<T>
{
public:
	template<typename... U>
	CToleranceImpl(std::string&& name, U&&... limits) :
		m_strName(name),
		m_bEnable(false),
		TolType<T>(limits...)
	{}

	bool IsMinTol() const override
	{
		return (std::is_base_of<MinTol<T>, CToleranceImpl>::value ||
			std::is_base_of<MinMaxTol<T>, CToleranceImpl>::value);
	}

	bool IsMaxTol() const override
	{
		return (std::is_base_of<MaxTol<T>, CToleranceImpl>::value ||
			std::is_base_of<MinMaxTol<T>, CToleranceImpl>::value);
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

using CTolerance		= CToleranceImpl<>;
using CToleranceMin		= CToleranceImpl<double, MinTol>;
using CToleranceMax		= CToleranceImpl<double, MaxTol>;
using CToleranceCharMax = CToleranceImpl<char, MaxTol>;