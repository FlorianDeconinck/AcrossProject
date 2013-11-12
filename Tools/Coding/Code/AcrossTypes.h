//----------------------------------------------------------
// Creation date : 05/09/2013
// Author : FD
//----------------------------------------------------------
//Defines types for all plateform to be used engine wide
//----------------------------------------------------------
#ifndef __AT_COMONTYPES_H__
#define __AT_COMONTYPES_H__
//----------------------------------------------------------
#define NULL 0
//----------------------------------------------------------
namespace AT{
	#if 1
		//----------------------------------------------------------
		//  Default typing
		//----------------------------------------------------------
		//Integers
		typedef __int8              I8;
		typedef unsigned __int8     U8;
		typedef __int16             I16;
		typedef unsigned __int16    U16;
		typedef __int32             I32;
		typedef unsigned __int32    U32;
		typedef __int64             I64;
		typedef unsigned __int64    U64;
		//Floating point
		typedef float               I32F;
		typedef double              I64F;
		//SIMD
		//#include <emmintrin.h>
		//typedef __m128i				VI32;
		//typedef __m128d				VI32F;
	#else
		//----------------------------------------------------------
		//  Default typing
		//----------------------------------------------------------
		//Intergers
		typedef char                I8;
		typedef unsigned char       U8;
		typedef int                 I32;
		typedef unsigned int        U32;
		typedef long                I64;
		typedef unsigned long       U64;
		//Floating point
		typedef float               I32F;
		//typedef unsigned float      U32F;
		typedef double              I64F;
		//typedef unsigned double     U64F;
		//SIMD
		//typedef float[4]            VI32F;
		//typedef unsigned float[4]   VU32F;
	#endif
}//namespace AT
//----------------------------------------------------------
// Engine related types
namespace AE{
	typedef AT::U8 grid_tile_t;
}//namespace AE
//----------------------------------------------------------
#endif //__AE_COMONTYPES_H__
//----------------------------------------------------------