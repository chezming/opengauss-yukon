﻿/*
 *
 * YkMemImage.cpp
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

#include "Toolkit/YkMemImage.h"
#include <limits.h>
#include <algorithm>
#include <cfloat>
#include <memory.h>
#include <math.h>
#include <stdlib.h>

namespace squish {

//=========================================maths=========================================

Sym3x3 ComputeWeightedCovariance( int n, Vec3 const* points, float const* weights )
{
	// compute the centroid
	float total = 0.0f;
	Vec3 centroid( 0.0f );
	for( int i = 0; i < n; ++i )
	{
		total += weights[i];
		centroid += weights[i]*points[i];
	}
	if( total > FLT_EPSILON )
		centroid /= total;

	// accumulate the covariance matrix
	Sym3x3 covariance( 0.0f );
	for( int i = 0; i < n; ++i )
	{
		Vec3 a = points[i] - centroid;
		Vec3 b = weights[i]*a;

		covariance[0] += a.X()*b.X();
		covariance[1] += a.X()*b.Y();
		covariance[2] += a.X()*b.Z();
		covariance[3] += a.Y()*b.Y();
		covariance[4] += a.Y()*b.Z();
		covariance[5] += a.Z()*b.Z();
	}

	// return it
	return covariance;
}

#define POWER_ITERATION_COUNT 	8

Vec3 ComputePrincipleComponent( Sym3x3 const& matrix )
{
	Vec4 const row0( matrix[0], matrix[1], matrix[2], 0.0f );
	Vec4 const row1( matrix[1], matrix[3], matrix[4], 0.0f );
	Vec4 const row2( matrix[2], matrix[4], matrix[5], 0.0f );
	Vec4 v = VEC4_CONST( 1.0f );
	for( int i = 0; i < POWER_ITERATION_COUNT; ++i )
	{
		// matrix multiply
		Vec4 w = row0*v.SplatX();
		w = MultiplyAdd(row1, v.SplatY(), w);
		w = MultiplyAdd(row2, v.SplatZ(), w);

		// get max component from xyz in all channels
		Vec4 a = YkMax(w.SplatX(), YkMax(w.SplatY(), w.SplatZ()));

		// divide through and advance
		v = w*Reciprocal(a);
	}
	return v.GetVec3();
}

//=========================================squish=========================================

static int FixFlags( int flags )
{
	// grab the flag bits
	int method = flags & ( kDxt1 | kDxt3 | kDxt5 );
	int fit = flags & ( kColourIterativeClusterFit | kColourClusterFit | kColourRangeFit );
	int extra = flags & kWeightColourByAlpha;

	// set defaults
	if( method != kDxt3 && method != kDxt5 )
		method = kDxt1;
	if( fit != kColourRangeFit && fit != kColourIterativeClusterFit )
		fit = kColourClusterFit;

	// done
	return method | fit | extra;
}

void CompressMasked( u8 const* rgba, int mask, void* block, int flags, float* metric )
{
	// fix any bad flags
	flags = FixFlags( flags );

	// get the block locations
	void* colourBlock = block;
	void* alphaBock = block;
	if( ( flags & ( kDxt3 | kDxt5 ) ) != 0 )
		colourBlock = reinterpret_cast< u8* >( block ) + 8;

	// create the minimal point set
	ColourSet colours( rgba, mask, flags );

	// check the compression type and compress colour
	if( colours.GetCount() == 1 )
	{
		// always do a single colour fit
		SingleColourFit fit( &colours, flags );
		fit.Compress( colourBlock );
	}
	else if( ( flags & kColourRangeFit ) != 0 || colours.GetCount() == 0 )
	{
		// do a range fit
		RangeFit fit( &colours, flags, metric );
		fit.Compress( colourBlock );
	}
	else
	{
		// default to a cluster fit (could be iterative or not)
		ClusterFit fit( &colours, flags, metric );
		fit.Compress( colourBlock );
	}

	// compress alpha separately if necessary
	if( ( flags & kDxt3 ) != 0 )
		CompressAlphaDxt3( rgba, mask, alphaBock );
	else if( ( flags & kDxt5 ) != 0 )
		CompressAlphaDxt5( rgba, mask, alphaBock );
}

void Decompress( u8* rgba, void const* block, int flags )
{
	// fix any bad flags
	flags = FixFlags( flags );

	// get the block locations
	void const* colourBlock = block;
	void const* alphaBock = block;
	if( ( flags & ( kDxt3 | kDxt5 ) ) != 0 )
		colourBlock = reinterpret_cast< u8 const* >( block ) + 8;

	// decompress colour
	DecompressColour( rgba, colourBlock, ( flags & kDxt1 ) != 0 );

	// decompress alpha separately if necessary
	if( ( flags & kDxt3 ) != 0 )
		DecompressAlphaDxt3( rgba, alphaBock );
	else if( ( flags & kDxt5 ) != 0 )
		DecompressAlphaDxt5( rgba, alphaBock );
}

void CompressImage( u8 const* rgba, int width, int height, void* blocks, int flags, float* metric )
{
	// fix any bad flags
	flags = FixFlags( flags );

	// initialise the block output
	u8* targetBlock = reinterpret_cast< u8* >( blocks );
	int bytesPerBlock = ( ( flags & kDxt1 ) != 0 ) ? 8 : 16;

	// build the 4x4 block of pixels
	u8 sourceRgba[16*4];

	// loop over blocks
	for( int y = 0; y < height; y += 4 )
	{
		for( int x = 0; x < width; x += 4 )
		{
			u8* targetPixel = sourceRgba;
			int mask = 0;
			for( int py = 0; py < 4; ++py )
			{
				for( int px = 0; px < 4; ++px )
				{
					// get the source pixel in the image
					int sx = x + px;
					int sy = y + py;

					// enable if we're in the image
					if( sx < width && sy < height )
					{
						// copy the rgba value
						u8 const* sourcePixel = rgba + 4*( width*sy + sx );
						for( int i = 0; i < 4; ++i )
							*targetPixel++ = *sourcePixel++;

						// enable this pixel
						mask |= ( 1 << ( 4*py + px ) );
					}
					else
					{
						// skip this pixel as its outside the image
						targetPixel += 4;
					}
				}
			}

			// compress it into the output
			CompressMasked( sourceRgba, mask, targetBlock, flags, metric );

			// advance
			targetBlock += bytesPerBlock;
		}
	}
}

void DecompressImage( u8* rgba, int width, int height, void const* blocks, int flags )
{
	// fix any bad flags
	flags = FixFlags( flags );

	// initialise the block input
	u8 const* sourceBlock = reinterpret_cast< u8 const* >( blocks );
	int bytesPerBlock = ( ( flags & kDxt1 ) != 0 ) ? 8 : 16;

	// loop over blocks
	for( int y = 0; y < height; y += 4 )
	{
		for( int x = 0; x < width; x += 4 )
		{
			// decompress the block
			u8 targetRgba[4*16];
			Decompress( targetRgba, sourceBlock, flags );

			// write the decompressed pixels to the correct image locations
			u8 const* sourcePixel = targetRgba;
			for( int py = 0; py < 4; ++py )
			{
				for( int px = 0; px < 4; ++px )
				{
					// get the target location
					int sx = x + px;
					int sy = y + py;
					if( sx < width && sy < height )
					{
						u8* targetPixel = rgba + 4*( width*sy + sx );

						// copy the rgba value
						for( int i = 0; i < 4; ++i )
							*targetPixel++ = *sourcePixel++;
					}
					else
					{
						// skip this pixel as its outside the image
						sourcePixel += 4;
					}
				}
			}

			// advance
			sourceBlock += bytesPerBlock;
		}
	}
}

//=========================================alpha=========================================

static int FloatToInt( float a, int limit )
{
	// use ANSI round-to-zero behaviour to get round-to-nearest
	int i = ( int )( a + 0.5f );

	// clamp to the limit
	if( i < 0 )
		i = 0;
	else if( i > limit )
		i = limit; 

	// done
	return i;
}

void CompressAlphaDxt3( u8 const* rgba, int mask, void* block )
{
	u8* bytes = reinterpret_cast< u8* >( block );

	// quantise and pack the alpha values pairwise
	for( int i = 0; i < 8; ++i )
	{
		// quantise down to 4 bits
		float alpha1 = ( float )rgba[8*i + 3] * ( 15.0f/255.0f );
		float alpha2 = ( float )rgba[8*i + 7] * ( 15.0f/255.0f );
		int quant1 = FloatToInt( alpha1, 15 );
		int quant2 = FloatToInt( alpha2, 15 );

		// set alpha to zero where masked
		int bit1 = 1 << ( 2*i );
		int bit2 = 1 << ( 2*i + 1 );
		if( ( mask & bit1 ) == 0 )
			quant1 = 0;
		if( ( mask & bit2 ) == 0 )
			quant2 = 0;

		// pack into the byte
		bytes[i] = ( u8 )( quant1 | ( quant2 << 4 ) );
	}
}

void DecompressAlphaDxt3( u8* rgba, void const* block )
{
	u8 const* bytes = reinterpret_cast< u8 const* >( block );

	// unpack the alpha values pairwise
	for( int i = 0; i < 8; ++i )
	{
		// quantise down to 4 bits
		u8 quant = bytes[i];

		// unpack the values
		u8 lo = quant & 0x0f;
		u8 hi = quant & 0xf0;

		// convert back up to bytes
		rgba[8*i + 3] = lo | ( lo << 4 );
		rgba[8*i + 7] = hi | ( hi >> 4 );
	}
}

static void FixRange( int& min, int& max, int steps )
{
	if( max - min < steps )
		max = std::min( min + steps, 255 );
	if( max - min < steps )
		min = std::max( 0, max - steps );
}

static int FitCodes( u8 const* rgba, int mask, u8 const* codes, u8* indices )
{
	// fit each alpha value to the codebook
	int err = 0;
	for( int i = 0; i < 16; ++i )
	{
		// check this pixel is valid
		int bit = 1 << i;
		if( ( mask & bit ) == 0 )
		{
			// use the first code
			indices[i] = 0;
			continue;
		}

		// find the least error and corresponding index
		int value = rgba[4*i + 3];
		int least = INT_MAX;
		int index = 0;
		for( int j = 0; j < 8; ++j )
		{
			// get the squared error from this code
			int dist = ( int )value - ( int )codes[j];
			dist *= dist;

			// compare with the best so far
			if( dist < least )
			{
				least = dist;
				index = j;
			}
		}

		// save this index and accumulate the error
		indices[i] = ( u8 )index;
		err += least;
	}

	// return the total error
	return err;
}

static void WriteAlphaBlock( int alpha0, int alpha1, u8 const* indices, void* block )
{
	u8* bytes = reinterpret_cast< u8* >( block );

	// write the first two bytes
	bytes[0] = ( u8 )alpha0;
	bytes[1] = ( u8 )alpha1;

	// pack the indices with 3 bits each
	u8* dest = bytes + 2;
	u8 const* src = indices;
	for( int i = 0; i < 2; ++i )
	{
		// pack 8 3-bit values
		int value = 0;
		for( int j = 0; j < 8; ++j )
		{
			int index = *src++;
			value |= ( index << 3*j );
		}

		// store in 3 bytes
		for( int j = 0; j < 3; ++j )
		{
			int byte = ( value >> 8*j ) & 0xff;
			*dest++ = ( u8 )byte;
		}
	}
}

static void WriteAlphaBlock5( int alpha0, int alpha1, u8 const* indices, void* block )
{
	// check the relative values of the endpoints
	if( alpha0 > alpha1 )
	{
		// swap the indices
		u8 swapped[16];
		for( int i = 0; i < 16; ++i )
		{
			u8 index = indices[i];
			if( index == 0 )
				swapped[i] = 1;
			else if( index == 1 )
				swapped[i] = 0;
			else if( index <= 5 )
				swapped[i] = 7 - index;
			else 
				swapped[i] = index;
		}

		// write the block
		WriteAlphaBlock( alpha1, alpha0, swapped, block );
	}
	else
	{
		// write the block
		WriteAlphaBlock( alpha0, alpha1, indices, block );
	}	
}

static void WriteAlphaBlock7( int alpha0, int alpha1, u8 const* indices, void* block )
{
	// check the relative values of the endpoints
	if( alpha0 < alpha1 )
	{
		// swap the indices
		u8 swapped[16];
		for( int i = 0; i < 16; ++i )
		{
			u8 index = indices[i];
			if( index == 0 )
				swapped[i] = 1;
			else if( index == 1 )
				swapped[i] = 0;
			else
				swapped[i] = 9 - index;
		}

		// write the block
		WriteAlphaBlock( alpha1, alpha0, swapped, block );
	}
	else
	{
		// write the block
		WriteAlphaBlock( alpha0, alpha1, indices, block );
	}	
}

void CompressAlphaDxt5( u8 const* rgba, int mask, void* block )
{
	// get the range for 5-alpha and 7-alpha interpolation
	int min5 = 255;
	int max5 = 0;
	int min7 = 255;
	int max7 = 0;
	for( int i = 0; i < 16; ++i )
	{
		// check this pixel is valid
		int bit = 1 << i;
		if( ( mask & bit ) == 0 )
			continue;

		// incorporate into the min/max
		int value = rgba[4*i + 3];
		if( value < min7 )
			min7 = value;
		if( value > max7 )
			max7 = value;
		if( value != 0 && value < min5 )
			min5 = value;
		if( value != 255 && value > max5 )
			max5 = value;
	}

	// handle the case that no valid range was found
	if( min5 > max5 )
		min5 = max5;
	if( min7 > max7 )
		min7 = max7;

	// fix the range to be the minimum in each case
	FixRange( min5, max5, 5 );
	FixRange( min7, max7, 7 );

	// set up the 5-alpha code book
	u8 codes5[8];
	codes5[0] = ( u8 )min5;
	codes5[1] = ( u8 )max5;
	for( int i = 1; i < 5; ++i )
		codes5[1 + i] = ( u8 )( ( ( 5 - i )*min5 + i*max5 )/5 );
	codes5[6] = 0;
	codes5[7] = 255;

	// set up the 7-alpha code book
	u8 codes7[8];
	codes7[0] = ( u8 )min7;
	codes7[1] = ( u8 )max7;
	for( int i = 1; i < 7; ++i )
		codes7[1 + i] = ( u8 )( ( ( 7 - i )*min7 + i*max7 )/7 );

	// fit the data to both code books
	u8 indices5[16];
	u8 indices7[16];
	int err5 = FitCodes( rgba, mask, codes5, indices5 );
	int err7 = FitCodes( rgba, mask, codes7, indices7 );

	// save the block with least error
	if( err5 <= err7 )
		WriteAlphaBlock5( min5, max5, indices5, block );
	else
		WriteAlphaBlock7( min7, max7, indices7, block );
}

void DecompressAlphaDxt5( u8* rgba, void const* block )
{
	// get the two alpha values
	u8 const* bytes = reinterpret_cast< u8 const* >( block );
	int alpha0 = bytes[0];
	int alpha1 = bytes[1];

	// compare the values to build the codebook
	u8 codes[8];
	codes[0] = ( u8 )alpha0;
	codes[1] = ( u8 )alpha1;
	if( alpha0 <= alpha1 )
	{
		// use 5-alpha codebook
		for( int i = 1; i < 5; ++i )
			codes[1 + i] = ( u8 )( ( ( 5 - i )*alpha0 + i*alpha1 )/5 );
		codes[6] = 0;
		codes[7] = 255;
	}
	else
	{
		// use 7-alpha codebook
		for( int i = 1; i < 7; ++i )
			codes[1 + i] = ( u8 )( ( ( 7 - i )*alpha0 + i*alpha1 )/7 );
	}

	// decode the indices
	u8 indices[16];
	u8 const* src = bytes + 2;
	u8* dest = indices;
	for( int i = 0; i < 2; ++i )
	{
		// grab 3 bytes
		int value = 0;
		for( int j = 0; j < 3; ++j )
		{
			int byte = *src++;
			value |= ( byte << 8*j );
		}

		// unpack 8 3-bit values from it
		for( int j = 0; j < 8; ++j )
		{
			int index = ( value >> 3*j ) & 0x7;
			*dest++ = ( u8 )index;
		}
	}

	// write out the indexed codebook values
	for( int i = 0; i < 16; ++i )
		rgba[4*i + 3] = codes[indices[i]];
}

//=========================================colourset=========================================

ColourSet::ColourSet( u8 const* rgba, int mask, int flags )
	: m_count( 0 ), 
	m_transparent( false )
{
	// check the compression mode for dxt1
	bool isDxt1 = ( ( flags & kDxt1 ) != 0 );
	bool weightByAlpha = ( ( flags & kWeightColourByAlpha ) != 0 );

	// create the minimal set
	for( int i = 0; i < 16; ++i )
	{
		// check this pixel is enabled
		int bit = 1 << i;
		if( ( mask & bit ) == 0 )
		{
			m_remap[i] = -1;
			continue;
		}

		// check for transparent pixels when using dxt1
		if( isDxt1 && rgba[4*i + 3] < 128 )
		{
			m_remap[i] = -1;
			m_transparent = true;
			continue;
		}

		// loop over previous points for a match
		for( int j = 0;; ++j )
		{
			// allocate a new point
			if( j == i )
			{
				// normalise coordinates to [0,1]
				float x = ( float )rgba[4*i] / 255.0f;
				float y = ( float )rgba[4*i + 1] / 255.0f;
				float z = ( float )rgba[4*i + 2] / 255.0f;

				// ensure there is always non-zero weight even for zero alpha
				float w = ( float )( rgba[4*i + 3] + 1 ) / 256.0f;

				// add the point
				m_points[m_count] = Vec3( x, y, z );
				m_weights[m_count] = ( weightByAlpha ? w : 1.0f );
				m_remap[i] = m_count;

				// advance
				++m_count;
				break;
			}

			// check for a match
			int oldbit = 1 << j;
			bool match = ( ( mask & oldbit ) != 0 )
				&& ( rgba[4*i] == rgba[4*j] )
				&& ( rgba[4*i + 1] == rgba[4*j + 1] )
				&& ( rgba[4*i + 2] == rgba[4*j + 2] )
				&& ( rgba[4*j + 3] >= 128 || !isDxt1 );
			if( match )
			{
				// get the index of the match
				int index = m_remap[j];

				// ensure there is always non-zero weight even for zero alpha
				float w = ( float )( rgba[4*i + 3] + 1 ) / 256.0f;

				// map to this point and increase the weight
				m_weights[index] += ( weightByAlpha ? w : 1.0f );
				m_remap[i] = index;
				break;
			}
		}
	}

	// square root the weights
	for( int i = 0; i < m_count; ++i )
		m_weights[i] = std::sqrt( m_weights[i] );
}

void ColourSet::RemapIndices( u8 const* source, u8* target ) const
{
	for( int i = 0; i < 16; ++i )
	{
		int j = m_remap[i];
		if( j == -1 )
			target[i] = 3;
		else
			target[i] = source[j];
	}
}

//=========================================colourfit=========================================

ColourFit::ColourFit( ColourSet const* colours, int flags ) 
	: m_colours( colours ), 
	m_flags( flags )
{
}

ColourFit::~ColourFit()
{
}

void ColourFit::Compress( void* block )
{
	Compress4( block );
}

//=========================================clusterfit=========================================

ClusterFit::ClusterFit( ColourSet const* colours, int flags, float* metric ) 
	: ColourFit( colours, flags )
{
	// set the iteration count
	m_iterationCount = ( m_flags & kColourIterativeClusterFit ) ? kMaxIterations : 1;

	// initialise the metric (old perceptual = 0.2126f, 0.7152f, 0.0722f)
	if( metric )
		m_metric = Vec4( metric[0], metric[1], metric[2], 1.0f );
	else
		m_metric = VEC4_CONST( 1.0f );	

	// initialise the best error
	m_besterror = VEC4_CONST( FLT_MAX );

	// cache some values
	int const count = m_colours->GetCount();
	Vec3 const* values = m_colours->GetPoints();

	// get the covariance matrix
	Sym3x3 covariance = ComputeWeightedCovariance( count, values, m_colours->GetWeights() );

	// compute the principle component
	m_principle = ComputePrincipleComponent( covariance );
}

bool ClusterFit::ConstructOrdering( Vec3 const& axis, int iteration )
{
	// cache some values
	int const count = m_colours->GetCount();
	Vec3 const* values = m_colours->GetPoints();

	// build the list of dot products
	float dps[16];
	u8* order = ( u8* )m_order + 16*iteration;
	for( int i = 0; i < count; ++i )
	{
		dps[i] = Dot( values[i], axis );
		order[i] = ( u8 )i;
	}

	// stable sort using them
	for( int i = 0; i < count; ++i )
	{
		for( int j = i; j > 0 && dps[j] < dps[j - 1]; --j )
		{
			std::swap( dps[j], dps[j - 1] );
			std::swap( order[j], order[j - 1] );
		}
	}

	// check this ordering is unique
	for( int it = 0; it < iteration; ++it )
	{
		u8 const* prev = ( u8* )m_order + 16*it;
		bool same = true;
		for( int i = 0; i < count; ++i )
		{
			if( order[i] != prev[i] )
			{
				same = false;
				break;
			}
		}
		if( same )
			return false;
	}

	// copy the ordering and weight all the points
	Vec3 const* unweighted = m_colours->GetPoints();
	float const* weights = m_colours->GetWeights();
	m_xsum_wsum = VEC4_CONST( 0.0f );
	for( int i = 0; i < count; ++i )
	{
		int j = order[i];
		Vec4 p( unweighted[j].X(), unweighted[j].Y(), unweighted[j].Z(), 1.0f );
		Vec4 w( weights[j] );
		Vec4 x = p*w;
		m_points_weights[i] = x;
		m_xsum_wsum += x;
	}
	return true;
}

void ClusterFit::Compress3( void* block )
{
	// declare variables
	int const count = m_colours->GetCount();
	Vec4 const two = VEC4_CONST( 2.0 );
	Vec4 const one = VEC4_CONST( 1.0f );
	Vec4 const half_half2( 0.5f, 0.5f, 0.5f, 0.25f );
	Vec4 const zero = VEC4_CONST( 0.0f );
	Vec4 const half = VEC4_CONST( 0.5f );
	Vec4 const grid( 31.0f, 63.0f, 31.0f, 0.0f );
	Vec4 const gridrcp( 1.0f/31.0f, 1.0f/63.0f, 1.0f/31.0f, 0.0f );

	// prepare an ordering using the principle axis
	ConstructOrdering( m_principle, 0 );

	// check all possible clusters and iterate on the total order
	Vec4 beststart = VEC4_CONST( 0.0f );
	Vec4 bestend = VEC4_CONST( 0.0f );
	Vec4 besterror = m_besterror;
	u8 bestindices[16];
	int bestiteration = 0;
	int besti = 0, bestj = 0;

	// loop over iterations (we avoid the case that all points in first or last cluster)
	for( int iterationIndex = 0;; )
	{
		// first cluster [0,i) is at the start
		Vec4 part0 = VEC4_CONST( 0.0f );
		for( int i = 0; i < count; ++i )
		{
			// second cluster [i,j) is half along
			Vec4 part1 = ( i == 0 ) ? m_points_weights[0] : VEC4_CONST( 0.0f );
			int jmin = ( i == 0 ) ? 1 : i;
			for( int j = jmin;; )
			{
				// last cluster [j,count) is at the end
				Vec4 part2 = m_xsum_wsum - part1 - part0;

				// compute least squares terms directly
				Vec4 alphax_sum = MultiplyAdd( part1, half_half2, part0 );
				Vec4 alpha2_sum = alphax_sum.SplatW();

				Vec4 betax_sum = MultiplyAdd( part1, half_half2, part2 );
				Vec4 beta2_sum = betax_sum.SplatW();

				Vec4 alphabeta_sum = ( part1*half_half2 ).SplatW();

				// compute the least-squares optimal points
				Vec4 factor = Reciprocal( NegativeMultiplySubtract( alphabeta_sum, alphabeta_sum, alpha2_sum*beta2_sum ) );
				Vec4 a = NegativeMultiplySubtract( betax_sum, alphabeta_sum, alphax_sum*beta2_sum )*factor;
				Vec4 b = NegativeMultiplySubtract( alphax_sum, alphabeta_sum, betax_sum*alpha2_sum )*factor;

				// clamp to the grid
				a = YkMin( one, YkMax( zero, a ) );
				b = YkMin( one, YkMax( zero, b ) );
				a = Truncate( MultiplyAdd( grid, a, half ) )*gridrcp;
				b = Truncate( MultiplyAdd( grid, b, half ) )*gridrcp;

				// compute the error (we skip the constant xxsum)
				Vec4 e1 = MultiplyAdd( a*a, alpha2_sum, b*b*beta2_sum );
				Vec4 e2 = NegativeMultiplySubtract( a, alphax_sum, a*b*alphabeta_sum );
				Vec4 e3 = NegativeMultiplySubtract( b, betax_sum, e2 );
				Vec4 e4 = MultiplyAdd( two, e3, e1 );

				// apply the metric to the error term
				Vec4 e5 = e4*m_metric;
				Vec4 error = e5.SplatX() + e5.SplatY() + e5.SplatZ();

				// keep the solution if it wins
				if( CompareAnyLessThan( error, besterror ) )
				{
					beststart = a;
					bestend = b;
					besti = i;
					bestj = j;
					besterror = error;
					bestiteration = iterationIndex;
				}

				// advance
				if( j == count )
					break;
				part1 += m_points_weights[j];
				++j;
			}

			// advance
			part0 += m_points_weights[i];
		}

		// stop if we didn't improve in this iteration
		if( bestiteration != iterationIndex )
			break;

		// advance if possible
		++iterationIndex;
		if( iterationIndex == m_iterationCount )
			break;

		// stop if a new iteration is an ordering that has already been tried
		Vec3 axis = ( bestend - beststart ).GetVec3();
		if( !ConstructOrdering( axis, iterationIndex ) )
			break;
	}

	// save the block if necessary
	if( CompareAnyLessThan( besterror, m_besterror ) )
	{
		// remap the indices
		u8 const* order = ( u8* )m_order + 16*bestiteration;

		u8 unordered[16];
		for( int m = 0; m < besti; ++m )
			unordered[order[m]] = 0;
		for( int m = besti; m < bestj; ++m )
			unordered[order[m]] = 2;
		for( int m = bestj; m < count; ++m )
			unordered[order[m]] = 1;

		m_colours->RemapIndices( unordered, bestindices );

		// save the block
		WriteColourBlock3( beststart.GetVec3(), bestend.GetVec3(), bestindices, block );

		// save the error
		m_besterror = besterror;
	}
}

void ClusterFit::Compress4( void* block )
{
	// declare variables
	int const count = m_colours->GetCount();
	Vec4 const two = VEC4_CONST( 2.0f );
	Vec4 const one = VEC4_CONST( 1.0f );
	Vec4 const onethird_onethird2( 1.0f/3.0f, 1.0f/3.0f, 1.0f/3.0f, 1.0f/9.0f );
	Vec4 const twothirds_twothirds2( 2.0f/3.0f, 2.0f/3.0f, 2.0f/3.0f, 4.0f/9.0f );
	Vec4 const twonineths = VEC4_CONST( 2.0f/9.0f );
	Vec4 const zero = VEC4_CONST( 0.0f );
	Vec4 const half = VEC4_CONST( 0.5f );
	Vec4 const grid( 31.0f, 63.0f, 31.0f, 0.0f );
	Vec4 const gridrcp( 1.0f/31.0f, 1.0f/63.0f, 1.0f/31.0f, 0.0f );

	// prepare an ordering using the principle axis
	ConstructOrdering( m_principle, 0 );

	// check all possible clusters and iterate on the total order
	Vec4 beststart = VEC4_CONST( 0.0f );
	Vec4 bestend = VEC4_CONST( 0.0f );
	Vec4 besterror = m_besterror;
	u8 bestindices[16];
	int bestiteration = 0;
	int besti = 0, bestj = 0, bestk = 0;

	// loop over iterations (we avoid the case that all points in first or last cluster)
	for( int iterationIndex = 0;; )
	{
		// first cluster [0,i) is at the start
		Vec4 part0 = VEC4_CONST( 0.0f );
		for( int i = 0; i < count; ++i )
		{
			// second cluster [i,j) is one third along
			Vec4 part1 = VEC4_CONST( 0.0f );
			for( int j = i;; )
			{
				// third cluster [j,k) is two thirds along
				Vec4 part2 = ( j == 0 ) ? m_points_weights[0] : VEC4_CONST( 0.0f );
				int kmin = ( j == 0 ) ? 1 : j;
				for( int k = kmin;; )
				{
					// last cluster [k,count) is at the end
					Vec4 part3 = m_xsum_wsum - part2 - part1 - part0;

					// compute least squares terms directly
					Vec4 const alphax_sum = MultiplyAdd( part2, onethird_onethird2, MultiplyAdd( part1, twothirds_twothirds2, part0 ) );
					Vec4 const alpha2_sum = alphax_sum.SplatW();

					Vec4 const betax_sum = MultiplyAdd( part1, onethird_onethird2, MultiplyAdd( part2, twothirds_twothirds2, part3 ) );
					Vec4 const beta2_sum = betax_sum.SplatW();

					Vec4 const alphabeta_sum = twonineths*( part1 + part2 ).SplatW();

					// compute the least-squares optimal points
					Vec4 factor = Reciprocal( NegativeMultiplySubtract( alphabeta_sum, alphabeta_sum, alpha2_sum*beta2_sum ) );
					Vec4 a = NegativeMultiplySubtract( betax_sum, alphabeta_sum, alphax_sum*beta2_sum )*factor;
					Vec4 b = NegativeMultiplySubtract( alphax_sum, alphabeta_sum, betax_sum*alpha2_sum )*factor;

					// clamp to the grid
					a = YkMin( one, YkMax( zero, a ) );
					b = YkMin( one, YkMax( zero, b ) );
					a = Truncate( MultiplyAdd( grid, a, half ) )*gridrcp;
					b = Truncate( MultiplyAdd( grid, b, half ) )*gridrcp;

					// compute the error (we skip the constant xxsum)
					Vec4 e1 = MultiplyAdd( a*a, alpha2_sum, b*b*beta2_sum );
					Vec4 e2 = NegativeMultiplySubtract( a, alphax_sum, a*b*alphabeta_sum );
					Vec4 e3 = NegativeMultiplySubtract( b, betax_sum, e2 );
					Vec4 e4 = MultiplyAdd( two, e3, e1 );

					// apply the metric to the error term
					Vec4 e5 = e4*m_metric;
					Vec4 error = e5.SplatX() + e5.SplatY() + e5.SplatZ();

					// keep the solution if it wins
					if( CompareAnyLessThan( error, besterror ) )
					{
						beststart = a;
						bestend = b;
						besterror = error;
						besti = i;
						bestj = j;
						bestk = k;
						bestiteration = iterationIndex;
					}

					// advance
					if( k == count )
						break;
					part2 += m_points_weights[k];
					++k;
				}

				// advance
				if( j == count )
					break;
				part1 += m_points_weights[j];
				++j;
			}

			// advance
			part0 += m_points_weights[i];
		}

		// stop if we didn't improve in this iteration
		if( bestiteration != iterationIndex )
			break;

		// advance if possible
		++iterationIndex;
		if( iterationIndex == m_iterationCount )
			break;

		// stop if a new iteration is an ordering that has already been tried
		Vec3 axis = ( bestend - beststart ).GetVec3();
		if( !ConstructOrdering( axis, iterationIndex ) )
			break;
	}

	// save the block if necessary
	if( CompareAnyLessThan( besterror, m_besterror ) )
	{
		// remap the indices
		u8 const* order = ( u8* )m_order + 16*bestiteration;

		u8 unordered[16];
		for( int m = 0; m < besti; ++m )
			unordered[order[m]] = 0;
		for( int m = besti; m < bestj; ++m )
			unordered[order[m]] = 2;
		for( int m = bestj; m < bestk; ++m )
			unordered[order[m]] = 3;
		for( int m = bestk; m < count; ++m )
			unordered[order[m]] = 1;

		m_colours->RemapIndices( unordered, bestindices );

		// save the block
		WriteColourBlock4( beststart.GetVec3(), bestend.GetVec3(), bestindices, block );

		// save the error
		m_besterror = besterror;
	}
}

//=========================================colourblock=========================================

static int FloatTo565( Vec3::Arg colour )
{
	// get the components in the correct range
	int r = FloatToInt( 31.0f*colour.X(), 31 );
	int g = FloatToInt( 63.0f*colour.Y(), 63 );
	int b = FloatToInt( 31.0f*colour.Z(), 31 );

	// pack into a single value
	return ( r << 11 ) | ( g << 5 ) | b;
}

static void WriteColourBlock( int a, int b, u8* indices, void* block )
{
	// get the block as bytes
	u8* bytes = ( u8* )block;

	// write the endpoints
	bytes[0] = ( u8 )( a & 0xff );
	bytes[1] = ( u8 )( a >> 8 );
	bytes[2] = ( u8 )( b & 0xff );
	bytes[3] = ( u8 )( b >> 8 );

	// write the indices
	for( int i = 0; i < 4; ++i )
	{
		u8 const* ind = indices + 4*i;
		bytes[4 + i] = ind[0] | ( ind[1] << 2 ) | ( ind[2] << 4 ) | ( ind[3] << 6 );
	}
}

void WriteColourBlock3( Vec3::Arg start, Vec3::Arg end, u8 const* indices, void* block )
{
	// get the packed values
	int a = FloatTo565( start );
	int b = FloatTo565( end );

	// remap the indices
	u8 remapped[16];
	if( a <= b )
	{
		// use the indices directly
		for( int i = 0; i < 16; ++i )
			remapped[i] = indices[i];
	}
	else
	{
		// swap a and b
		std::swap( a, b );
		for( int i = 0; i < 16; ++i )
		{
			if( indices[i] == 0 )
				remapped[i] = 1;
			else if( indices[i] == 1 )
				remapped[i] = 0;
			else
				remapped[i] = indices[i];
		}
	}

	// write the block
	WriteColourBlock( a, b, remapped, block );
}

void WriteColourBlock4( Vec3::Arg start, Vec3::Arg end, u8 const* indices, void* block )
{
	// get the packed values
	int a = FloatTo565( start );
	int b = FloatTo565( end );

	// remap the indices
	u8 remapped[16];
	if( a < b )
	{
		// swap a and b
		std::swap( a, b );
		for( int i = 0; i < 16; ++i )
			remapped[i] = ( indices[i] ^ 0x1 ) & 0x3;
	}
	else if( a == b )
	{
		// use index 0
		for( int i = 0; i < 16; ++i )
			remapped[i] = 0;
	}
	else
	{
		// use the indices directly
		for( int i = 0; i < 16; ++i )
			remapped[i] = indices[i];
	}

	// write the block
	WriteColourBlock( a, b, remapped, block );
}

static int Unpack565( u8 const* packed, u8* colour )
{
	// build the packed value
	int value = ( int )packed[0] | ( ( int )packed[1] << 8 );

	// get the components in the stored range
	u8 red = ( u8 )( ( value >> 11 ) & 0x1f );
	u8 green = ( u8 )( ( value >> 5 ) & 0x3f );
	u8 blue = ( u8 )( value & 0x1f );

	// scale up to 8 bits
	colour[0] = ( red << 3 ) | ( red >> 2 );
	colour[1] = ( green << 2 ) | ( green >> 4 );
	colour[2] = ( blue << 3 ) | ( blue >> 2 );
	colour[3] = 255;

	// return the value
	return value;
}

void DecompressColour( u8* rgba, void const* block, bool isDxt1 )
{
	// get the block bytes
	u8 const* bytes = reinterpret_cast< u8 const* >( block );

	// unpack the endpoints
	u8 codes[16];
	int a = Unpack565( bytes, codes );
	int b = Unpack565( bytes + 2, codes + 4 );

	// generate the midpoints
	for( int i = 0; i < 3; ++i )
	{
		int c = codes[i];
		int d = codes[4 + i];

		if( isDxt1 && a <= b )
		{
			codes[8 + i] = ( u8 )( ( c + d )/2 );
			codes[12 + i] = 0;
		}
		else
		{
			codes[8 + i] = ( u8 )( ( 2*c + d )/3 );
			codes[12 + i] = ( u8 )( ( c + 2*d )/3 );
		}
	}

	// fill in alpha for the intermediate values
	codes[8 + 3] = 255;
	codes[12 + 3] = ( isDxt1 && a <= b ) ? 0 : 255;

	// unpack the indices
	u8 indices[16];
	for( int i = 0; i < 4; ++i )
	{
		u8* ind = indices + 4*i;
		u8 packed = bytes[4 + i];

		ind[0] = packed & 0x3;
		ind[1] = ( packed >> 2 ) & 0x3;
		ind[2] = ( packed >> 4 ) & 0x3;
		ind[3] = ( packed >> 6 ) & 0x3;
	}

	// store out the colours
	for( int i = 0; i < 16; ++i )
	{
		u8 offset = 4*indices[i];
		for( int j = 0; j < 4; ++j )
			rgba[4*i + j] = codes[offset + j];
	}
}

//=========================================rangefit=========================================

RangeFit::RangeFit( ColourSet const* colours, int flags, float* metric ) 
	: ColourFit( colours, flags )
{
	// initialise the metric (old perceptual = 0.2126f, 0.7152f, 0.0722f)
	if( metric )
		m_metric = Vec3( metric[0], metric[1], metric[2] );
	else
		m_metric = Vec3( 1.0f );	

	// initialise the best error
	m_besterror = FLT_MAX;

	// cache some values
	int const count = m_colours->GetCount();
	Vec3 const* values = m_colours->GetPoints();
	float const* weights = m_colours->GetWeights();

	// get the covariance matrix
	Sym3x3 covariance = ComputeWeightedCovariance( count, values, weights );

	// compute the principle component
	Vec3 principle = ComputePrincipleComponent( covariance );

	// get the min and max range as the codebook endpoints
	Vec3 start( 0.0f );
	Vec3 end( 0.0f );
	if( count > 0 )
	{
		float min, max;

		// compute the range
		start = end = values[0];
		min = max = Dot( values[0], principle );
		for( int i = 1; i < count; ++i )
		{
			float val = Dot( values[i], principle );
			if( val < min )
			{
				start = values[i];
				min = val;
			}
			else if( val > max )
			{
				end = values[i];
				max = val;
			}
		}
	}

	// clamp the output to [0, 1]
	Vec3 const one( 1.0f );
	Vec3 const zero( 0.0f );
	start = YkMin( one, YkMax( zero, start ) );
	end = YkMin( one, YkMax( zero, end ) );

	// clamp to the grid and save
	Vec3 const grid( 31.0f, 63.0f, 31.0f );
	Vec3 const gridrcp( 1.0f/31.0f, 1.0f/63.0f, 1.0f/31.0f );
	Vec3 const half( 0.5f );
	m_start = Truncate( grid*start + half )*gridrcp;
	m_end = Truncate( grid*end + half )*gridrcp;
}

void RangeFit::Compress3( void* block )
{
	// cache some values
	int const count = m_colours->GetCount();
	Vec3 const* values = m_colours->GetPoints();

	// create a codebook
	Vec3 codes[3];
	codes[0] = m_start;
	codes[1] = m_end;
	codes[2] = 0.5f*m_start + 0.5f*m_end;

	// match each point to the closest code
	u8 closest[16];
	float error = 0.0f;
	for( int i = 0; i < count; ++i )
	{
		// find the closest code
		float dist = FLT_MAX;
		int idx = 0;
		for( int j = 0; j < 3; ++j )
		{
			float d = LengthSquared( m_metric*( values[i] - codes[j] ) );
			if( d < dist )
			{
				dist = d;
				idx = j;
			}
		}

		// save the index
		closest[i] = ( u8 )idx;

		// accumulate the error
		error += dist;
	}

	// save this scheme if it wins
	if( error < m_besterror )
	{
		// remap the indices
		u8 indices[16];
		m_colours->RemapIndices( closest, indices );

		// save the block
		WriteColourBlock3( m_start, m_end, indices, block );

		// save the error
		m_besterror = error;
	}
}

void RangeFit::Compress4( void* block )
{
	// cache some values
	int const count = m_colours->GetCount();
	Vec3 const* values = m_colours->GetPoints();

	// create a codebook
	Vec3 codes[4];
	codes[0] = m_start;
	codes[1] = m_end;
	codes[2] = ( 2.0f/3.0f )*m_start + ( 1.0f/3.0f )*m_end;
	codes[3] = ( 1.0f/3.0f )*m_start + ( 2.0f/3.0f )*m_end;

	// match each point to the closest code
	u8 closest[16];
	float error = 0.0f;
	for( int i = 0; i < count; ++i )
	{
		// find the closest code
		float dist = FLT_MAX;
		int idx = 0;
		for( int j = 0; j < 4; ++j )
		{
			float d = LengthSquared( m_metric*( values[i] - codes[j] ) );
			if( d < dist )
			{
				dist = d;
				idx = j;
			}
		}

		// save the index
		closest[i] = ( u8 )idx;

		// accumulate the error
		error += dist;
	}

	// save this scheme if it wins
	if( error < m_besterror )
	{
		// remap the indices
		u8 indices[16];
		m_colours->RemapIndices( closest, indices );

		// save the block
		WriteColourBlock4( m_start, m_end, indices, block );

		// save the error
		m_besterror = error;
	}
}

//=========================================singlecolourfit=========================================

SingleColourFit::SingleColourFit( ColourSet const* colours, int flags )
	: ColourFit( colours, flags )
{
	// grab the single colour
	Vec3 const* values = m_colours->GetPoints();
	m_colour[0] = ( u8 )FloatToInt( 255.0f*values->X(), 255 );
	m_colour[1] = ( u8 )FloatToInt( 255.0f*values->Y(), 255 );
	m_colour[2] = ( u8 )FloatToInt( 255.0f*values->Z(), 255 );

	// initialise the best error
	m_besterror = INT_MAX;
}

void SingleColourFit::Compress3( void* block )
{
	// build the table of lookups
	SingleColourLookup const* const lookups[] = 
	{
		lookup_5_3, 
		lookup_6_3, 
		lookup_5_3
	};

	// find the best end-points and index
	ComputeEndPoints( lookups );

	// build the block if we win
	if( m_error < m_besterror )
	{
		// remap the indices
		u8 indices[16];
		m_colours->RemapIndices( &m_index, indices );

		// save the block
		WriteColourBlock3( m_start, m_end, indices, block );

		// save the error
		m_besterror = m_error;
	}
}

void SingleColourFit::Compress4( void* block )
{
	// build the table of lookups
	SingleColourLookup const* const lookups[] = 
	{
		lookup_5_4, 
		lookup_6_4, 
		lookup_5_4
	};

	// find the best end-points and index
	ComputeEndPoints( lookups );

	// build the block if we win
	if( m_error < m_besterror )
	{
		// remap the indices
		u8 indices[16];
		m_colours->RemapIndices( &m_index, indices );

		// save the block
		WriteColourBlock4( m_start, m_end, indices, block );

		// save the error
		m_besterror = m_error;
	}
}

void SingleColourFit::ComputeEndPoints( SingleColourLookup const* const* lookups )
{
	// check each index combination (endpoint or intermediate)
	m_error = INT_MAX;
	for( int index = 0; index < 2; ++index )
	{
		// check the error for this codebook index
		SourceBlock const* sources[3];
		int error = 0;
		for( int channel = 0; channel < 3; ++channel )
		{
			// grab the lookup table and index for this channel
			SingleColourLookup const* lookup = lookups[channel];
			int target = m_colour[channel];

			// store a pointer to the source for this channel
			sources[channel] = lookup[target].sources + index;

			// accumulate the error
			int diff = sources[channel]->error;
			error += diff*diff;			
		}

		// keep it if the error is lower
		if( error < m_error )
		{
			m_start = Vec3(
				( float )sources[0]->start/31.0f, 
				( float )sources[1]->start/63.0f, 
				( float )sources[2]->start/31.0f
				);
			m_end = Vec3(
				( float )sources[0]->end/31.0f, 
				( float )sources[1]->end/63.0f, 
				( float )sources[2]->end/31.0f
				);
			m_index = ( u8 )( 2*index );
			m_error = error;
		}
	}
}

}

//=========================================YkMemImage=========================================

namespace Yk {

unsigned int YkMemImage::Encode( const unsigned int pixsize, unsigned int width, unsigned int height,const unsigned char* in,unsigned char *out )
{
	if (in == NULL || out == NULL || width == 0 || height == 0 || pixsize < 3 || pixsize > 4)
	{
		return 0;
	}

	unsigned int size=0;

	if (pixsize == 3)
	{
		squish::CompressImage(in,width,height,out,squish::kDxt1 | squish::kColourRangeFit);
		size = ((width-1)/4+1)*((height-1)/4+1)*8;
	}
	else
	{
		squish::CompressImage(in,width,height,out,squish::kDxt5 | squish::kColourRangeFit);
		size = ((width-1)/4+1)*((height-1)/4+1)*16;
	}
	
	return size;
}

void YkMemImage::Decode( const unsigned int pixsize, unsigned int width, unsigned int height,unsigned char* out,const unsigned char *in )
{
	if (in == NULL || out == NULL || width == 0 || height == 0 || pixsize < 3 || pixsize > 4 )
	{
		return;
	}

	if (pixsize == 3)
	{
		squish::DecompressImage(out,width,height,in,squish::kDxt1 | squish::kColourRangeFit);
	}
	else
	{
		squish::DecompressImage(out,width,height,in,squish::kDxt5 | squish::kColourRangeFit);
	}
}

static void halveImage(int components, int width, int height,
	const unsigned char *datain, unsigned char *dataout)
{
	int i, j, k;
	int newwidth, newheight;
	int delta;
	unsigned char *s;
	const unsigned char *t;

	newwidth = width / 2;
	newheight = height / 2;
	delta = width * components;
	s = dataout;
	t = datain;

	/* Piece o' cake! */
	for (i = 0; i < newheight; i++) {
		for (j = 0; j < newwidth; j++) {
			for (k = 0; k < components; k++) {
				s[0] = (t[0] + t[components] + t[delta] +
					t[delta+components] + 2) / 4;
				s++; t++;
			}
			t += components;
		}
		t += delta;
	}
}

