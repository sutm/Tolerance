#pragma once

#include <string>
#include <map>
#include <type_traits>
#include "toltraits.h"
#include "defines.h"

// Abstract base class for tolerance
struct CToleranceBase
{
	CToleranceBase(	std::string name, std::string desc) :
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

	virtual bool IsDevTol() const = 0;
	virtual bool IsMinTol() const = 0;
	virtual bool IsMaxTol() const = 0;

	virtual bool HasPerPin() const = 0;
	virtual bool Is3DOnly() const = 0;
	
	virtual bool HasRelativeMode() const = 0;

private:
	const std::string m_strName;
	
protected:
	std::string m_strDesc;
	bool m_bEnable;
	int m_nPriority;
};

// Tolerance Checkers can be one of the following:
// - DevTol: check both min and max limits (default)
// - MinTol: check min limit only
// - MaxTol: check max limit only
//
template<typename T>
struct MinMaxTol
{
public:
	static const bool MinLimit = true;
	static const bool MaxLimit = true;
	static const bool SingleLimit = !(MinLimit && MaxLimit);

	MinMaxTol(T dRejectLo, T dRejectHi) : 
		m_dRejectLo(dRejectLo),
		m_dRejectHi(dRejectHi)
	{}

	// check tolerance and return true if passes
	bool CheckTolerance(T value) const
	{
		return !(value < m_dRejectLo || value > m_dRejectHi);
	}

	void SetRejectLCL(T value) { m_dRejectLo = value; }
	void SetRejectUCL(T value) { m_dRejectHi = value; }
	T GetRejectLCL() const { return m_dRejectLo; }
	T GetRejectUCL() const { return m_dRejectHi; }

protected:
	T m_dRejectLo;
	T m_dRejectHi;
};

template<typename T>
struct MinTol
{
public:
	static const bool MinLimit = true;
	static const bool MaxLimit = false;
	static const bool SingleLimit = !(MinLimit && MaxLimit);

	MinTol(T dRejectLo) : m_dRejectLo(dRejectLo)
	{}

	// check tolerance and return true if passes
	bool CheckTolerance(T value) const
	{
		return !(value < m_dRejectLo);
	}

	void SetRejectLCL(T value) { m_dRejectLo = value; }
	T GetRejectLCL() const { return m_dRejectLo; }

private:
	T m_dRejectLo;
};

template<typename T>
struct MaxTol
{
public:
	static const bool MinLimit = false;
	static const bool MaxLimit = true;
	static const bool SingleLimit = !(MinLimit && MaxLimit);

	MaxTol(T dRejectHi) : m_dRejectHi(dRejectHi)
	{}

	// check tolerance and return true if passes
	bool CheckTolerance(T value) const
	{
		return !(value > m_dRejectHi);
	}

	void SetRejectUCL(T value) { m_dRejectHi = value; }
	T GetRejectUCL() const { return m_dRejectHi; }

private:
	T m_dRejectHi;
};

// template class for tolerance
template <
	typename T,
	template <typename U> class TolCheck = MinMaxTol,			// DevTol, MinTol, MaxTol
	typename Traits = TolPerPinTraits
>
class CToleranceImplBaseT :	public CToleranceBase, 
						public TolCheck<T>
{
public:
	template <typename U>
	CToleranceImplBaseT(std::string name, std::string desc, U rejectLo, U rejectHi,
		typename std::enable_if<!TolCheck<U>::SingleLimit>::type* = 0) :
		CToleranceBase(std::move(name), std::move(desc)),
		TolCheck<T>(rejectLo, rejectHi)
	{}

	template <typename U>
	CToleranceImplBaseT(std::string name, std::string desc, U reject, 
		typename std::enable_if<TolCheck<U>::SingleLimit>::type* = 0) :
		CToleranceBase(std::move(name), std::move(desc)),
		TolCheck<T>(reject)
	{}
	
	bool IsMinTol() const override
	{
		return TolCheck<T>::MinLimit;
	}
		
	bool IsMaxTol() const override
	{
		return TolCheck<T>::MaxLimit;
	}

	bool IsDevTol() const override
	{
		return !TolCheck<T>::SingleLimit;
	}

	bool Is3DOnly() const override
	{
		return Traits::Is3DOnly();
	}

	bool HasPerPin() const override
	{
		return Traits::HasPerPin();
	}
};

template <
	typename Derived,
	typename T,
	template <typename U> class TolCheck = MinMaxTol,			// DevTol, MinTol, MaxTol
	typename Traits = TolPerPinTraits
