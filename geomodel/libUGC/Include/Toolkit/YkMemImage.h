﻿/*
 *
 * YkMemImage.h
 *
 * Copyright (C) 2021 SuperMap Software Co., Ltd.
 *
 * Yukon is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>. *
 */

#ifndef AFX_YKMEMIMAGE_H__42932F8E_3F0F_4219_9576_FA2F58F121A1__INCLUDED_
#define AFX_YKMEMIMAGE_H__42932F8E_3F0F_4219_9576_FA2F58F121A1__INCLUDED_

#pragma once

#include "Stream/YkDefines.h"
#include <cmath>
#include <algorithm>

namespace squish {

//=========================================maths=========================================

class Vec3
{
public:
	typedef Vec3 const& Arg;

	Vec3()
	{
	}

	explicit Vec3( float s )
	{
		m_x = s;
		m_y = s;
		m_z = s;
	}

	Vec3( float x, float y, float z )
	{
		m_x = x;
		m_y = y;
		m_z = z;
	}

	float X() const { return m_x; }
	float Y() const { return m_y; }
	float Z() const { return m_z; }

	Vec3 operator-() const
	{
		return Vec3( -m_x, -m_y, -m_z );
	}

	Vec3& operator+=( Arg v )
	{
		m_x += v.m_x;
		m_y += v.m_y;
		m_z += v.m_z;
		return *this;
	}

	Vec3& operator-=( Arg v )
	{
		m_x -= v.m_x;
		m_y -= v.m_y;
		m_z -= v.m_z;
		return *this;
	}

	Vec3& operator*=( Arg v )
	{
		m_x *= v.m_x;
		m_y *= v.m_y;
		m_z *= v.m_z;
		return *this;
	}

	Vec3& operator*=( float s )
	{
		m_x *= s;
		m_y *= s;
		m_z *= s;
		return *this;
	}

	Vec3& operator/=( Arg v )
	{
		m_x /= v.m_x;
		m_y /= v.m_y;
		m_z /= v.m_z;
		return *this;
	}

	Vec3& operator/=( float s )
	{
		float t = 1.0f/s;
		m_x *= t;
		m_y *= t;
		m_z *= t;
		return *this;
	}

	friend Vec3 operator+( Arg left, Arg right )
	{
		Vec3 copy( left );
		return copy += right;
	}

	friend Vec3 operator-( Arg left, Arg right )
	{
		Vec3 copy( left );
		return copy -= right;
	}

	friend Vec3 operator*( Arg left, Arg right )
	{
		Vec3 copy( left );
		return copy *= right;
	}

	friend Vec3 operator*( Arg left, float right )
	{
		Vec3 copy( left );
		return copy *= right;
	}

	friend Vec3 operator*( float left, Arg right )
	{
		Vec3 copy( right );
		return copy *= left;
	}

	friend Vec3 operator/( Arg left, Arg right )
	{
		Vec3 copy( left );
		return copy /= right;
	}

	friend Vec3 operator/( Arg left, float right )
	{
		Vec3 copy( left );
		return copy /= right;
	}

	friend float Dot( Arg left, Arg right )
	{
		return left.m_x*right.m_x + left.m_y*right.m_y + left.m_z*right.m_z;
	}

	friend Vec3 YkMin( Arg left, Arg right )
	{
		return Vec3(
			std::min( left.m_x, right.m_x ), 
			std::min( left.m_y, right.m_y ), 
			std::min( left.m_z, right.m_z )
			);
	}

	friend Vec3 YkMax( Arg left, Arg right )
	{
		return Vec3(
			std::max( left.m_x, right.m_x ), 
			std::max( left.m_y, right.m_y ), 
			std::max( left.m_z, right.m_z )
			);
	}

	friend Vec3 Truncate( Arg v )
	{
		return Vec3(
			v.m_x > 0.0f ? std::floor( v.m_x ) : std::ceil( v.m_x ), 
			v.m_y > 0.0f ? std::floor( v.m_y ) : std::ceil( v.m_y ), 
			v.m_z > 0.0f ? std::floor( v.m_z ) : std::ceil( v.m_z )
			);
	}

private:
	float m_x;
	float m_y;
	float m_z;
};

inline float LengthSquared( Vec3::Arg v )
{
	return Dot( v, v );
}

class Sym3x3
{
public:
	Sym3x3()
	{
	}

	Sym3x3( float s )
	{
		for( int i = 0; i < 6; ++i )
			m_x[i] = s;
	}

	float operator[]( int index ) const
	{
		return m_x[index];
	}

	float& operator[]( int index )
	{
		return m_x[index];
	}

private:
	float m_x[6];
};

Sym3x3 ComputeWeightedCovariance( int n, Vec3 const* points, float const* weights );
Vec3 ComputePrincipleComponent( Sym3x3 const& matrix );

//=========================================simd_float=========================================

#define VEC4_CONST( X ) Vec4( X )

class Vec4
{
public:
	typedef Vec4 const& Arg;

	Vec4() {}

	explicit Vec4( float s )
		: m_x( s ),
		m_y( s ),
		m_z( s ),
		m_w( s )
	{
	}

	Vec4( float x, float y, float z, float w )
		: m_x( x ),
		m_y( y ),
		m_z( z ),
		m_w( w )
	{
	}

	Vec3 GetVec3() const
	{
		return Vec3( m_x, m_y, m_z );
	}

	Vec4 SplatX() const { return Vec4( m_x ); }
	Vec4 SplatY() const { return Vec4( m_y ); }
	Vec4 SplatZ() const { return Vec4( m_z ); }
	Vec4 SplatW() const { return Vec4( m_w ); }

	Vec4& operator+=( Arg v )
	{
		m_x += v.m_x;
		m_y += v.m_y;
		m_z += v.m_z;
		m_w += v.m_w;
		return *this;
	}

	Vec4& operator-=( Arg v )
	{
		m_x -= v.m_x;
		m_y -= v.m_y;
		m_z -= v.m_z;
		m_w -= v.m_w;
		return *this;
	}

	Vec4& operator*=( Arg v )
	{
		m_x *= v.m_x;
		m_y *= v.m_y;
		m_z *= v.m_z;
		m_w *= v.m_w;
		return *this;
	}

	friend Vec4 operator+( Vec4::Arg left, Vec4::Arg right  )
	{
		Vec4 copy( left );
		return copy += right;
	}

	friend Vec4 operator-( Vec4::Arg left, Vec4::Arg right  )
	{
		Vec4 copy( left );
		return copy -= right;
	}

	friend Vec4 operator*( Vec4::Arg left, Vec4::Arg right  )
	{
		Vec4 copy( left );
		return copy *= right;
	}

	//! Returns a*b + c
	friend Vec4 MultiplyAdd( Vec4::Arg a, Vec4::Arg b, Vec4::Arg c )
	{
		return a*b + c;
	}

	//! Returns -( a*b - c )
	friend Vec4 NegativeMultiplySubtract( Vec4::Arg a, Vec4::Arg b, Vec4::Arg c )
	{
		return c - a*b;
	}

	friend Vec4 Reciprocal( Vec4::Arg v )
	{
		return Vec4( 
			1.0f/v.m_x, 
			1.0f/v.m_y, 
			1.0f/v.m_z, 
			1.0f/v.m_w 
			);
	}

	friend Vec4 YkMin( Vec4::Arg left, Vec4::Arg right )
	{
		return Vec4( 
			std::min( left.m_x, right.m_x ), 
			std::min( left.m_y, right.m_y ), 
			std::min( left.m_z, right.m_z ), 
			std::min( left.m_w, right.m_w ) 
			);
	}

	friend Vec4 YkMax( Vec4::Arg left, Vec4::Arg right )
	{
		return Vec4( 
			std::max( left.m_x, right.m_x ), 
			std::max( left.m_y, right.m_y ), 
			std::max( left.m_z, right.m_z ), 
			std::max( left.m_w, right.m_w ) 
			);
	}

	friend Vec4 Truncate( Vec4::Arg v )
	{
		return Vec4(
			v.m_x > 0.0f ? std::floor( v.m_x ) : std::ceil( v.m_x ), 
			v.m_y > 0.0f ? std::floor( v.m_y ) : std::ceil( v.m_y ), 
			v.m_z > 0.0f ? std::floor( v.m_z ) : std::ceil( v.m_z ),
			v.m_w > 0.0f ? std::floor( v.m_w ) : std::ceil( v.m_w )
			);
	}

	friend bool CompareAnyLessThan( Vec4::Arg left, Vec4::Arg right ) 
	{
		return left.m_x < right.m_x
			|| left.m_y < right.m_y
			|| left.m_z < right.m_z
			|| left.m_w < right.m_w;
	}

private:
	float m_x;
	float m_y;
	float m_z;
	float m_w;
};

//=========================================squish=========================================

// -----------------------------------------------------------------------------

//! Typedef a quantity that is a single unsigned byte.
typedef unsigned char u8;

// -----------------------------------------------------------------------------

enum
{
	//! Use DXT1 compression.
	kDxt1 = ( 1 << 0 ), 
	
	//! Use DXT3 compression.
	kDxt3 = ( 1 << 1 ), 
	
	//! Use DXT5 compression.
	kDxt5 = ( 1 << 2 ), 
	
	//! Use a very slow but very high quality colour compressor.
	kColourIterativeClusterFit = ( 1 << 8 ),	
	
	//! Use a slow but high quality colour compressor (the default).
	kColourClusterFit = ( 1 << 3 ),	
	
	//! Use a fast but low quality colour compressor.
	kColourRangeFit	= ( 1 << 4 ),
	
