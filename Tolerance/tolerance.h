#pragma once

#include <string>
#include <map>
#include <type_traits>
#include "defines.h"

// Abstract base class for tolerance
struct CToleranceBase
{
	CToleranceBase(	std::string name, std::string desc, bool bHasPerPin, bool bTrue3D) :
		m_strName(std::move(name)),
		m_strDesc(std::move(desc)),
		m_bHasPerPin(bHasPerPin),
		m_bTrue3D(bTrue3D),
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

	bool HasPerPin() const { return m_bHasPerPin; }
	bool IsTrue3DOnly() const { return m_bTrue3D; }
	
	virtual bool HasRelativeMode() const { return false; }

private:
	const std::string m_strName;
	const bool m_bHasPerPin;
	const bool m_bTrue3D;

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
	DevTol(T dRejectLo, T dRejectHi) : 
		m_dRejectLo(dRejectLo),
		m_dRejectHi(dRejectHi)
	{}

	// check tolerance and return true if passes
	bool CheckTolerance(T value) const
	{
		return !(value < m_dRejectLo || value > m_dRejectHi);
	}

	static const bool bMinTol = true;
	static const bool bMaxTol = true;

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

	static const bool bMinTol = true;
	static const bool bMaxTol = false;

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

	static const bool bMinTol = false;
	static const bool bMaxTol = true;

};

template<typename T>
struct Nominal
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

private:
	T m_dNominal;
};

//template <typename T>
//struct TolTraits
//{
//	static const bool bFixedRelativeMode = true;
//};
//
//template <typename T>
//struct TolTraits<Nominal<T>>
//{
//	static const bool bFixedRelativeMode = false;
//};

// template class for tolerance
template <
	typename T = double,
	template <typename U> class TolCheck = DevTol			// DevTol, MinTol, MaxTol
>
class CToleranceImpl :	public CToleranceBase, 
						public TolCheck<T>
{
public:

	CToleranceImpl(	std::string name, std::string desc, T reject, bool bHasPerPin=true, bool bTrue3D=false) :
		CToleranceBase(std::move(name), std::move(desc), bHasPerPin, bTrue3D),
		TolCheck<T>(reject)
	{}
		
	CToleranceImpl(	std::string name, std::string desc, T rejectLo, T rejectHi, bool bHasPerPin=true, bool bTrue3D=false) :
		CToleranceBase(std::move(name), std::move(desc), bHasPerPin, bTrue3D),
		TolCheck<T>(rejectLo, rejectHi)
	{}

	bool IsMinTol() const override
	{
		return TolCheck<T>::bMinTol;
	};
		
	bool IsMaxTol() const override
	{
		return TolCheck<T>::bMaxTol;
	};
};

template <
	typename T = double,
	template <typename U> class TolCheck = DevTol			// DevTol, MinTol, MaxTol
>
class CToleranceNomT :	public CToleranceImpl<T, TolCheck>,
						public Nominal<T>
{
public:

	CToleranceNomT(	std::string name, std::string desc, T reject, bool bHasPerPin=true, bool bTrue3D=false) :
			CToleranceImpl(std::move(name), std::move(desc), bHasPerPin, bTrue3D)
	  {}

	CToleranceNomT(	std::string name, std::string desc, T rejectLo, T rejectHi, bool bHasPerPin=true, bool bTrue3D=false) :
			CToleranceImpl(std::move(name), std::move(desc), bHasPerPin, bTrue3D)
	  {}

	bool HasRelativeMode() const override { return true; }
};

CToleranceNomT<double, DevTol> m_test;

template <
	typename T = double,
	template <typename U, template <typename V> class TolChecker> class TolType = CToleranceNomT
>
class CToleranceMinT :	public TolType<T, MinTol>
{
public:
	CToleranceMinT(	std::string name, std::string desc, T rejectLo, bool bHasPerPin=true, bool bTrue3D=false) :
		TolType(std::move(name), std::move(desc), rejectLo, bHasPerPin, bTrue3D)
	{}
};

template <
	typename T = double,
	template <typename U, template <typename V> class TolChecker> class TolType = CToleranceNomT
>
class CToleranceMaxT :	public TolType<T, MaxTol>
{
public:
	CToleranceMaxT(	std::string name, std::string desc, T rejectHi, bool bHasPerPin=true, bool bTrue3D=false) :
	  TolType(std::move(name), std::move(desc), rejectHi, bHasPerPin, bTrue3D)
	  {}
};

template <
	typename T = double,
	template <typename U, template <typename V> class TolChecker> class TolType = CToleranceNomT
>
class CToleranceDevT :	public TolType<T, DevTol>
{
public:
	CToleranceDevT(	std::string name, std::string desc, T rejectLo, T rejectHi, bool bHasPerPin=true, bool bTrue3D=false) :
	  TolType(std::move(name), std::move(desc), rejectLo, rejectHi, bHasPerPin, bTrue3D)
	  {}
};

// typedef for commonly used tolerance types
typedef CToleranceDevT<>			CToleranceDev;
typedef CToleranceMinT<double>		CToleranceMin;
typedef CToleranceMaxT<double>		CToleranceMax;

