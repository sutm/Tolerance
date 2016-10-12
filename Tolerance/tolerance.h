#pragma once

#include <string>
#include <map>
#include <type_traits>
#include "defines.h"

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

	virtual bool HasPerPin() const = 0;
	virtual bool Is3DOnly() const = 0;
	virtual bool IsFixedRelativeMode() const = 0;

protected:
	const std::string m_strName;
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
struct HasNominal
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

template<typename T>
struct NoNominal
{
};

struct HasPerPin
{
};
typedef void NoPerPin;

struct Category3D
{
};
typedef void CategoryAny;

template <typename T>
struct TolTraits
{
	static const bool bHasPerPin = false;
	static const bool bFor3DOnly = false;
	static const bool bFixedRelativeMode = false;
};

template <>
struct TolTraits<HasPerPin>
{
	static const bool bHasPerPin = true;
};

template <>
struct TolTraits<Category3D>
{
	static const bool bFor3DOnly = true;
};

template <typename T>
struct TolTraits<NoNominal<T>>
{
	static const bool bFixedRelativeMode = true;
};

// template class for tolerance
template <
	typename T = double,
	template <typename U> class TolCheck = DevTol,			// DevTol, MinTol, MaxTol
	typename PerPinType = HasPerPin,						// NoPerPin, HasPerPin
	typename Category2D3DType = CategoryAny,				// CategoryAny, Category3D
	template <typename V> class NominalType = HasNominal	// HasNominal, NoNominal
>
class CToleranceImpl :	public CToleranceBase, 
						public TolCheck<T>,
						public NominalType<T>
{
public:

	CToleranceImpl(	std::string name, std::string desc, T reject) :
		CToleranceBase(std::move(name), std::move(desc)),
		TolCheck<T>(reject)
	{}
		
	CToleranceImpl(	std::string name, std::string desc, T rejectLo, T rejectHi) :
		CToleranceBase(std::move(name), std::move(desc)),
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

	bool HasPerPin() const override
	{
		return TolTraits<PerPinType>::bHasPerPin;
	}

	bool Is3DOnly() const override
	{
		return TolTraits<Category2D3DType>::bFor3DOnly;
	}

	bool IsFixedRelativeMode() const override
	{
		return TolTraits<NominalType<T>>::bFixedRelativeMode;
	}
};

template <
	typename T = double,
	typename PerPinType = HasPerPin,						// NoPerPin, HasPerPin
	typename Category2D3DType = CategoryAny,				// CategoryAny, Category3D
	template <typename V> class NominalType = HasNominal	// HasNominal, NoNominal
>
class CToleranceMinT :	public CToleranceImpl<T, 
											MinTol, 
											PerPinType, 
											Category2D3DType, 
											NominalType>
{
public:
	CToleranceMinT(	std::string name, std::string desc, T rejectLo) :
		CToleranceImpl(std::move(name), std::move(desc), rejectLo)
	{}
};

template <
	typename T = double,
	typename PerPinType = HasPerPin,						// NoPerPin, HasPerPin
	typename Category2D3DType = CategoryAny,				// CategoryAny, Category3D
	template <typename V> class NominalType = HasNominal	// HasNominal, NoNominal
>
class CToleranceMaxT :	public CToleranceImpl<T, 
											MaxTol, 
											PerPinType, 
											Category2D3DType, 
											NominalType>
{
public:
	CToleranceMaxT(	std::string name, std::string desc, T rejectHi) :
	  CToleranceImpl(std::move(name), std::move(desc), rejectHi)
	  {}
};

template <
	typename T = double,
	typename PerPinType = HasPerPin,						// NoPerPin, HasPerPin
	typename Category2D3DType = CategoryAny,				// CategoryAny, Category3D
	template <typename V> class NominalType = HasNominal	// HasNominal, NoNominal
>
class CToleranceDevT :	public CToleranceImpl<T, 
											DevTol, 
											PerPinType, 
											Category2D3DType, 
											NominalType>
{
public:
	CToleranceDevT(	std::string name, std::string desc, T rejectLo, T rejectHi) :
	  CToleranceImpl(std::move(name), std::move(desc), rejectLo, rejectHi)
	  {}
};

// typedef for commonly used tolerance types
typedef CToleranceDevT<>			CToleranceDev;
typedef CToleranceMinT<double>		CToleranceMin;
typedef CToleranceMaxT<double>		CToleranceMax;