	//! Weight the colour by alpha during cluster fit (disabled by default).
	kWeightColourByAlpha = ( 1 << 7 )
};

// -----------------------------------------------------------------------------

/*! @brief Compresses a 4x4 block of pixels.

	@param rgba		The rgba values of the 16 source pixels.
	@param mask		The valid pixel mask.
	@param block	Storage for the compressed DXT block.
	@param flags	Compression flags.
	@param metric	An optional perceptual metric.
	
	The source pixels should be presented as a contiguous array of 16 rgba
	values, with each component as 1 byte each. In memory this should be:
	
		{ r1, g1, b1, a1, .... , r16, g16, b16, a16 }
		
	The mask parameter enables only certain pixels within the block. The lowest
	bit enables the first pixel and so on up to the 16th bit. Bits beyond the
	16th bit are ignored. Pixels that are not enabled are allowed to take
	arbitrary colours in the output block. An example of how this can be used
	is in the CompressImage function to disable pixels outside the bounds of
	the image when the width or height is not divisible by 4.
	
	The flags parameter should specify either kDxt1, kDxt3 or kDxt5 compression, 
	however, DXT1 will be used by default if none is specified. When using DXT1 
	compression, 8 bytes of storage are required for the compressed DXT block. 
	DXT3 and DXT5 compression require 16 bytes of storage per block.
	
	The flags parameter can also specify a preferred colour compressor to use 
	when fitting the RGB components of the data. Possible colour compressors 
	are: kColourClusterFit (the default), kColourRangeFit (very fast, low 
	quality) or kColourIterativeClusterFit (slowest, best quality).
		
	When using kColourClusterFit or kColourIterativeClusterFit, an additional 
	flag can be specified to weight the importance of each pixel by its alpha 
	value. For images that are rendered using alpha blending, this can 
	significantly increase the perceived quality.
	
	The metric parameter can be used to weight the relative importance of each
	colour channel, or pass NULL to use the default uniform weight of 
	{ 1.0f, 1.0f, 1.0f }. This replaces the previous flag-based control that 
	allowed either uniform or "perceptual" weights with the fixed values
	{ 0.2126f, 0.7152f, 0.0722f }. If non-NULL, the metric should point to a 
	contiguous array of 3 floats.
*/
void CompressMasked( u8 const* rgba, int mask, void* block, int flags, float* metric = 0 );

// -----------------------------------------------------------------------------

/*! @brief Compresses a 4x4 block of pixels.

	@param rgba		The rgba values of the 16 source pixels.
	@param block	Storage for the compressed DXT block.
	@param flags	Compression flags.
	@param metric	An optional perceptual metric.
	
	The source pixels should be presented as a contiguous array of 16 rgba
	values, with each component as 1 byte each. In memory this should be:
	
		{ r1, g1, b1, a1, .... , r16, g16, b16, a16 }
	
	The flags parameter should specify either kDxt1, kDxt3 or kDxt5 compression, 
	however, DXT1 will be used by default if none is specified. When using DXT1 
	compression, 8 bytes of storage are required for the compressed DXT block. 
	DXT3 and DXT5 compression require 16 bytes of storage per block.
	
	The flags parameter can also specify a preferred colour compressor to use 
	when fitting the RGB components of the data. Possible colour compressors 
	are: kColourClusterFit (the default), kColourRangeFit (very fast, low 
	quality) or kColourIterativeClusterFit (slowest, best quality).
		
	When using kColourClusterFit or kColourIterativeClusterFit, an additional 
	flag can be specified to weight the importance of each pixel by its alpha 
	value. For images that are rendered using alpha blending, this can 
	significantly increase the perceived quality.
	
	The metric parameter can be used to weight the relative importance of each
	colour channel, or pass NULL to use the default uniform weight of 
	{ 1.0f, 1.0f, 1.0f }. This replaces the previous flag-based control that 
	allowed either uniform or "perceptual" weights with the fixed values
	{ 0.2126f, 0.7152f, 0.0722f }. If non-NULL, the metric should point to a 
	contiguous array of 3 floats.
	
	This method is an inline that calls CompressMasked with a mask of 0xffff, 
	provided for compatibility with older versions of squish.
*/
inline void Compress( u8 const* rgba, void* block, int flags, float* metric = 0 )
{
	CompressMasked( rgba, 0xffff, block, flags, metric );
}

// -----------------------------------------------------------------------------

/*! @brief Decompresses a 4x4 block of pixels.

	@param rgba		Storage for the 16 decompressed pixels.
	@param block	The compressed DXT block.
	@param flags	Compression flags.

	The decompressed pixels will be written as a contiguous array of 16 rgba
	values, with each component as 1 byte each. In memory this is:
	
		{ r1, g1, b1, a1, .... , r16, g16, b16, a16 }
	
	The flags parameter should specify either kDxt1, kDxt3 or kDxt5 compression, 
	however, DXT1 will be used by default if none is specified. All other flags 
	are ignored.
*/
void Decompress( u8* rgba, void const* block, int flags );

// -----------------------------------------------------------------------------

/*! @brief Compresses an image in memory.

	@param rgba		The pixels of the source.
	@param width	The width of the source image.
	@param height	The height of the source image.
	@param blocks	Storage for the compressed output.
	@param flags	Compression flags.
	@param metric	An optional perceptual metric.
	
	The source pixels should be presented as a contiguous array of width*height
	rgba values, with each component as 1 byte each. In memory this should be:
	
		{ r1, g1, b1, a1, .... , rn, gn, bn, an } for n = width*height
		
	The flags parameter should specify either kDxt1, kDxt3 or kDxt5 compression, 
	however, DXT1 will be used by default if none is specified. When using DXT1 
	compression, 8 bytes of storage are required for each compressed DXT block. 
	DXT3 and DXT5 compression require 16 bytes of storage per block.
	
	The flags parameter can also specify a preferred colour compressor to use 
	when fitting the RGB components of the data. Possible colour compressors 
	are: kColourClusterFit (the default), kColourRangeFit (very fast, low 
	quality) or kColourIterativeClusterFit (slowest, best quality).
		
	When using kColourClusterFit or kColourIterativeClusterFit, an additional 
	flag can be specified to weight the importance of each pixel by its alpha 
	value. For images that are rendered using alpha blending, this can 
	significantly increase the perceived quality.
	
	The metric parameter can be used to weight the relative importance of each
	colour channel, or pass NULL to use the default uniform weight of 
	{ 1.0f, 1.0f, 1.0f }. This replaces the previous flag-based control that 
	allowed either uniform or "perceptual" weights with the fixed values
	{ 0.2126f, 0.7152f, 0.0722f }. If non-NULL, the metric should point to a 
	contiguous array of 3 floats.
	
	Internally this function calls squish::CompressMasked for each block, which 
	allows for pixels outside the image to take arbitrary values. The function 
	squish::GetStorageRequirements can be called to compute the amount of memory
	to allocate for the compressed output.
*/
void CompressImage( u8 const* rgba, int width, int height, void* blocks, int flags, float* metric = 0 );

// -----------------------------------------------------------------------------

/*! @brief Decompresses an image in memory.

	@param rgba		Storage for the decompressed pixels.
	@param width	The width of the source image.
	@param height	The height of the source image.
	@param blocks	The compressed DXT blocks.
	@param flags	Compression flags.
	
	The decompressed pixels will be written as a contiguous array of width*height
	16 rgba values, with each component as 1 byte each. In memory this is:
	
		{ r1, g1, b1, a1, .... , rn, gn, bn, an } for n = width*height
		
	The flags parameter should specify either kDxt1, kDxt3 or kDxt5 compression, 
	however, DXT1 will be used by default if none is specified. All other flags 
	are ignored.

	Internally this function calls squish::Decompress for each block.
*/
void DecompressImage( u8* rgba, int width, int height, void const* blocks, int flags );

// -----------------------------------------------------------------------------

//=========================================alpha=========================================

void CompressAlphaDxt3( u8 const* rgba, int mask, void* block );
void CompressAlphaDxt5( u8 const* rgba, int mask, void* block );

void DecompressAlphaDxt3( u8* rgba, void const* block );
void DecompressAlphaDxt5( u8* rgba, void const* block );

//=========================================colourset=========================================

/*! @brief Represents a set of block colours
*/
class ColourSet
{
public:
	ColourSet( u8 const* rgba, int mask, int flags );

	int GetCount() const { return m_count; }
	Vec3 const* GetPoints() const { return m_points; }
	float const* GetWeights() const { return m_weights; }
	bool IsTransparent() const { return m_transparent; }

	void RemapIndices( u8 const* source, u8* target ) const;

private:
	int m_count;
	Vec3 m_points[16];
	float m_weights[16];
	int m_remap[16];
	bool m_transparent;
};

//=========================================colourfit=========================================

class ColourFit
{
public:
	ColourFit( ColourSet const* colours, int flags );
	virtual ~ColourFit();

	void Compress( void* block );

protected:
	virtual void Compress3( void* block ) = 0;
	virtual void Compress4( void* block ) = 0;

	ColourSet const* m_colours;
	int m_flags;
};

//=========================================clusterfit=========================================

class ClusterFit : public ColourFit
{
public:
	ClusterFit( ColourSet const* colours, int flags, float* metric );

private:
	bool ConstructOrdering( Vec3 const& axis, int iteration );

	virtual void Compress3( void* block );
	virtual void Compress4( void* block );

	enum { kMaxIterations = 8 };

	int m_iterationCount;
	Vec3 m_principle;
	u8 m_order[16*kMaxIterations];
	Vec4 m_points_weights[16];
	Vec4 m_xsum_wsum;
	Vec4 m_metric;
	Vec4 m_besterror;
};

//=========================================colourblock=========================================

void WriteColourBlock3( Vec3::Arg start, Vec3::Arg end, u8 const* indices, void* block );
void WriteColourBlock4( Vec3::Arg start, Vec3::Arg end, u8 const* indices, void* block );

void DecompressColour( u8* rgba, void const* block, bool isDxt1 );

//=========================================rangefit=========================================

class RangeFit : public ColourFit
{
public:
	RangeFit( ColourSet const* colours, int flags, float* metric );

private:
	virtual void Compress3( void* block );
	virtual void Compress4( void* block );