>
class CToleranceImplT :	public CToleranceImplBaseT<T, TolCheck, Traits>
{
public:
	template <typename U>
	CToleranceImplT(std::string name, std::string desc, U rejectLo, U rejectHi,
		typename std::enable_if<!TolCheck<U>::SingleLimit>::type* = 0) :
	CToleranceImplBaseT(std::move(name), std::move(desc), rejectLo, rejectHi)
	{}

	template <typename U>
	CToleranceImplT(std::string name, std::string desc, U reject, 
		typename std::enable_if<TolCheck<U>::SingleLimit>::type* = 0) :
	CToleranceImplBaseT(std::move(name), std::move(desc), reject)
	{}

	virtual bool HasRelativeMode() const { return false; }
};

template <
	typename Derived,
	typename T,
	template <typename U> class TolCheck = MinMaxTol,			// DevTol, MinTol, MaxTol
	typename Traits = TolPerPinTraits
>
class CToleranceNomT :	public CToleranceImplBaseT<T, TolCheck, Traits>
{
public:
	template <typename U>
	CToleranceNomT(	std::string name, std::string desc, U rejectLo, U rejectHi,
		typename std::enable_if<!TolCheck<U>::SingleLimit>::type* = 0) :
		CToleranceImplBaseT(std::move(name), std::move(desc), rejectLo, rejectHi),
		m_bRelative(false)
	{}

	template <typename U>
	CToleranceNomT(std::string name, std::string desc, U reject,
		typename std::enable_if<TolCheck<U>::SingleLimit>::type* = 0) :
		CToleranceImplBaseT(std::move(name), std::move(desc), reject),
		m_bRelative(false)
	{}

	virtual bool HasRelativeMode() const { return true; }

	void SetNominal(T value) { m_dNominal = value; }
	T GetNominal() const { return m_dNominal; }

private:
	T m_dNominal;
	bool m_bRelative;
};

template <typename T, typename Traits = TolPerPinTraits>
class CToleranceMinT :	public CToleranceNomT<CToleranceMinT<T>, T, MinTol, Traits>
{
public:
	CToleranceMinT(	std::string name, std::string desc, T rejectLo) :
	  CToleranceNomT(std::move(name), std::move(desc), rejectLo)
	  {}
};

template <typename T, typename Traits = TolPerPinTraits>
class CToleranceMaxT : public CToleranceNomT<CToleranceMaxT<T>, T, MaxTol, Traits>
{
public:
	CToleranceMaxT(	std::string name, std::string desc, T rejectHi) :
	  CToleranceNomT(std::move(name), std::move(desc), rejectHi)
	  {}
};

template <typename T, typename Traits = TolPerPinTraits>
class CToleranceMinMaxT : public CToleranceNomT<CToleranceMinMaxT<T>, T, MinMaxTol, Traits>
{
public:
	CToleranceMinMaxT(	std::string name, std::string desc, T rejectLo, T rejectHi) :
	  CToleranceNomT(std::move(name), std::move(desc), rejectLo, rejectHi)
	  {}
};

typedef CToleranceMinMaxT<double>	CToleranceDev;
typedef CToleranceMinT<double>	CToleranceMin;
typedef CToleranceMaxT<double>	CToleranceMax;

template <typename T, typename Traits = TolPerPinTraits>
class CToleranceAbsMinT :	public CToleranceImplT<CToleranceMinT<T>, T, MinTol, Traits>
{
public:
	CToleranceAbsMinT(	std::string name, std::string desc, T rejectLo) :
	  CToleranceImplT(std::move(name), std::move(desc), rejectLo)
	  {}
};

template <typename T, typename Traits = TolPerPinTraits>
class CToleranceAbsMaxT : public CToleranceImplT<CToleranceMaxT<T>, T, MaxTol, Traits>
{
public:
	CToleranceAbsMaxT(	std::string name, std::string desc, T rejectHi) :
	  CToleranceImplT(std::move(name), std::move(desc), rejectHi)
	  {}
};

template <typename T, typename Traits = TolPerPinTraits>
class CToleranceAbsMinMaxT : public CToleranceImplT<CToleranceMinMaxT<T>, T, MinMaxTol, Traits>
{
public:
	CToleranceAbsMinMaxT(	std::string name, std::string desc, T rejectLo, T rejectHi) :
	  CToleranceImplT(std::move(name), std::move(desc), rejectLo, rejectHi)
	  {}
};

typedef CToleranceAbsMinMaxT<double>	CToleranceAbsDev;
typedef CToleranceAbsMinT<double>	CToleranceAbsMin;
typedef CToleranceAbsMaxT<double>	CToleranceAbsMax;

