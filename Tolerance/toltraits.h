#pragma once

// Tolerance Flags
#define TOL_2D		        0x00000001L
#define TOL_3D				0x00000002L
#define TOL_2D3D            0x00000003L	// TOL_2D | TOL_3D
#define TOL_PERPIN			0x00000004L

template <unsigned long t_dwFlags = 0>
class CTolTraits
{
public:
	static bool Is3D()
	{
		return (t_dwFlags | TOL_3D) != 0;
	}

	static bool Is2D()
	{
		return (t_dwFlags | TOL_2D) != 0;
	}

	static bool Is3DOnly()
	{
		return (Is3D() && !Is2D());
	}

	static bool HasPerPin()
	{
		return (t_dwFlags | TOL_PERPIN) != 0;
	}
};

typedef CTolTraits<TOL_2D | TOL_PERPIN> CTol2DPerPinTraits;
typedef CTolTraits<TOL_3D | TOL_PERPIN> CTol3DPerPinTraits;
typedef CTolTraits<TOL_2D3D | TOL_PERPIN> CTolAllDPerPinTraits;