void YkMemImage::Scale(const unsigned int components, unsigned int widthin, unsigned int heightin,unsigned char *in,
				  unsigned int widthout, unsigned int heightout, unsigned char *out)
{
    float x, lowx, highx, convx, halfconvx;
    float y, lowy, highy, convy, halfconvy;
    float xpercent,ypercent;
    float percent;
    /* Max components in a format is 4, so... */
    float totals[4];
    float area;
    int i,j,k,yint,xint,xindex,yindex;
    int temp;

	const unsigned char* datain = (const unsigned char *)in;
	unsigned char* dataout = (unsigned char*)out;

	if (widthin == widthout && heightin == heightout)
	{
		memcpy(dataout,datain,heightin*widthin*components);
		return;
	}
	
	if (widthin == widthout*2 && heightin == heightout*2) {
		halveImage(components, widthin, heightin, datain, dataout);
		return;
	}
    convy = (float) heightin/heightout;
    convx = (float) widthin/widthout;
    halfconvx = convx/2;
    halfconvy = convy/2;
    for (i = 0; i < heightout; i++) {
	y = (float)(convy * (i+0.5));
	if (heightin > heightout) {
	    highy = y + halfconvy;
	    lowy = y - halfconvy;
	} else {
	    highy = y + 0.5;
	    lowy = y - 0.5;
	}
	for (j = 0; j < widthout; j++) {
	    x = convx * (j+0.5);
	    if (widthin > widthout) {
		highx = x + halfconvx;
		lowx = x - halfconvx;
	    } else {
		highx = x + 0.5;
		lowx = x - 0.5;
	    }

	    /*
	    ** Ok, now apply box filter to box that goes from (lowx, lowy)
	    ** to (highx, highy) on input data into this pixel on output
	    ** data.
	    */
	    totals[0] = totals[1] = totals[2] = totals[3] = 0.0;
	    area = 0.0;

	    y = lowy;
	    yint = floor(y);
	    while (y < highy) {
		yindex = (yint + heightin) % heightin;
		if (highy < yint+1) {
		    ypercent = highy - y;
		} else {
		    ypercent = yint+1 - y;
		}

		x = lowx;
		xint = floor(x);

		while (x < highx) {
		    xindex = (xint + widthin) % widthin;
		    if (highx < xint+1) {
			xpercent = highx - x;
		    } else {
			xpercent = xint+1 - x;
		    }

		    percent = xpercent * ypercent;
		    area += percent;
		    temp = (xindex + (yindex * widthin)) * components;
		    for (k = 0; k < components; k++) {
			totals[k] += datain[temp + k] * percent;
		    }

		    xint++;
		    x = xint;
		}
		yint++;
		y = yint;
	    }

	    temp = (j + (i * widthout)) * components;
	    for (k = 0; k < components; k++) {
		/* totals[] should be rounded in the case of enlarging an RGB
		 * ramp when the type is 332 or 4444
		 */
		dataout[temp + k] = (totals[k]+0.5)/area;
	    }
	}
    }
}

}