	Vec3 m_metric;
	Vec3 m_start;
	Vec3 m_end;
	float m_besterror;
};

//=========================================singlecolourlookup=========================================

struct SourceBlock
{
	u8 start;
	u8 end;
	u8 error;
};

struct SingleColourLookup
{
	SourceBlock sources[2];
};

static SingleColourLookup const lookup_5_3[] = 
{
	{ { { 0, 0, 0 }, { 0, 0, 0 } } },
	{ { { 0, 0, 1 }, { 0, 0, 1 } } },
	{ { { 0, 0, 2 }, { 0, 0, 2 } } },
	{ { { 0, 0, 3 }, { 0, 1, 1 } } },
	{ { { 0, 0, 4 }, { 0, 1, 0 } } },
	{ { { 1, 0, 3 }, { 0, 1, 1 } } },
	{ { { 1, 0, 2 }, { 0, 1, 2 } } },
	{ { { 1, 0, 1 }, { 0, 2, 1 } } },
	{ { { 1, 0, 0 }, { 0, 2, 0 } } },
	{ { { 1, 0, 1 }, { 0, 2, 1 } } },
	{ { { 1, 0, 2 }, { 0, 2, 2 } } },
	{ { { 1, 0, 3 }, { 0, 3, 1 } } },
	{ { { 1, 0, 4 }, { 0, 3, 0 } } },
	{ { { 2, 0, 3 }, { 0, 3, 1 } } },
	{ { { 2, 0, 2 }, { 0, 3, 2 } } },
	{ { { 2, 0, 1 }, { 0, 4, 1 } } },
	{ { { 2, 0, 0 }, { 0, 4, 0 } } },
	{ { { 2, 0, 1 }, { 0, 4, 1 } } },
	{ { { 2, 0, 2 }, { 0, 4, 2 } } },
	{ { { 2, 0, 3 }, { 0, 5, 1 } } },
	{ { { 2, 0, 4 }, { 0, 5, 0 } } },
	{ { { 3, 0, 3 }, { 0, 5, 1 } } },
	{ { { 3, 0, 2 }, { 0, 5, 2 } } },
	{ { { 3, 0, 1 }, { 0, 6, 1 } } },
	{ { { 3, 0, 0 }, { 0, 6, 0 } } },
	{ { { 3, 0, 1 }, { 0, 6, 1 } } },
	{ { { 3, 0, 2 }, { 0, 6, 2 } } },
	{ { { 3, 0, 3 }, { 0, 7, 1 } } },
	{ { { 3, 0, 4 }, { 0, 7, 0 } } },
	{ { { 4, 0, 4 }, { 0, 7, 1 } } },
	{ { { 4, 0, 3 }, { 0, 7, 2 } } },
	{ { { 4, 0, 2 }, { 1, 7, 1 } } },
	{ { { 4, 0, 1 }, { 1, 7, 0 } } },
	{ { { 4, 0, 0 }, { 0, 8, 0 } } },
	{ { { 4, 0, 1 }, { 0, 8, 1 } } },
	{ { { 4, 0, 2 }, { 2, 7, 1 } } },
	{ { { 4, 0, 3 }, { 2, 7, 0 } } },
	{ { { 4, 0, 4 }, { 0, 9, 0 } } },
	{ { { 5, 0, 3 }, { 0, 9, 1 } } },
	{ { { 5, 0, 2 }, { 3, 7, 1 } } },
	{ { { 5, 0, 1 }, { 3, 7, 0 } } },
	{ { { 5, 0, 0 }, { 0, 10, 0 } } },
	{ { { 5, 0, 1 }, { 0, 10, 1 } } },
	{ { { 5, 0, 2 }, { 0, 10, 2 } } },
	{ { { 5, 0, 3 }, { 0, 11, 1 } } },
	{ { { 5, 0, 4 }, { 0, 11, 0 } } },
	{ { { 6, 0, 3 }, { 0, 11, 1 } } },
	{ { { 6, 0, 2 }, { 0, 11, 2 } } },
	{ { { 6, 0, 1 }, { 0, 12, 1 } } },
	{ { { 6, 0, 0 }, { 0, 12, 0 } } },
	{ { { 6, 0, 1 }, { 0, 12, 1 } } },
	{ { { 6, 0, 2 }, { 0, 12, 2 } } },
	{ { { 6, 0, 3 }, { 0, 13, 1 } } },
	{ { { 6, 0, 4 }, { 0, 13, 0 } } },
	{ { { 7, 0, 3 }, { 0, 13, 1 } } },
	{ { { 7, 0, 2 }, { 0, 13, 2 } } },
	{ { { 7, 0, 1 }, { 0, 14, 1 } } },
	{ { { 7, 0, 0 }, { 0, 14, 0 } } },
	{ { { 7, 0, 1 }, { 0, 14, 1 } } },
	{ { { 7, 0, 2 }, { 0, 14, 2 } } },
	{ { { 7, 0, 3 }, { 0, 15, 1 } } },
	{ { { 7, 0, 4 }, { 0, 15, 0 } } },
	{ { { 8, 0, 4 }, { 0, 15, 1 } } },
	{ { { 8, 0, 3 }, { 0, 15, 2 } } },
	{ { { 8, 0, 2 }, { 1, 15, 1 } } },
	{ { { 8, 0, 1 }, { 1, 15, 0 } } },
	{ { { 8, 0, 0 }, { 0, 16, 0 } } },
	{ { { 8, 0, 1 }, { 0, 16, 1 } } },
	{ { { 8, 0, 2 }, { 2, 15, 1 } } },
	{ { { 8, 0, 3 }, { 2, 15, 0 } } },
	{ { { 8, 0, 4 }, { 0, 17, 0 } } },
	{ { { 9, 0, 3 }, { 0, 17, 1 } } },
	{ { { 9, 0, 2 }, { 3, 15, 1 } } },
	{ { { 9, 0, 1 }, { 3, 15, 0 } } },
	{ { { 9, 0, 0 }, { 0, 18, 0 } } },
	{ { { 9, 0, 1 }, { 0, 18, 1 } } },
	{ { { 9, 0, 2 }, { 0, 18, 2 } } },
	{ { { 9, 0, 3 }, { 0, 19, 1 } } },
	{ { { 9, 0, 4 }, { 0, 19, 0 } } },
	{ { { 10, 0, 3 }, { 0, 19, 1 } } },
	{ { { 10, 0, 2 }, { 0, 19, 2 } } },
	{ { { 10, 0, 1 }, { 0, 20, 1 } } },
	{ { { 10, 0, 0 }, { 0, 20, 0 } } },
	{ { { 10, 0, 1 }, { 0, 20, 1 } } },
	{ { { 10, 0, 2 }, { 0, 20, 2 } } },
	{ { { 10, 0, 3 }, { 0, 21, 1 } } },
	{ { { 10, 0, 4 }, { 0, 21, 0 } } },
	{ { { 11, 0, 3 }, { 0, 21, 1 } } },
	{ { { 11, 0, 2 }, { 0, 21, 2 } } },
	{ { { 11, 0, 1 }, { 0, 22, 1 } } },
	{ { { 11, 0, 0 }, { 0, 22, 0 } } },
	{ { { 11, 0, 1 }, { 0, 22, 1 } } },
	{ { { 11, 0, 2 }, { 0, 22, 2 } } },
	{ { { 11, 0, 3 }, { 0, 23, 1 } } },
	{ { { 11, 0, 4 }, { 0, 23, 0 } } },
	{ { { 12, 0, 4 }, { 0, 23, 1 } } },
	{ { { 12, 0, 3 }, { 0, 23, 2 } } },
	{ { { 12, 0, 2 }, { 1, 23, 1 } } },
	{ { { 12, 0, 1 }, { 1, 23, 0 } } },
	{ { { 12, 0, 0 }, { 0, 24, 0 } } },
	{ { { 12, 0, 1 }, { 0, 24, 1 } } },
	{ { { 12, 0, 2 }, { 2, 23, 1 } } },
	{ { { 12, 0, 3 }, { 2, 23, 0 } } },
	{ { { 12, 0, 4 }, { 0, 25, 0 } } },
	{ { { 13, 0, 3 }, { 0, 25, 1 } } },
	{ { { 13, 0, 2 }, { 3, 23, 1 } } },
	{ { { 13, 0, 1 }, { 3, 23, 0 } } },
	{ { { 13, 0, 0 }, { 0, 26, 0 } } },
	{ { { 13, 0, 1 }, { 0, 26, 1 } } },
	{ { { 13, 0, 2 }, { 0, 26, 2 } } },
	{ { { 13, 0, 3 }, { 0, 27, 1 } } },
	{ { { 13, 0, 4 }, { 0, 27, 0 } } },
	{ { { 14, 0, 3 }, { 0, 27, 1 } } },
	{ { { 14, 0, 2 }, { 0, 27, 2 } } },
	{ { { 14, 0, 1 }, { 0, 28, 1 } } },
	{ { { 14, 0, 0 }, { 0, 28, 0 } } },
	{ { { 14, 0, 1 }, { 0, 28, 1 } } },
	{ { { 14, 0, 2 }, { 0, 28, 2 } } },
	{ { { 14, 0, 3 }, { 0, 29, 1 } } },
	{ { { 14, 0, 4 }, { 0, 29, 0 } } },
	{ { { 15, 0, 3 }, { 0, 29, 1 } } },
	{ { { 15, 0, 2 }, { 0, 29, 2 } } },
	{ { { 15, 0, 1 }, { 0, 30, 1 } } },
	{ { { 15, 0, 0 }, { 0, 30, 0 } } },
	{ { { 15, 0, 1 }, { 0, 30, 1 } } },
	{ { { 15, 0, 2 }, { 0, 30, 2 } } },
	{ { { 15, 0, 3 }, { 0, 31, 1 } } },
	{ { { 15, 0, 4 }, { 0, 31, 0 } } },
	{ { { 16, 0, 4 }, { 0, 31, 1 } } },
	{ { { 16, 0, 3 }, { 0, 31, 2 } } },
	{ { { 16, 0, 2 }, { 1, 31, 1 } } },
	{ { { 16, 0, 1 }, { 1, 31, 0 } } },
	{ { { 16, 0, 0 }, { 4, 28, 0 } } },
	{ { { 16, 0, 1 }, { 4, 28, 1 } } },
	{ { { 16, 0, 2 }, { 2, 31, 1 } } },
	{ { { 16, 0, 3 }, { 2, 31, 0 } } },
	{ { { 16, 0, 4 }, { 4, 29, 0 } } },
	{ { { 17, 0, 3 }, { 4, 29, 1 } } },
	{ { { 17, 0, 2 }, { 3, 31, 1 } } },
	{ { { 17, 0, 1 }, { 3, 31, 0 } } },
	{ { { 17, 0, 0 }, { 4, 30, 0 } } },
	{ { { 17, 0, 1 }, { 4, 30, 1 } } },
	{ { { 17, 0, 2 }, { 4, 30, 2 } } },
	{ { { 17, 0, 3 }, { 4, 31, 1 } } },
	{ { { 17, 0, 4 }, { 4, 31, 0 } } },
	{ { { 18, 0, 3 }, { 4, 31, 1 } } },
	{ { { 18, 0, 2 }, { 4, 31, 2 } } },
	{ { { 18, 0, 1 }, { 5, 31, 1 } } },
	{ { { 18, 0, 0 }, { 5, 31, 0 } } },
	{ { { 18, 0, 1 }, { 5, 31, 1 } } },
	{ { { 18, 0, 2 }, { 5, 31, 2 } } },
	{ { { 18, 0, 3 }, { 6, 31, 1 } } },
	{ { { 18, 0, 4 }, { 6, 31, 0 } } },
	{ { { 19, 0, 3 }, { 6, 31, 1 } } },
	{ { { 19, 0, 2 }, { 6, 31, 2 } } },
	{ { { 19, 0, 1 }, { 7, 31, 1 } } },
	{ { { 19, 0, 0 }, { 7, 31, 0 } } },
	{ { { 19, 0, 1 }, { 7, 31, 1 } } },
	{ { { 19, 0, 2 }, { 7, 31, 2 } } },
	{ { { 19, 0, 3 }, { 8, 31, 1 } } },
	{ { { 19, 0, 4 }, { 8, 31, 0 } } },
	{ { { 20, 0, 4 }, { 8, 31, 1 } } },
	{ { { 20, 0, 3 }, { 8, 31, 2 } } },
	{ { { 20, 0, 2 }, { 9, 31, 1 } } },
	{ { { 20, 0, 1 }, { 9, 31, 0 } } },
	{ { { 20, 0, 0 }, { 12, 28, 0 } } },
	{ { { 20, 0, 1 }, { 12, 28, 1 } } },
	{ { { 20, 0, 2 }, { 10, 31, 1 } } },
	{ { { 20, 0, 3 }, { 10, 31, 0 } } },
	{ { { 20, 0, 4 }, { 12, 29, 0 } } },
	{ { { 21, 0, 3 }, { 12, 29, 1 } } },
	{ { { 21, 0, 2 }, { 11, 31, 1 } } },
	{ { { 21, 0, 1 }, { 11, 31, 0 } } },
	{ { { 21, 0, 0 }, { 12, 30, 0 } } },
	{ { { 21, 0, 1 }, { 12, 30, 1 } } },
	{ { { 21, 0, 2 }, { 12, 30, 2 } } },
	{ { { 21, 0, 3 }, { 12, 31, 1 } } },
	{ { { 21, 0, 4 }, { 12, 31, 0 } } },
	{ { { 22, 0, 3 }, { 12, 31, 1 } } },
	{ { { 22, 0, 2 }, { 12, 31, 2 } } },
	{ { { 22, 0, 1 }, { 13, 31, 1 } } },
	{ { { 22, 0, 0 }, { 13, 31, 0 } } },
	{ { { 22, 0, 1 }, { 13, 31, 1 } } },
	{ { { 22, 0, 2 }, { 13, 31, 2 } } },
	{ { { 22, 0, 3 }, { 14, 31, 1 } } },
	{ { { 22, 0, 4 }, { 14, 31, 0 } } },
	{ { { 23, 0, 3 }, { 14, 31, 1 } } },
	{ { { 23, 0, 2 }, { 14, 31, 2 } } },
	{ { { 23, 0, 1 }, { 15, 31, 1 } } },
	{ { { 23, 0, 0 }, { 15, 31, 0 } } },
	{ { { 23, 0, 1 }, { 15, 31, 1 } } },
	{ { { 23, 0, 2 }, { 15, 31, 2 } } },
	{ { { 23, 0, 3 }, { 16, 31, 1 } } },
	{ { { 23, 0, 4 }, { 16, 31, 0 } } },
	{ { { 24, 0, 4 }, { 16, 31, 1 } } },
	{ { { 24, 0, 3 }, { 16, 31, 2 } } },
	{ { { 24, 0, 2 }, { 17, 31, 1 } } },
	{ { { 24, 0, 1 }, { 17, 31, 0 } } },
	{ { { 24, 0, 0 }, { 20, 28, 0 } } },
	{ { { 24, 0, 1 }, { 20, 28, 1 } } },
	{ { { 24, 0, 2 }, { 18, 31, 1 } } },
	{ { { 24, 0, 3 }, { 18, 31, 0 } } },
	{ { { 24, 0, 4 }, { 20, 29, 0 } } },
	{ { { 25, 0, 3 }, { 20, 29, 1 } } },
	{ { { 25, 0, 2 }, { 19, 31, 1 } } },
	{ { { 25, 0, 1 }, { 19, 31, 0 } } },
	{ { { 25, 0, 0 }, { 20, 30, 0 } } },
	{ { { 25, 0, 1 }, { 20, 30, 1 } } },
	{ { { 25, 0, 2 }, { 20, 30, 2 } } },
	{ { { 25, 0, 3 }, { 20, 31, 1 } } },
	{ { { 25, 0, 4 }, { 20, 31, 0 } } },
	{ { { 26, 0, 3 }, { 20, 31, 1 } } },
	{ { { 26, 0, 2 }, { 20, 31, 2 } } },
	{ { { 26, 0, 1 }, { 21, 31, 1 } } },
	{ { { 26, 0, 0 }, { 21, 31, 0 } } },
	{ { { 26, 0, 1 }, { 21, 31, 1 } } },
	{ { { 26, 0, 2 }, { 21, 31, 2 } } },
	{ { { 26, 0, 3 }, { 22, 31, 1 } } },
	{ { { 26, 0, 4 }, { 22, 31, 0 } } },
	{ { { 27, 0, 3 }, { 22, 31, 1 } } },
	{ { { 27, 0, 2 }, { 22, 31, 2 } } },
	{ { { 27, 0, 1 }, { 23, 31, 1 } } },
	{ { { 27, 0, 0 }, { 23, 31, 0 } } },
	{ { { 27, 0, 1 }, { 23, 31, 1 } } },
	{ { { 27, 0, 2 }, { 23, 31, 2 } } },
	{ { { 27, 0, 3 }, { 24, 31, 1 } } },
	{ { { 27, 0, 4 }, { 24, 31, 0 } } },
	{ { { 28, 0, 4 }, { 24, 31, 1 } } },
	{ { { 28, 0, 3 }, { 24, 31, 2 } } },
	{ { { 28, 0, 2 }, { 25, 31, 1 } } },
	{ { { 28, 0, 1 }, { 25, 31, 0 } } },
	{ { { 28, 0, 0 }, { 28, 28, 0 } } },
	{ { { 28, 0, 1 }, { 28, 28, 1 } } },
	{ { { 28, 0, 2 }, { 26, 31, 1 } } },
	{ { { 28, 0, 3 }, { 26, 31, 0 } } },
	{ { { 28, 0, 4 }, { 28, 29, 0 } } },
	{ { { 29, 0, 3 }, { 28, 29, 1 } } },
	{ { { 29, 0, 2 }, { 27, 31, 1 } } },
	{ { { 29, 0, 1 }, { 27, 31, 0 } } },
	{ { { 29, 0, 0 }, { 28, 30, 0 } } },
	{ { { 29, 0, 1 }, { 28, 30, 1 } } },
	{ { { 29, 0, 2 }, { 28, 30, 2 } } },
	{ { { 29, 0, 3 }, { 28, 31, 1 } } },
	{ { { 29, 0, 4 }, { 28, 31, 0 } } },
	{ { { 30, 0, 3 }, { 28, 31, 1 } } },
	{ { { 30, 0, 2 }, { 28, 31, 2 } } },
	{ { { 30, 0, 1 }, { 29, 31, 1 } } },
	{ { { 30, 0, 0 }, { 29, 31, 0 } } },
	{ { { 30, 0, 1 }, { 29, 31, 1 } } },
	{ { { 30, 0, 2 }, { 29, 31, 2 } } },
	{ { { 30, 0, 3 }, { 30, 31, 1 } } },
	{ { { 30, 0, 4 }, { 30, 31, 0 } } },
	{ { { 31, 0, 3 }, { 30, 31, 1 } } },
	{ { { 31, 0, 2 }, { 30, 31, 2 } } },
	{ { { 31, 0, 1 }, { 31, 31, 1 } } },
	{ { { 31, 0, 0 }, { 31, 31, 0 } } }
};

static SingleColourLookup const lookup_6_3[] = 
{
	{ { { 0, 0, 0 }, { 0, 0, 0 } } },
	{ { { 0, 0, 1 }, { 0, 1, 1 } } },
	{ { { 0, 0, 2 }, { 0, 1, 0 } } },
	{ { { 1, 0, 1 }, { 0, 2, 1 } } },
	{ { { 1, 0, 0 }, { 0, 2, 0 } } },
	{ { { 1, 0, 1 }, { 0, 3, 1 } } },
	{ { { 1, 0, 2 }, { 0, 3, 0 } } },
	{ { { 2, 0, 1 }, { 0, 4, 1 } } },
	{ { { 2, 0, 0 }, { 0, 4, 0 } } },
	{ { { 2, 0, 1 }, { 0, 5, 1 } } },
	{ { { 2, 0, 2 }, { 0, 5, 0 } } },
	{ { { 3, 0, 1 }, { 0, 6, 1 } } },
	{ { { 3, 0, 0 }, { 0, 6, 0 } } },
	{ { { 3, 0, 1 }, { 0, 7, 1 } } },
	{ { { 3, 0, 2 }, { 0, 7, 0 } } },
	{ { { 4, 0, 1 }, { 0, 8, 1 } } },
	{ { { 4, 0, 0 }, { 0, 8, 0 } } },
	{ { { 4, 0, 1 }, { 0, 9, 1 } } },
	{ { { 4, 0, 2 }, { 0, 9, 0 } } },
	{ { { 5, 0, 1 }, { 0, 10, 1 } } },
	{ { { 5, 0, 0 }, { 0, 10, 0 } } },
	{ { { 5, 0, 1 }, { 0, 11, 1 } } },
	{ { { 5, 0, 2 }, { 0, 11, 0 } } },
	{ { { 6, 0, 1 }, { 0, 12, 1 } } },
	{ { { 6, 0, 0 }, { 0, 12, 0 } } },
	{ { { 6, 0, 1 }, { 0, 13, 1 } } },
	{ { { 6, 0, 2 }, { 0, 13, 0 } } },
	{ { { 7, 0, 1 }, { 0, 14, 1 } } },
	{ { { 7, 0, 0 }, { 0, 14, 0 } } },
	{ { { 7, 0, 1 }, { 0, 15, 1 } } },
	{ { { 7, 0, 2 }, { 0, 15, 0 } } },
	{ { { 8, 0, 1 }, { 0, 16, 1 } } },
	{ { { 8, 0, 0 }, { 0, 16, 0 } } },
	{ { { 8, 0, 1 }, { 0, 17, 1 } } },
	{ { { 8, 0, 2 }, { 0, 17, 0 } } },
	{ { { 9, 0, 1 }, { 0, 18, 1 } } },
	{ { { 9, 0, 0 }, { 0, 18, 0 } } },
	{ { { 9, 0, 1 }, { 0, 19, 1 } } },
	{ { { 9, 0, 2 }, { 0, 19, 0 } } },
	{ { { 10, 0, 1 }, { 0, 20, 1 } } },
	{ { { 10, 0, 0 }, { 0, 20, 0 } } },
	{ { { 10, 0, 1 }, { 0, 21, 1 } } },
	{ { { 10, 0, 2 }, { 0, 21, 0 } } },
	{ { { 11, 0, 1 }, { 0, 22, 1 } } },
	{ { { 11, 0, 0 }, { 0, 22, 0 } } },
	{ { { 11, 0, 1 }, { 0, 23, 1 } } },
	{ { { 11, 0, 2 }, { 0, 23, 0 } } },
	{ { { 12, 0, 1 }, { 0, 24, 1 } } },
	{ { { 12, 0, 0 }, { 0, 24, 0 } } },
	{ { { 12, 0, 1 }, { 0, 25, 1 } } },
	{ { { 12, 0, 2 }, { 0, 25, 0 } } },
	{ { { 13, 0, 1 }, { 0, 26, 1 } } },
	{ { { 13, 0, 0 }, { 0, 26, 0 } } },
	{ { { 13, 0, 1 }, { 0, 27, 1 } } },
	{ { { 13, 0, 2 }, { 0, 27, 0 } } },
	{ { { 14, 0, 1 }, { 0, 28, 1 } } },
	{ { { 14, 0, 0 }, { 0, 28, 0 } } },
	{ { { 14, 0, 1 }, { 0, 29, 1 } } },
	{ { { 14, 0, 2 }, { 0, 29, 0 } } },
	{ { { 15, 0, 1 }, { 0, 30, 1 } } },
	{ { { 15, 0, 0 }, { 0, 30, 0 } } },
	{ { { 15, 0, 1 }, { 0, 31, 1 } } },
	{ { { 15, 0, 2 }, { 0, 31, 0 } } },
	{ { { 16, 0, 2 }, { 1, 31, 1 } } },
	{ { { 16, 0, 1 }, { 1, 31, 0 } } },
	{ { { 16, 0, 0 }, { 0, 32, 0 } } },
	{ { { 16, 0, 1 }, { 2, 31, 0 } } },
	{ { { 16, 0, 2 }, { 0, 33, 0 } } },
	{ { { 17, 0, 1 }, { 3, 31, 0 } } },
	{ { { 17, 0, 0 }, { 0, 34, 0 } } },
	{ { { 17, 0, 1 }, { 4, 31, 0 } } },
	{ { { 17, 0, 2 }, { 0, 35, 0 } } },
	{ { { 18, 0, 1 }, { 5, 31, 0 } } },
	{ { { 18, 0, 0 }, { 0, 36, 0 } } },
	{ { { 18, 0, 1 }, { 6, 31, 0 } } },
	{ { { 18, 0, 2 }, { 0, 37, 0 } } },
	{ { { 19, 0, 1 }, { 7, 31, 0 } } },
	{ { { 19, 0, 0 }, { 0, 38, 0 } } },
	{ { { 19, 0, 1 }, { 8, 31, 0 } } },
	{ { { 19, 0, 2 }, { 0, 39, 0 } } },
	{ { { 20, 0, 1 }, { 9, 31, 0 } } },
	{ { { 20, 0, 0 }, { 0, 40, 0 } } },
	{ { { 20, 0, 1 }, { 10, 31, 0 } } },
	{ { { 20, 0, 2 }, { 0, 41, 0 } } },
	{ { { 21, 0, 1 }, { 11, 31, 0 } } },
	{ { { 21, 0, 0 }, { 0, 42, 0 } } },
	{ { { 21, 0, 1 }, { 12, 31, 0 } } },
	{ { { 21, 0, 2 }, { 0, 43, 0 } } },
	{ { { 22, 0, 1 }, { 13, 31, 0 } } },
	{ { { 22, 0, 0 }, { 0, 44, 0 } } },
	{ { { 22, 0, 1 }, { 14, 31, 0 } } },
	{ { { 22, 0, 2 }, { 0, 45, 0 } } },
	{ { { 23, 0, 1 }, { 15, 31, 0 } } },
	{ { { 23, 0, 0 }, { 0, 46, 0 } } },
	{ { { 23, 0, 1 }, { 0, 47, 1 } } },
	{ { { 23, 0, 2 }, { 0, 47, 0 } } },
	{ { { 24, 0, 1 }, { 0, 48, 1 } } },
	{ { { 24, 0, 0 }, { 0, 48, 0 } } },
	{ { { 24, 0, 1 }, { 0, 49, 1 } } },
	{ { { 24, 0, 2 }, { 0, 49, 0 } } },
	{ { { 25, 0, 1 }, { 0, 50, 1 } } },
	{ { { 25, 0, 0 }, { 0, 50, 0 } } },
	{ { { 25, 0, 1 }, { 0, 51, 1 } } },
	{ { { 25, 0, 2 }, { 0, 51, 0 } } },
	{ { { 26, 0, 1 }, { 0, 52, 1 } } },
	{ { { 26, 0, 0 }, { 0, 52, 0 } } },
	{ { { 26, 0, 1 }, { 0, 53, 1 } } },
	{ { { 26, 0, 2 }, { 0, 53, 0 } } },
	{ { { 27, 0, 1 }, { 0, 54, 1 } } },
	{ { { 27, 0, 0 }, { 0, 54, 0 } } },
	{ { { 27, 0, 1 }, { 0, 55, 1 } } },
	{ { { 27, 0, 2 }, { 0, 55, 0 } } },
	{ { { 28, 0, 1 }, { 0, 56, 1 } } },
	{ { { 28, 0, 0 }, { 0, 56, 0 } } },
	{ { { 28, 0, 1 }, { 0, 57, 1 } } },
	{ { { 28, 0, 2 }, { 0, 57, 0 } } },
	{ { { 29, 0, 1 }, { 0, 58, 1 } } },
	{ { { 29, 0, 0 }, { 0, 58, 0 } } },
	{ { { 29, 0, 1 }, { 0, 59, 1 } } },
	{ { { 29, 0, 2 }, { 0, 59, 0 } } },
	{ { { 30, 0, 1 }, { 0, 60, 1 } } },
	{ { { 30, 0, 0 }, { 0, 60, 0 } } },
	{ { { 30, 0, 1 }, { 0, 61, 1 } } },
	{ { { 30, 0, 2 }, { 0, 61, 0 } } },
	{ { { 31, 0, 1 }, { 0, 62, 1 } } },
	{ { { 31, 0, 0 }, { 0, 62, 0 } } },
	{ { { 31, 0, 1 }, { 0, 63, 1 } } },
	{ { { 31, 0, 2 }, { 0, 63, 0 } } },
	{ { { 32, 0, 2 }, { 1, 63, 1 } } },
	{ { { 32, 0, 1 }, { 1, 63, 0 } } },
	{ { { 32, 0, 0 }, { 16, 48, 0 } } },
	{ { { 32, 0, 1 }, { 2, 63, 0 } } },
	{ { { 32, 0, 2 }, { 16, 49, 0 } } },
	{ { { 33, 0, 1 }, { 3, 63, 0 } } },
	{ { { 33, 0, 0 }, { 16, 50, 0 } } },
	{ { { 33, 0, 1 }, { 4, 63, 0 } } },
	{ { { 33, 0, 2 }, { 16, 51, 0 } } },
	{ { { 34, 0, 1 }, { 5, 63, 0 } } },
	{ { { 34, 0, 0 }, { 16, 52, 0 } } },
	{ { { 34, 0, 1 }, { 6, 63, 0 } } },
	{ { { 34, 0, 2 }, { 16, 53, 0 } } },
	{ { { 35, 0, 1 }, { 7, 63, 0 } } },
	{ { { 35, 0, 0 }, { 16, 54, 0 } } },
	{ { { 35, 0, 1 }, { 8, 63, 0 } } },
	{ { { 35, 0, 2 }, { 16, 55, 0 } } },
	{ { { 36, 0, 1 }, { 9, 63, 0 } } },
	{ { { 36, 0, 0 }, { 16, 56, 0 } } },
	{ { { 36, 0, 1 }, { 10, 63, 0 } } },
	{ { { 36, 0, 2 }, { 16, 57, 0 } } },
	{ { { 37, 0, 1 }, { 11, 63, 0 } } },
	{ { { 37, 0, 0 }, { 16, 58, 0 } } },
	{ { { 37, 0, 1 }, { 12, 63, 0 } } },
	{ { { 37, 0, 2 }, { 16, 59, 0 } } },
	{ { { 38, 0, 1 }, { 13, 63, 0 } } },
	{ { { 38, 0, 0 }, { 16, 60, 0 } } },
	{ { { 38, 0, 1 }, { 14, 63, 0 } } },
	{ { { 38, 0, 2 }, { 16, 61, 0 } } },
	{ { { 39, 0, 1 }, { 15, 63, 0 } } },
	{ { { 39, 0, 0 }, { 16, 62, 0 } } },
	{ { { 39, 0, 1 }, { 16, 63, 1 } } },
	{ { { 39, 0, 2 }, { 16, 63, 0 } } },
	{ { { 40, 0, 1 }, { 17, 63, 1 } } },
	{ { { 40, 0, 0 }, { 17, 63, 0 } } },
	{ { { 40, 0, 1 }, { 18, 63, 1 } } },
	{ { { 40, 0, 2 }, { 18, 63, 0 } } },
	{ { { 41, 0, 1 }, { 19, 63, 1 } } },
	{ { { 41, 0, 0 }, { 19, 63, 0 } } },
	{ { { 41, 0, 1 }, { 20, 63, 1 } } },
	{ { { 41, 0, 2 }, { 20, 63, 0 } } },
	{ { { 42, 0, 1 }, { 21, 63, 1 } } },
	{ { { 42, 0, 0 }, { 21, 63, 0 } } },
	{ { { 42, 0, 1 }, { 22, 63, 1 } } },
	{ { { 42, 0, 2 }, { 22, 63, 0 } } },
	{ { { 43, 0, 1 }, { 23, 63, 1 } } },
	{ { { 43, 0, 0 }, { 23, 63, 0 } } },
	{ { { 43, 0, 1 }, { 24, 63, 1 } } },
	{ { { 43, 0, 2 }, { 24, 63, 0 } } },
	{ { { 44, 0, 1 }, { 25, 63, 1 } } },
	{ { { 44, 0, 0 }, { 25, 63, 0 } } },
	{ { { 44, 0, 1 }, { 26, 63, 1 } } },
	{ { { 44, 0, 2 }, { 26, 63, 0 } } },
	{ { { 45, 0, 1 }, { 27, 63, 1 } } },
	{ { { 45, 0, 0 }, { 27, 63, 0 } } },
	{ { { 45, 0, 1 }, { 28, 63, 1 } } },
	{ { { 45, 0, 2 }, { 28, 63, 0 } } },
	{ { { 46, 0, 1 }, { 29, 63, 1 } } },
	{ { { 46, 0, 0 }, { 29, 63, 0 } } },
	{ { { 46, 0, 1 }, { 30, 63, 1 } } },
	{ { { 46, 0, 2 }, { 30, 63, 0 } } },
	{ { { 47, 0, 1 }, { 31, 63, 1 } } },
	{ { { 47, 0, 0 }, { 31, 63, 0 } } },
	{ { { 47, 0, 1 }, { 32, 63, 1 } } },
	{ { { 47, 0, 2 }, { 32, 63, 0 } } },
	{ { { 48, 0, 2 }, { 33, 63, 1 } } },
	{ { { 48, 0, 1 }, { 33, 63, 0 } } },
	{ { { 48, 0, 0 }, { 48, 48, 0 } } },
	{ { { 48, 0, 1 }, { 34, 63, 0 } } },
	{ { { 48, 0, 2 }, { 48, 49, 0 } } },
	{ { { 49, 0, 1 }, { 35, 63, 0 } } },
	{ { { 49, 0, 0 }, { 48, 50, 0 } } },
	{ { { 49, 0, 1 }, { 36, 63, 0 } } },
	{ { { 49, 0, 2 }, { 48, 51, 0 } } },
	{ { { 50, 0, 1 }, { 37, 63, 0 } } },
	{ { { 50, 0, 0 }, { 48, 52, 0 } } },
	{ { { 50, 0, 1 }, { 38, 63, 0 } } },
	{ { { 50, 0, 2 }, { 48, 53, 0 } } },
	{ { { 51, 0, 1 }, { 39, 63, 0 } } },
	{ { { 51, 0, 0 }, { 48, 54, 0 } } },
	{ { { 51, 0, 1 }, { 40, 63, 0 } } },
	{ { { 51, 0, 2 }, { 48, 55, 0 } } },
	{ { { 52, 0, 1 }, { 41, 63, 0 } } },
	{ { { 52, 0, 0 }, { 48, 56, 0 } } },
	{ { { 52, 0, 1 }, { 42, 63, 0 } } },
	{ { { 52, 0, 2 }, { 48, 57, 0 } } },
	{ { { 53, 0, 1 }, { 43, 63, 0 } } },
	{ { { 53, 0, 0 }, { 48, 58, 0 } } },
	{ { { 53, 0, 1 }, { 44, 63, 0 } } },
	{ { { 53, 0, 2 }, { 48, 59, 0 } } },
	{ { { 54, 0, 1 }, { 45, 63, 0 } } },
	{ { { 54, 0, 0 }, { 48, 60, 0 } } },
	{ { { 54, 0, 1 }, { 46, 63, 0 } } },
	{ { { 54, 0, 2 }, { 48, 61, 0 } } },
	{ { { 55, 0, 1 }, { 47, 63, 0 } } },
	{ { { 55, 0, 0 }, { 48, 62, 0 } } },
	{ { { 55, 0, 1 }, { 48, 63, 1 } } },
	{ { { 55, 0, 2 }, { 48, 63, 0 } } },
	{ { { 56, 0, 1 }, { 49, 63, 1 } } },
	{ { { 56, 0, 0 }, { 49, 63, 0 } } },
	{ { { 56, 0, 1 }, { 50, 63, 1 } } },
	{ { { 56, 0, 2 }, { 50, 63, 0 } } },
	{ { { 57, 0, 1 }, { 51, 63, 1 } } },
	{ { { 57, 0, 0 }, { 51, 63, 0 } } },
	{ { { 57, 0, 1 }, { 52, 63, 1 } } },
	{ { { 57, 0, 2 }, { 52, 63, 0 } } },
	{ { { 58, 0, 1 }, { 53, 63, 1 } } },
	{ { { 58, 0, 0 }, { 53, 63, 0 } } },
	{ { { 58, 0, 1 }, { 54, 63, 1 } } },
	{ { { 58, 0, 2 }, { 54, 63, 0 } } },
	{ { { 59, 0, 1 }, { 55, 63, 1 } } },
	{ { { 59, 0, 0 }, { 55, 63, 0 } } },
	{ { { 59, 0, 1 }, { 56, 63, 1 } } },
	{ { { 59, 0, 2 }, { 56, 63, 0 } } },
	{ { { 60, 0, 1 }, { 57, 63, 1 } } },
	{ { { 60, 0, 0 }, { 57, 63, 0 } } },
	{ { { 60, 0, 1 }, { 58, 63, 1 } } },
	{ { { 60, 0, 2 }, { 58, 63, 0 } } },
	{ { { 61, 0, 1 }, { 59, 63, 1 } } },
	{ { { 61, 0, 0 }, { 59, 63, 0 } } },
	{ { { 61, 0, 1 }, { 60, 63, 1 } } },
	{ { { 61, 0, 2 }, { 60, 63, 0 } } },
	{ { { 62, 0, 1 }, { 61, 63, 1 } } },
	{ { { 62, 0, 0 }, { 61, 63, 0 } } },
	{ { { 62, 0, 1 }, { 62, 63, 1 } } },
	{ { { 62, 0, 2 }, { 62, 63, 0 } } },
	{ { { 63, 0, 1 }, { 63, 63, 1 } } },
	{ { { 63, 0, 0 }, { 63, 63, 0 } } }
};

static SingleColourLookup const lookup_5_4[] = 
{
	{ { { 0, 0, 0 }, { 0, 0, 0 } } },
	{ { { 0, 0, 1 }, { 0, 1, 1 } } },
	{ { { 0, 0, 2 }, { 0, 1, 0 } } },
	{ { { 0, 0, 3 }, { 0, 1, 1 } } },
	{ { { 0, 0, 4 }, { 0, 2, 1 } } },
	{ { { 1, 0, 3 }, { 0, 2, 0 } } },
	{ { { 1, 0, 2 }, { 0, 2, 1 } } },
	{ { { 1, 0, 1 }, { 0, 3, 1 } } },
	{ { { 1, 0, 0 }, { 0, 3, 0 } } },
	{ { { 1, 0, 1 }, { 1, 2, 1 } } },
	{ { { 1, 0, 2 }, { 1, 2, 0 } } },
	{ { { 1, 0, 3 }, { 0, 4, 0 } } },
	{ { { 1, 0, 4 }, { 0, 5, 1 } } },
	{ { { 2, 0, 3 }, { 0, 5, 0 } } },
	{ { { 2, 0, 2 }, { 0, 5, 1 } } },
	{ { { 2, 0, 1 }, { 0, 6, 1 } } },
	{ { { 2, 0, 0 }, { 0, 6, 0 } } },
	{ { { 2, 0, 1 }, { 2, 3, 1 } } },
	{ { { 2, 0, 2 }, { 2, 3, 0 } } },
	{ { { 2, 0, 3 }, { 0, 7, 0 } } },
	{ { { 2, 0, 4 }, { 1, 6, 1 } } },
	{ { { 3, 0, 3 }, { 1, 6, 0 } } },
	{ { { 3, 0, 2 }, { 0, 8, 0 } } },
	{ { { 3, 0, 1 }, { 0, 9, 1 } } },
	{ { { 3, 0, 0 }, { 0, 9, 0 } } },
	{ { { 3, 0, 1 }, { 0, 9, 1 } } },
	{ { { 3, 0, 2 }, { 0, 10, 1 } } },
	{ { { 3, 0, 3 }, { 0, 10, 0 } } },
	{ { { 3, 0, 4 }, { 2, 7, 1 } } },
	{ { { 4, 0, 4 }, { 2, 7, 0 } } },
	{ { { 4, 0, 3 }, { 0, 11, 0 } } },
	{ { { 4, 0, 2 }, { 1, 10, 1 } } },
	{ { { 4, 0, 1 }, { 1, 10, 0 } } },
	{ { { 4, 0, 0 }, { 0, 12, 0 } } },
	{ { { 4, 0, 1 }, { 0, 13, 1 } } },
	{ { { 4, 0, 2 }, { 0, 13, 0 } } },
	{ { { 4, 0, 3 }, { 0, 13, 1 } } },
	{ { { 4, 0, 4 }, { 0, 14, 1 } } },
	{ { { 5, 0, 3 }, { 0, 14, 0 } } },
	{ { { 5, 0, 2 }, { 2, 11, 1 } } },
	{ { { 5, 0, 1 }, { 2, 11, 0 } } },
	{ { { 5, 0, 0 }, { 0, 15, 0 } } },
	{ { { 5, 0, 1 }, { 1, 14, 1 } } },
	{ { { 5, 0, 2 }, { 1, 14, 0 } } },
	{ { { 5, 0, 3 }, { 0, 16, 0 } } },
	{ { { 5, 0, 4 }, { 0, 17, 1 } } },
	{ { { 6, 0, 3 }, { 0, 17, 0 } } },
	{ { { 6, 0, 2 }, { 0, 17, 1 } } },
	{ { { 6, 0, 1 }, { 0, 18, 1 } } },
	{ { { 6, 0, 0 }, { 0, 18, 0 } } },
	{ { { 6, 0, 1 }, { 2, 15, 1 } } },
	{ { { 6, 0, 2 }, { 2, 15, 0 } } },
	{ { { 6, 0, 3 }, { 0, 19, 0 } } },
	{ { { 6, 0, 4 }, { 1, 18, 1 } } },
	{ { { 7, 0, 3 }, { 1, 18, 0 } } },
	{ { { 7, 0, 2 }, { 0, 20, 0 } } },
	{ { { 7, 0, 1 }, { 0, 21, 1 } } },
	{ { { 7, 0, 0 }, { 0, 21, 0 } } },
	{ { { 7, 0, 1 }, { 0, 21, 1 } } },
	{ { { 7, 0, 2 }, { 0, 22, 1 } } },
	{ { { 7, 0, 3 }, { 0, 22, 0 } } },
	{ { { 7, 0, 4 }, { 2, 19, 1 } } },
	{ { { 8, 0, 4 }, { 2, 19, 0 } } },
	{ { { 8, 0, 3 }, { 0, 23, 0 } } },
	{ { { 8, 0, 2 }, { 1, 22, 1 } } },
	{ { { 8, 0, 1 }, { 1, 22, 0 } } },
	{ { { 8, 0, 0 }, { 0, 24, 0 } } },
	{ { { 8, 0, 1 }, { 0, 25, 1 } } },
	{ { { 8, 0, 2 }, { 0, 25, 0 } } },
	{ { { 8, 0, 3 }, { 0, 25, 1 } } },
	{ { { 8, 0, 4 }, { 0, 26, 1 } } },
	{ { { 9, 0, 3 }, { 0, 26, 0 } } },
	{ { { 9, 0, 2 }, { 2, 23, 1 } } },
	{ { { 9, 0, 1 }, { 2, 23, 0 } } },
	{ { { 9, 0, 0 }, { 0, 27, 0 } } },
	{ { { 9, 0, 1 }, { 1, 26, 1 } } },
	{ { { 9, 0, 2 }, { 1, 26, 0 } } },
	{ { { 9, 0, 3 }, { 0, 28, 0 } } },
	{ { { 9, 0, 4 }, { 0, 29, 1 } } },
	{ { { 10, 0, 3 }, { 0, 29, 0 } } },
	{ { { 10, 0, 2 }, { 0, 29, 1 } } },
	{ { { 10, 0, 1 }, { 0, 30, 1 } } },
	{ { { 10, 0, 0 }, { 0, 30, 0 } } },
	{ { { 10, 0, 1 }, { 2, 27, 1 } } },
	{ { { 10, 0, 2 }, { 2, 27, 0 } } },
	{ { { 10, 0, 3 }, { 0, 31, 0 } } },
	{ { { 10, 0, 4 }, { 1, 30, 1 } } },
	{ { { 11, 0, 3 }, { 1, 30, 0 } } },
	{ { { 11, 0, 2 }, { 4, 24, 0 } } },
	{ { { 11, 0, 1 }, { 1, 31, 1 } } },
	{ { { 11, 0, 0 }, { 1, 31, 0 } } },
	{ { { 11, 0, 1 }, { 1, 31, 1 } } },
	{ { { 11, 0, 2 }, { 2, 30, 1 } } },
	{ { { 11, 0, 3 }, { 2, 30, 0 } } },
	{ { { 11, 0, 4 }, { 2, 31, 1 } } },
	{ { { 12, 0, 4 }, { 2, 31, 0 } } },
	{ { { 12, 0, 3 }, { 4, 27, 0 } } },
	{ { { 12, 0, 2 }, { 3, 30, 1 } } },
	{ { { 12, 0, 1 }, { 3, 30, 0 } } },
	{ { { 12, 0, 0 }, { 4, 28, 0 } } },
	{ { { 12, 0, 1 }, { 3, 31, 1 } } },
	{ { { 12, 0, 2 }, { 3, 31, 0 } } },
	{ { { 12, 0, 3 }, { 3, 31, 1 } } },
	{ { { 12, 0, 4 }, { 4, 30, 1 } } },
	{ { { 13, 0, 3 }, { 4, 30, 0 } } },
	{ { { 13, 0, 2 }, { 6, 27, 1 } } },
	{ { { 13, 0, 1 }, { 6, 27, 0 } } },
	{ { { 13, 0, 0 }, { 4, 31, 0 } } },
	{ { { 13, 0, 1 }, { 5, 30, 1 } } },
	{ { { 13, 0, 2 }, { 5, 30, 0 } } },
	{ { { 13, 0, 3 }, { 8, 24, 0 } } },
	{ { { 13, 0, 4 }, { 5, 31, 1 } } },
	{ { { 14, 0, 3 }, { 5, 31, 0 } } },
	{ { { 14, 0, 2 }, { 5, 31, 1 } } },
	{ { { 14, 0, 1 }, { 6, 30, 1 } } },
	{ { { 14, 0, 0 }, { 6, 30, 0 } } },
	{ { { 14, 0, 1 }, { 6, 31, 1 } } },
	{ { { 14, 0, 2 }, { 6, 31, 0 } } },
	{ { { 14, 0, 3 }, { 8, 27, 0 } } },
	{ { { 14, 0, 4 }, { 7, 30, 1 } } },
	{ { { 15, 0, 3 }, { 7, 30, 0 } } },
	{ { { 15, 0, 2 }, { 8, 28, 0 } } },
	{ { { 15, 0, 1 }, { 7, 31, 1 } } },
	{ { { 15, 0, 0 }, { 7, 31, 0 } } },
	{ { { 15, 0, 1 }, { 7, 31, 1 } } },
	{ { { 15, 0, 2 }, { 8, 30, 1 } } },
	{ { { 15, 0, 3 }, { 8, 30, 0 } } },
	{ { { 15, 0, 4 }, { 10, 27, 1 } } },
	{ { { 16, 0, 4 }, { 10, 27, 0 } } },
	{ { { 16, 0, 3 }, { 8, 31, 0 } } },
	{ { { 16, 0, 2 }, { 9, 30, 1 } } },
	{ { { 16, 0, 1 }, { 9, 30, 0 } } },
	{ { { 16, 0, 0 }, { 12, 24, 0 } } },
	{ { { 16, 0, 1 }, { 9, 31, 1 } } },
	{ { { 16, 0, 2 }, { 9, 31, 0 } } },
	{ { { 16, 0, 3 }, { 9, 31, 1 } } },
	{ { { 16, 0, 4 }, { 10, 30, 1 } } },
	{ { { 17, 0, 3 }, { 10, 30, 0 } } },
	{ { { 17, 0, 2 }, { 10, 31, 1 } } },
	{ { { 17, 0, 1 }, { 10, 31, 0 } } },
	{ { { 17, 0, 0 }, { 12, 27, 0 } } },
	{ { { 17, 0, 1 }, { 11, 30, 1 } } },
	{ { { 17, 0, 2 }, { 11, 30, 0 } } },
	{ { { 17, 0, 3 }, { 12, 28, 0 } } },
	{ { { 17, 0, 4 }, { 11, 31, 1 } } },
	{ { { 18, 0, 3 }, { 11, 31, 0 } } },
	{ { { 18, 0, 2 }, { 11, 31, 1 } } },
	{ { { 18, 0, 1 }, { 12, 30, 1 } } },
	{ { { 18, 0, 0 }, { 12, 30, 0 } } },
	{ { { 18, 0, 1 }, { 14, 27, 1 } } },
	{ { { 18, 0, 2 }, { 14, 27, 0 } } },
	{ { { 18, 0, 3 }, { 12, 31, 0 } } },
	{ { { 18, 0, 4 }, { 13, 30, 1 } } },
	{ { { 19, 0, 3 }, { 13, 30, 0 } } },
	{ { { 19, 0, 2 }, { 16, 24, 0 } } },
	{ { { 19, 0, 1 }, { 13, 31, 1 } } },
	{ { { 19, 0, 0 }, { 13, 31, 0 } } },
	{ { { 19, 0, 1 }, { 13, 31, 1 } } },
	{ { { 19, 0, 2 }, { 14, 30, 1 } } },
	{ { { 19, 0, 3 }, { 14, 30, 0 } } },
	{ { { 19, 0, 4 }, { 14, 31, 1 } } },
	{ { { 20, 0, 4 }, { 14, 31, 0 } } },
	{ { { 20, 0, 3 }, { 16, 27, 0 } } },
	{ { { 20, 0, 2 }, { 15, 30, 1 } } },
	{ { { 20, 0, 1 }, { 15, 30, 0 } } },
	{ { { 20, 0, 0 }, { 16, 28, 0 } } },
	{ { { 20, 0, 1 }, { 15, 31, 1 } } },
	{ { { 20, 0, 2 }, { 15, 31, 0 } } },
	{ { { 20, 0, 3 }, { 15, 31, 1 } } },
	{ { { 20, 0, 4 }, { 16, 30, 1 } } },
	{ { { 21, 0, 3 }, { 16, 30, 0 } } },
	{ { { 21, 0, 2 }, { 18, 27, 1 } } },
	{ { { 21, 0, 1 }, { 18, 27, 0 } } },
	{ { { 21, 0, 0 }, { 16, 31, 0 } } },
	{ { { 21, 0, 1 }, { 17, 30, 1 } } },
	{ { { 21, 0, 2 }, { 17, 30, 0 } } },
	{ { { 21, 0, 3 }, { 20, 24, 0 } } },
	{ { { 21, 0, 4 }, { 17, 31, 1 } } },
	{ { { 22, 0, 3 }, { 17, 31, 0 } } },
	{ { { 22, 0, 2 }, { 17, 31, 1 } } },
	{ { { 22, 0, 1 }, { 18, 30, 1 } } },
	{ { { 22, 0, 0 }, { 18, 30, 0 } } },
	{ { { 22, 0, 1 }, { 18, 31, 1 } } },
	{ { { 22, 0, 2 }, { 18, 31, 0 } } },
	{ { { 22, 0, 3 }, { 20, 27, 0 } } },
	{ { { 22, 0, 4 }, { 19, 30, 1 } } },
	{ { { 23, 0, 3 }, { 19, 30, 0 } } },
	{ { { 23, 0, 2 }, { 20, 28, 0 } } },
	{ { { 23, 0, 1 }, { 19, 31, 1 } } },
	{ { { 23, 0, 0 }, { 19, 31, 0 } } },
	{ { { 23, 0, 1 }, { 19, 31, 1 } } },
	{ { { 23, 0, 2 }, { 20, 30, 1 } } },
	{ { { 23, 0, 3 }, { 20, 30, 0 } } },
	{ { { 23, 0, 4 }, { 22, 27, 1 } } },
	{ { { 24, 0, 4 }, { 22, 27, 0 } } },
	{ { { 24, 0, 3 }, { 20, 31, 0 } } },
	{ { { 24, 0, 2 }, { 21, 30, 1 } } },
	{ { { 24, 0, 1 }, { 21, 30, 0 } } },
	{ { { 24, 0, 0 }, { 24, 24, 0 } } },
	{ { { 24, 0, 1 }, { 21, 31, 1 } } },
	{ { { 24, 0, 2 }, { 21, 31, 0 } } },
	{ { { 24, 0, 3 }, { 21, 31, 1 } } },
	{ { { 24, 0, 4 }, { 22, 30, 1 } } },
	{ { { 25, 0, 3 }, { 22, 30, 0 } } },
	{ { { 25, 0, 2 }, { 22, 31, 1 } } },
	{ { { 25, 0, 1 }, { 22, 31, 0 } } },
	{ { { 25, 0, 0 }, { 24, 27, 0 } } },
	{ { { 25, 0, 1 }, { 23, 30, 1 } } },
	{ { { 25, 0, 2 }, { 23, 30, 0 } } },
	{ { { 25, 0, 3 }, { 24, 28, 0 } } },
	{ { { 25, 0, 4 }, { 23, 31, 1 } } },
	{ { { 26, 0, 3 }, { 23, 31, 0 } } },
	{ { { 26, 0, 2 }, { 23, 31, 1 } } },
	{ { { 26, 0, 1 }, { 24, 30, 1 } } },
	{ { { 26, 0, 0 }, { 24, 30, 0 } } },
	{ { { 26, 0, 1 }, { 26, 27, 1 } } },
	{ { { 26, 0, 2 }, { 26, 27, 0 } } },
	{ { { 26, 0, 3 }, { 24, 31, 0 } } },
	{ { { 26, 0, 4 }, { 25, 30, 1 } } },
	{ { { 27, 0, 3 }, { 25, 30, 0 } } },
	{ { { 27, 0, 2 }, { 28, 24, 0 } } },
	{ { { 27, 0, 1 }, { 25, 31, 1 } } },
	{ { { 27, 0, 0 }, { 25, 31, 0 } } },
	{ { { 27, 0, 1 }, { 25, 31, 1 } } },
	{ { { 27, 0, 2 }, { 26, 30, 1 } } },
	{ { { 27, 0, 3 }, { 26, 30, 0 } } },
	{ { { 27, 0, 4 }, { 26, 31, 1 } } },
	{ { { 28, 0, 4 }, { 26, 31, 0 } } },
	{ { { 28, 0, 3 }, { 28, 27, 0 } } },
	{ { { 28, 0, 2 }, { 27, 30, 1 } } },
	{ { { 28, 0, 1 }, { 27, 30, 0 } } },
	{ { { 28, 0, 0 }, { 28, 28, 0 } } },
	{ { { 28, 0, 1 }, { 27, 31, 1 } } },
	{ { { 28, 0, 2 }, { 27, 31, 0 } } },
	{ { { 28, 0, 3 }, { 27, 31, 1 } } },
	{ { { 28, 0, 4 }, { 28, 30, 1 } } },
	{ { { 29, 0, 3 }, { 28, 30, 0 } } },
	{ { { 29, 0, 2 }, { 30, 27, 1 } } },
	{ { { 29, 0, 1 }, { 30, 27, 0 } } },
	{ { { 29, 0, 0 }, { 28, 31, 0 } } },
	{ { { 29, 0, 1 }, { 29, 30, 1 } } },
	{ { { 29, 0, 2 }, { 29, 30, 0 } } },
	{ { { 29, 0, 3 }, { 29, 30, 1 } } },
	{ { { 29, 0, 4 }, { 29, 31, 1 } } },
	{ { { 30, 0, 3 }, { 29, 31, 0 } } },
	{ { { 30, 0, 2 }, { 29, 31, 1 } } },
	{ { { 30, 0, 1 }, { 30, 30, 1 } } },
	{ { { 30, 0, 0 }, { 30, 30, 0 } } },
	{ { { 30, 0, 1 }, { 30, 31, 1 } } },
	{ { { 30, 0, 2 }, { 30, 31, 0 } } },
	{ { { 30, 0, 3 }, { 30, 31, 1 } } },
	{ { { 30, 0, 4 }, { 31, 30, 1 } } },
	{ { { 31, 0, 3 }, { 31, 30, 0 } } },
	{ { { 31, 0, 2 }, { 31, 30, 1 } } },
	{ { { 31, 0, 1 }, { 31, 31, 1 } } },
	{ { { 31, 0, 0 }, { 31, 31, 0 } } }
};

static SingleColourLookup const lookup_6_4[] = 
{
	{ { { 0, 0, 0 }, { 0, 0, 0 } } },
	{ { { 0, 0, 1 }, { 0, 1, 0 } } },
	{ { { 0, 0, 2 }, { 0, 2, 0 } } },
	{ { { 1, 0, 1 }, { 0, 3, 1 } } },
	{ { { 1, 0, 0 }, { 0, 3, 0 } } },
	{ { { 1, 0, 1 }, { 0, 4, 0 } } },
	{ { { 1, 0, 2 }, { 0, 5, 0 } } },
	{ { { 2, 0, 1 }, { 0, 6, 1 } } },
	{ { { 2, 0, 0 }, { 0, 6, 0 } } },
	{ { { 2, 0, 1 }, { 0, 7, 0 } } },
	{ { { 2, 0, 2 }, { 0, 8, 0 } } },
	{ { { 3, 0, 1 }, { 0, 9, 1 } } },
	{ { { 3, 0, 0 }, { 0, 9, 0 } } },
	{ { { 3, 0, 1 }, { 0, 10, 0 } } },
	{ { { 3, 0, 2 }, { 0, 11, 0 } } },
	{ { { 4, 0, 1 }, { 0, 12, 1 } } },
	{ { { 4, 0, 0 }, { 0, 12, 0 } } },
	{ { { 4, 0, 1 }, { 0, 13, 0 } } },
	{ { { 4, 0, 2 }, { 0, 14, 0 } } },
	{ { { 5, 0, 1 }, { 0, 15, 1 } } },
	{ { { 5, 0, 0 }, { 0, 15, 0 } } },
	{ { { 5, 0, 1 }, { 0, 16, 0 } } },
	{ { { 5, 0, 2 }, { 1, 15, 0 } } },
	{ { { 6, 0, 1 }, { 0, 17, 0 } } },
	{ { { 6, 0, 0 }, { 0, 18, 0 } } },
	{ { { 6, 0, 1 }, { 0, 19, 0 } } },
	{ { { 6, 0, 2 }, { 3, 14, 0 } } },
	{ { { 7, 0, 1 }, { 0, 20, 0 } } },
	{ { { 7, 0, 0 }, { 0, 21, 0 } } },
	{ { { 7, 0, 1 }, { 0, 22, 0 } } },
	{ { { 7, 0, 2 }, { 4, 15, 0 } } },
	{ { { 8, 0, 1 }, { 0, 23, 0 } } },
	{ { { 8, 0, 0 }, { 0, 24, 0 } } },
	{ { { 8, 0, 1 }, { 0, 25, 0 } } },
	{ { { 8, 0, 2 }, { 6, 14, 0 } } },
	{ { { 9, 0, 1 }, { 0, 26, 0 } } },
	{ { { 9, 0, 0 }, { 0, 27, 0 } } },
	{ { { 9, 0, 1 }, { 0, 28, 0 } } },
	{ { { 9, 0, 2 }, { 7, 15, 0 } } },
	{ { { 10, 0, 1 }, { 0, 29, 0 } } },
	{ { { 10, 0, 0 }, { 0, 30, 0 } } },
	{ { { 10, 0, 1 }, { 0, 31, 0 } } },
	{ { { 10, 0, 2 }, { 9, 14, 0 } } },
	{ { { 11, 0, 1 }, { 0, 32, 0 } } },
	{ { { 11, 0, 0 }, { 0, 33, 0 } } },
	{ { { 11, 0, 1 }, { 2, 30, 0 } } },
	{ { { 11, 0, 2 }, { 0, 34, 0 } } },
	{ { { 12, 0, 1 }, { 0, 35, 0 } } },
	{ { { 12, 0, 0 }, { 0, 36, 0 } } },
	{ { { 12, 0, 1 }, { 3, 31, 0 } } },
	{ { { 12, 0, 2 }, { 0, 37, 0 } } },
	{ { { 13, 0, 1 }, { 0, 38, 0 } } },
	{ { { 13, 0, 0 }, { 0, 39, 0 } } },
	{ { { 13, 0, 1 }, { 5, 30, 0 } } },
	{ { { 13, 0, 2 }, { 0, 40, 0 } } },
	{ { { 14, 0, 1 }, { 0, 41, 0 } } },
	{ { { 14, 0, 0 }, { 0, 42, 0 } } },
	{ { { 14, 0, 1 }, { 6, 31, 0 } } },
	{ { { 14, 0, 2 }, { 0, 43, 0 } } },
	{ { { 15, 0, 1 }, { 0, 44, 0 } } },
	{ { { 15, 0, 0 }, { 0, 45, 0 } } },
	{ { { 15, 0, 1 }, { 8, 30, 0 } } },
	{ { { 15, 0, 2 }, { 0, 46, 0 } } },
	{ { { 16, 0, 2 }, { 0, 47, 0 } } },
	{ { { 16, 0, 1 }, { 1, 46, 0 } } },
	{ { { 16, 0, 0 }, { 0, 48, 0 } } },
	{ { { 16, 0, 1 }, { 0, 49, 0 } } },
	{ { { 16, 0, 2 }, { 0, 50, 0 } } },
	{ { { 17, 0, 1 }, { 2, 47, 0 } } },
	{ { { 17, 0, 0 }, { 0, 51, 0 } } },
	{ { { 17, 0, 1 }, { 0, 52, 0 } } },
	{ { { 17, 0, 2 }, { 0, 53, 0 } } },
	{ { { 18, 0, 1 }, { 4, 46, 0 } } },
	{ { { 18, 0, 0 }, { 0, 54, 0 } } },
	{ { { 18, 0, 1 }, { 0, 55, 0 } } },
	{ { { 18, 0, 2 }, { 0, 56, 0 } } },
	{ { { 19, 0, 1 }, { 5, 47, 0 } } },
	{ { { 19, 0, 0 }, { 0, 57, 0 } } },
	{ { { 19, 0, 1 }, { 0, 58, 0 } } },
	{ { { 19, 0, 2 }, { 0, 59, 0 } } },
	{ { { 20, 0, 1 }, { 7, 46, 0 } } },
	{ { { 20, 0, 0 }, { 0, 60, 0 } } },
	{ { { 20, 0, 1 }, { 0, 61, 0 } } },
	{ { { 20, 0, 2 }, { 0, 62, 0 } } },
	{ { { 21, 0, 1 }, { 8, 47, 0 } } },
	{ { { 21, 0, 0 }, { 0, 63, 0 } } },
	{ { { 21, 0, 1 }, { 1, 62, 0 } } },
	{ { { 21, 0, 2 }, { 1, 63, 0 } } },
	{ { { 22, 0, 1 }, { 10, 46, 0 } } },
	{ { { 22, 0, 0 }, { 2, 62, 0 } } },
	{ { { 22, 0, 1 }, { 2, 63, 0 } } },
	{ { { 22, 0, 2 }, { 3, 62, 0 } } },
	{ { { 23, 0, 1 }, { 11, 47, 0 } } },
	{ { { 23, 0, 0 }, { 3, 63, 0 } } },
	{ { { 23, 0, 1 }, { 4, 62, 0 } } },
	{ { { 23, 0, 2 }, { 4, 63, 0 } } },
	{ { { 24, 0, 1 }, { 13, 46, 0 } } },
	{ { { 24, 0, 0 }, { 5, 62, 0 } } },
	{ { { 24, 0, 1 }, { 5, 63, 0 } } },
	{ { { 24, 0, 2 }, { 6, 62, 0 } } },
	{ { { 25, 0, 1 }, { 14, 47, 0 } } },
	{ { { 25, 0, 0 }, { 6, 63, 0 } } },
	{ { { 25, 0, 1 }, { 7, 62, 0 } } },
	{ { { 25, 0, 2 }, { 7, 63, 0 } } },
	{ { { 26, 0, 1 }, { 16, 45, 0 } } },
	{ { { 26, 0, 0 }, { 8, 62, 0 } } },
	{ { { 26, 0, 1 }, { 8, 63, 0 } } },
	{ { { 26, 0, 2 }, { 9, 62, 0 } } },
	{ { { 27, 0, 1 }, { 16, 48, 0 } } },
	{ { { 27, 0, 0 }, { 9, 63, 0 } } },
	{ { { 27, 0, 1 }, { 10, 62, 0 } } },
	{ { { 27, 0, 2 }, { 10, 63, 0 } } },
	{ { { 28, 0, 1 }, { 16, 51, 0 } } },
	{ { { 28, 0, 0 }, { 11, 62, 0 } } },
	{ { { 28, 0, 1 }, { 11, 63, 0 } } },
	{ { { 28, 0, 2 }, { 12, 62, 0 } } },
	{ { { 29, 0, 1 }, { 16, 54, 0 } } },
	{ { { 29, 0, 0 }, { 12, 63, 0 } } },
	{ { { 29, 0, 1 }, { 13, 62, 0 } } },
	{ { { 29, 0, 2 }, { 13, 63, 0 } } },
	{ { { 30, 0, 1 }, { 16, 57, 0 } } },
	{ { { 30, 0, 0 }, { 14, 62, 0 } } },
	{ { { 30, 0, 1 }, { 14, 63, 0 } } },
	{ { { 30, 0, 2 }, { 15, 62, 0 } } },
	{ { { 31, 0, 1 }, { 16, 60, 0 } } },
	{ { { 31, 0, 0 }, { 15, 63, 0 } } },
	{ { { 31, 0, 1 }, { 24, 46, 0 } } },
	{ { { 31, 0, 2 }, { 16, 62, 0 } } },
	{ { { 32, 0, 2 }, { 16, 63, 0 } } },
	{ { { 32, 0, 1 }, { 17, 62, 0 } } },
	{ { { 32, 0, 0 }, { 25, 47, 0 } } },
	{ { { 32, 0, 1 }, { 17, 63, 0 } } },
	{ { { 32, 0, 2 }, { 18, 62, 0 } } },
	{ { { 33, 0, 1 }, { 18, 63, 0 } } },
	{ { { 33, 0, 0 }, { 27, 46, 0 } } },
	{ { { 33, 0, 1 }, { 19, 62, 0 } } },
	{ { { 33, 0, 2 }, { 19, 63, 0 } } },
	{ { { 34, 0, 1 }, { 20, 62, 0 } } },
	{ { { 34, 0, 0 }, { 28, 47, 0 } } },
	{ { { 34, 0, 1 }, { 20, 63, 0 } } },
	{ { { 34, 0, 2 }, { 21, 62, 0 } } },
	{ { { 35, 0, 1 }, { 21, 63, 0 } } },
	{ { { 35, 0, 0 }, { 30, 46, 0 } } },
	{ { { 35, 0, 1 }, { 22, 62, 0 } } },
	{ { { 35, 0, 2 }, { 22, 63, 0 } } },
	{ { { 36, 0, 1 }, { 23, 62, 0 } } },
	{ { { 36, 0, 0 }, { 31, 47, 0 } } },
	{ { { 36, 0, 1 }, { 23, 63, 0 } } },
	{ { { 36, 0, 2 }, { 24, 62, 0 } } },
	{ { { 37, 0, 1 }, { 24, 63, 0 } } },
	{ { { 37, 0, 0 }, { 32, 47, 0 } } },
	{ { { 37, 0, 1 }, { 25, 62, 0 } } },
	{ { { 37, 0, 2 }, { 25, 63, 0 } } },
	{ { { 38, 0, 1 }, { 26, 62, 0 } } },
	{ { { 38, 0, 0 }, { 32, 50, 0 } } },
	{ { { 38, 0, 1 }, { 26, 63, 0 } } },
	{ { { 38, 0, 2 }, { 27, 62, 0 } } },
	{ { { 39, 0, 1 }, { 27, 63, 0 } } },
	{ { { 39, 0, 0 }, { 32, 53, 0 } } },
	{ { { 39, 0, 1 }, { 28, 62, 0 } } },
	{ { { 39, 0, 2 }, { 28, 63, 0 } } },
	{ { { 40, 0, 1 }, { 29, 62, 0 } } },
	{ { { 40, 0, 0 }, { 32, 56, 0 } } },
	{ { { 40, 0, 1 }, { 29, 63, 0 } } },
	{ { { 40, 0, 2 }, { 30, 62, 0 } } },
	{ { { 41, 0, 1 }, { 30, 63, 0 } } },
	{ { { 41, 0, 0 }, { 32, 59, 0 } } },
	{ { { 41, 0, 1 }, { 31, 62, 0 } } },
	{ { { 41, 0, 2 }, { 31, 63, 0 } } },
	{ { { 42, 0, 1 }, { 32, 61, 0 } } },
	{ { { 42, 0, 0 }, { 32, 62, 0 } } },
	{ { { 42, 0, 1 }, { 32, 63, 0 } } },
	{ { { 42, 0, 2 }, { 41, 46, 0 } } },
	{ { { 43, 0, 1 }, { 33, 62, 0 } } },
	{ { { 43, 0, 0 }, { 33, 63, 0 } } },
	{ { { 43, 0, 1 }, { 34, 62, 0 } } },
	{ { { 43, 0, 2 }, { 42, 47, 0 } } },
	{ { { 44, 0, 1 }, { 34, 63, 0 } } },
	{ { { 44, 0, 0 }, { 35, 62, 0 } } },
	{ { { 44, 0, 1 }, { 35, 63, 0 } } },
	{ { { 44, 0, 2 }, { 44, 46, 0 } } },
	{ { { 45, 0, 1 }, { 36, 62, 0 } } },
	{ { { 45, 0, 0 }, { 36, 63, 0 } } },
	{ { { 45, 0, 1 }, { 37, 62, 0 } } },
	{ { { 45, 0, 2 }, { 45, 47, 0 } } },
	{ { { 46, 0, 1 }, { 37, 63, 0 } } },
	{ { { 46, 0, 0 }, { 38, 62, 0 } } },
	{ { { 46, 0, 1 }, { 38, 63, 0 } } },
	{ { { 46, 0, 2 }, { 47, 46, 0 } } },
	{ { { 47, 0, 1 }, { 39, 62, 0 } } },
	{ { { 47, 0, 0 }, { 39, 63, 0 } } },
	{ { { 47, 0, 1 }, { 40, 62, 0 } } },
	{ { { 47, 0, 2 }, { 48, 46, 0 } } },
	{ { { 48, 0, 2 }, { 40, 63, 0 } } },
	{ { { 48, 0, 1 }, { 41, 62, 0 } } },
	{ { { 48, 0, 0 }, { 41, 63, 0 } } },
	{ { { 48, 0, 1 }, { 48, 49, 0 } } },
	{ { { 48, 0, 2 }, { 42, 62, 0 } } },
	{ { { 49, 0, 1 }, { 42, 63, 0 } } },
	{ { { 49, 0, 0 }, { 43, 62, 0 } } },
	{ { { 49, 0, 1 }, { 48, 52, 0 } } },
	{ { { 49, 0, 2 }, { 43, 63, 0 } } },
	{ { { 50, 0, 1 }, { 44, 62, 0 } } },
	{ { { 50, 0, 0 }, { 44, 63, 0 } } },
	{ { { 50, 0, 1 }, { 48, 55, 0 } } },
	{ { { 50, 0, 2 }, { 45, 62, 0 } } },
	{ { { 51, 0, 1 }, { 45, 63, 0 } } },
	{ { { 51, 0, 0 }, { 46, 62, 0 } } },
	{ { { 51, 0, 1 }, { 48, 58, 0 } } },
	{ { { 51, 0, 2 }, { 46, 63, 0 } } },
	{ { { 52, 0, 1 }, { 47, 62, 0 } } },
	{ { { 52, 0, 0 }, { 47, 63, 0 } } },
	{ { { 52, 0, 1 }, { 48, 61, 0 } } },
	{ { { 52, 0, 2 }, { 48, 62, 0 } } },
	{ { { 53, 0, 1 }, { 56, 47, 0 } } },
	{ { { 53, 0, 0 }, { 48, 63, 0 } } },
	{ { { 53, 0, 1 }, { 49, 62, 0 } } },
	{ { { 53, 0, 2 }, { 49, 63, 0 } } },
	{ { { 54, 0, 1 }, { 58, 46, 0 } } },
	{ { { 54, 0, 0 }, { 50, 62, 0 } } },
	{ { { 54, 0, 1 }, { 50, 63, 0 } } },
	{ { { 54, 0, 2 }, { 51, 62, 0 } } },
	{ { { 55, 0, 1 }, { 59, 47, 0 } } },
	{ { { 55, 0, 0 }, { 51, 63, 0 } } },
	{ { { 55, 0, 1 }, { 52, 62, 0 } } },
	{ { { 55, 0, 2 }, { 52, 63, 0 } } },
	{ { { 56, 0, 1 }, { 61, 46, 0 } } },
	{ { { 56, 0, 0 }, { 53, 62, 0 } } },
	{ { { 56, 0, 1 }, { 53, 63, 0 } } },
	{ { { 56, 0, 2 }, { 54, 62, 0 } } },
	{ { { 57, 0, 1 }, { 62, 47, 0 } } },
	{ { { 57, 0, 0 }, { 54, 63, 0 } } },
	{ { { 57, 0, 1 }, { 55, 62, 0 } } },
	{ { { 57, 0, 2 }, { 55, 63, 0 } } },
	{ { { 58, 0, 1 }, { 56, 62, 1 } } },
	{ { { 58, 0, 0 }, { 56, 62, 0 } } },
	{ { { 58, 0, 1 }, { 56, 63, 0 } } },
	{ { { 58, 0, 2 }, { 57, 62, 0 } } },
	{ { { 59, 0, 1 }, { 57, 63, 1 } } },
	{ { { 59, 0, 0 }, { 57, 63, 0 } } },
	{ { { 59, 0, 1 }, { 58, 62, 0 } } },
	{ { { 59, 0, 2 }, { 58, 63, 0 } } },
	{ { { 60, 0, 1 }, { 59, 62, 1 } } },
	{ { { 60, 0, 0 }, { 59, 62, 0 } } },
	{ { { 60, 0, 1 }, { 59, 63, 0 } } },
	{ { { 60, 0, 2 }, { 60, 62, 0 } } },
	{ { { 61, 0, 1 }, { 60, 63, 1 } } },
	{ { { 61, 0, 0 }, { 60, 63, 0 } } },
	{ { { 61, 0, 1 }, { 61, 62, 0 } } },
	{ { { 61, 0, 2 }, { 61, 63, 0 } } },
	{ { { 62, 0, 1 }, { 62, 62, 1 } } },
	{ { { 62, 0, 0 }, { 62, 62, 0 } } },
	{ { { 62, 0, 1 }, { 62, 63, 0 } } },
	{ { { 62, 0, 2 }, { 63, 62, 0 } } },
	{ { { 63, 0, 1 }, { 63, 63, 1 } } },
	{ { { 63, 0, 0 }, { 63, 63, 0 } } }
};

//=========================================singlecolourfit=========================================

class SingleColourFit : public ColourFit
{
public:
	SingleColourFit( ColourSet const* colours, int flags );

private:
	virtual void Compress3( void* block );
	virtual void Compress4( void* block );

	void ComputeEndPoints( SingleColourLookup const* const* lookups );

	u8 m_colour[3];
	Vec3 m_start;
	Vec3 m_end;
	u8 m_index;
	int m_error;
	int m_besterror;
};

}

//=========================================YkMemImage=========================================

namespace Yk {

class TOOLKIT_API YkMemImage  
{
public:
	// format 原图像的像素格式
	// width,height 原图像的宽度和高度
	// in 原图像的数据
	// comtype 图像压缩类型
	// out  压缩后的输出,外部必须分配足够的空间
	// return 返回压缩后图像数据的大小
	static unsigned int Encode( const unsigned int pixsize, unsigned int width, unsigned int height,const unsigned char* in,unsigned char *out );

	// 解压缩dxtn
	static void Decode( const unsigned int pixsize, unsigned int width, unsigned int height,unsigned char* out,const unsigned char *in );

	// 按照指定的大小缩放图片
	static void Scale(const unsigned int components, unsigned int widthin, unsigned int heightin,unsigned char *in,
		unsigned int widthout, unsigned int heightout, unsigned char *out);
};

}

#endif
