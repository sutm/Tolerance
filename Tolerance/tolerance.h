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

	bool IsDevTol() const { return IsMinTol() && IsMaxTol(); }
	virtual bool IsMinTol() const = 0;
	virtual bool IsMaxTol() const = 0;

	bool HasPerPin() const { return true; }
	virtual bool Is3DOnly() const = 0;
	
	virtual bool HasRelativeMode() const = 0;// { return false; }

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
struct DevTol
{
public:
	static const bool MinLimit = true;
	static const bool MaxLimit = true;
	static const bool SingleLimit = !(MinLimit && MaxLimit);

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

private:
	T m_dRejectHi;
};

template <typename Derived, typename T>
class Nominal
{
public:
	void SetNominal(T value)
	{
		m_dNominal = value;
	}

	T GetNominal() const
	{
		return m_dNominal;
	}

	virtual bool HasRelativeMode() const { return true; }

private:
	T m_dNominal;
	bool m_bRelative;
};

// template class for tolerance
template <
	typename Derived,
	typename T,
	template <typename U> class TolCheck = DevTol,			// DevTol, MinTol, MaxTol
	typename Traits = TolPerPinTraits
>
class CToleranceImpl :	public CToleranceBase, 
						public TolCheck<T>
{
public:
	template <typename U>
	CToleranceImpl(std::string name, std::string desc, U rejectLo, U rejectHi,
		typename std::enable_if<!TolCheck<U>::SingleLimit>::type* = 0) :
		CToleranceBase(std::move(name), std::move(desc)),
		TolCheck<T>(rejectLo, rejectHi)
	{}

	template <typename U>
	CToleranceImpl(	std::string name, std::string desc, U reject, 
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

	bool Is3DOnly() const override
	{
		return Traits::Is3DOnly();
	}

	virtual bool HasRelativeMode() const { return false; }
};

template <
	typename Derived,
	typename T,
	template <typename U> class TolCheck = DevTol,			// DevTol, MinTol, MaxTol
	typename Traits = TolPerPinTraits
>
class CToleranceNomT :	public CToleranceImpl<Derived, T, TolCheck, Traits>, 
						public Nominal<Derived, T>
{
public:
	template <typename U>
	CToleranceNomT(	std::string name, std::string desc, U rejectLo, U rejectHi,
		typename std::enable_if<!TolCheck<U>::SingleLimit>::type* = 0) :
		CToleranceImpl(std::move(name), std::move(desc), rejectLo, rejectHi)
	{}

	template <typename U>
	CToleranceNomT(std::string name, std::string desc, U reject,
		typename std::enable_if<TolCheck<U>::SingleLimit>::type* = 0) :
		CToleranceImpl(std::move(name), std::move(desc), reject)
	{}

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
class CToleranceDevT : public CToleranceNomT<CToleranceDevT<T>, T, DevTol, Traits>
{
public:
	CToleranceDevT(	std::string name, std::string desc, T rejectLo, T rejectHi) :
	  CToleranceNomT(std::move(name), std::move(desc), rejectLo, rejectHi)
	  {}
};
//
//template <typename T>
//class CToleranceMinT_NoNominal : public CToleranceImpl<T, MinTol>
//{
//public:
//	CToleranceMinT_NoNominal(std::string name, std::string desc, T rejectLo, bool bHasPerPin=true, bool bTrue3D=false) :
//	  CToleranceImpl(std::move(name), std::move(desc), rejectLo, bHasPerPin, bTrue3D)
//	  {}
//};
//
//template <typename T>
//class CToleranceMaxT_NoNominal : public CToleranceImpl<T, MaxTol>
//{
//public:
//	CToleranceMaxT_NoNominal(std::string name, std::string desc, T rejectHi, bool bHasPerPin=true, bool bTrue3D=false) :
//	  CToleranceImpl(std::move(name), std::move(desc), rejectHi, bHasPerPin, bTrue3D)
//	  {}
//};
//
//template <typename T>
//class CToleranceDevT_NoNominal : public CToleranceImpl<T, DevTol>
//{
//public:
//	CToleranceDevT_NoNominal(std::string name, std::string desc, T rejectLo, T rejectHi, bool bHasPerPin=true, bool bTrue3D=false) :
//	  CToleranceImpl(std::move(name), std::move(desc), rejectLo, rejectHi, bHasPerPin, bTrue3D)
//	  {}
//};

typedef CToleranceDevT<double>		CToleranceDev;
typedef CToleranceMinT<double>		CToleranceMin;
typedef CToleranceMaxT<double>		CToleranceMax;



