#pragma once

// Tolerance Flags
#define TOL_2D		        0x00000001L
#define TOL_3D				0x00000002L
#define TOL_2D3D            0x00000003L	// TOL_2D | TOL_3D
#define TOL_PERPIN			0x00000004L

template <typename T, unsigned long t_dwFlags = 0>
class CTolTraits
{
public:
	using Type = T;

	static bool Is3D()
	{
		return (t_dwFlags & TOL_3D) != 0;
	}

	static bool Is2D()
	{
		return (t_dwFlags & TOL_2D) != 0;
	}

	static bool Is3DOnly()
	{
		return (Is3D() && !Is2D());
	}

	static bool HasPerPin()
	{
		return (t_dwFlags & TOL_PERPIN) != 0;
	}
};

template <typename T>
using Tol2DPerPinTraits = CTolTraits<T, TOL_2D | TOL_PERPIN>;

template <typename T>
using Tol3DPerPinTraits = CTolTraits<T, TOL_3D | TOL_PERPIN>;

template <typename T>
using TolPerPinTraits = CTolTraits<T, TOL_2D3D | TOL_PERPIN>;

template <typename T>
using Tol2DTraits = CTolTraits<T, TOL_2D> ;

template <typename T>
using Tol3DTraits = CTolTraits<T, TOL_3D>;

template <typename T>
using TolTraits = CTolTraits<T, TOL_2D3D>;