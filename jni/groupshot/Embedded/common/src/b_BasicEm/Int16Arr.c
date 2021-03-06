/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ---- includes ----------------------------------------------------------- */

#include "b_BasicEm/Functions.h"
#include "b_BasicEm/Int16Arr.h"

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ auxiliary functions } ---------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ constructor / destructor } ----------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

void bbs_Int16Arr_init( struct bbs_Context* cpA,
					    struct bbs_Int16Arr* ptrA )
{
	ptrA->arrPtrE = NULL;
	ptrA->sizeE = 0;
	ptrA->allocatedSizeE = 0;
	ptrA->mspE = NULL;
}

/* ------------------------------------------------------------------------- */

void bbs_Int16Arr_exit( struct bbs_Context* cpA,
					    struct bbs_Int16Arr* ptrA )
{
	bbs_MemSeg_free( cpA, ptrA->mspE, ptrA->arrPtrE );
	ptrA->arrPtrE = NULL;
	ptrA->mspE = NULL;
	ptrA->sizeE = 0;
	ptrA->allocatedSizeE = 0;
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ operators } -------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

void bbs_Int16Arr_copy( struct bbs_Context* cpA,
					    struct bbs_Int16Arr* ptrA, 
						const struct bbs_Int16Arr* srcPtrA )
{
#ifdef DEBUG1
	if( ptrA->allocatedSizeE < srcPtrA->sizeE )
	{
		bbs_ERROR0( "void bbs_Int16Arr_copy(...):\n"
				   "Insufficient allocated memory in destination array." );		
		return;
	}
#endif
	bbs_Int16Arr_size( cpA, ptrA, srcPtrA->sizeE );
	bbs_memcpy16( ptrA->arrPtrE, srcPtrA->arrPtrE, srcPtrA->sizeE * bbs_SIZEOF16( int16 ) ); 
}

/* ------------------------------------------------------------------------- */

flag bbs_Int16Arr_equal( struct bbs_Context* cpA,
						 const struct bbs_Int16Arr* ptrA, 
						 const struct bbs_Int16Arr* srcPtrA )
{
	uint32 iL;
	const int16* ptr1L = ptrA->arrPtrE;
	const int16* ptr2L = srcPtrA->arrPtrE;
	if( ptrA->sizeE != srcPtrA->sizeE ) return FALSE;
	for( iL = ptrA->sizeE; iL > 0; iL-- )
	{
		if( *ptr1L++ != *ptr2L++ ) return FALSE;
	}
	return TRUE;
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ query functions } -------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

uint32 bbs_Int16Arr_heapSize( struct bbs_Context* cpA,
							  const struct bbs_Int16Arr* ptrA, 
							  uint32 sizeA )
{
	return sizeA * bbs_SIZEOF16( int16 ) + bbs_MEM_BLOCK_OVERHD;
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ modify functions } ------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
	
void bbs_Int16Arr_create( struct bbs_Context* cpA,
						  struct bbs_Int16Arr* ptrA, 
						  uint32 sizeA,
						  struct bbs_MemSeg* mspA )
{
	if( bbs_Context_error( cpA ) ) return;
	if( ptrA->sizeE == sizeA ) return;
	if( ptrA->arrPtrE != 0 )
	{
		bbs_Int16Arr_size( cpA, ptrA, sizeA );
	}
	else
	{
		ptrA->arrPtrE = bbs_MemSeg_alloc( cpA, mspA, sizeA * bbs_SIZEOF16( int16 ) );
		if( bbs_Context_error( cpA ) ) return;
		ptrA->allocatedSizeE = sizeA;
		ptrA->sizeE = sizeA;
		if( !mspA->sharedE ) ptrA->mspE = mspA;
	}
}

/* ------------------------------------------------------------------------- */

void bbs_Int16Arr_createAligned( struct bbs_Context* cpA,
								 struct bbs_Int16Arr* ptrA,
								 uint32 sizeA,
								 struct bbs_MemSeg* mspA,
								 struct bbs_Int16Arr* allocPtrA, 
								 uint32 alignBytesA )
{
	/* allocate extra memory for alignment */
	bbs_Int16Arr_create( cpA, allocPtrA, sizeA + ( ( alignBytesA - 1 ) >> 1 ), mspA );

	/* set members of ptrA */
	ptrA->mspE = 0; /* no own allocated memory */
	ptrA->sizeE = sizeA;
	ptrA->allocatedSizeE = ptrA->sizeE;
	ptrA->arrPtrE = allocPtrA->arrPtrE;

#if defined( WIN32 ) || defined( _WIN32_WCE )
	/* disable warning "pointer truncation...": */
	#pragma warning( disable : 4311 )
#endif


	/* align memory */
#ifdef bbs_TYPES_64_AVAILABLE

	while( ( ( ( uint64 ) ptrA->arrPtrE ) & ( alignBytesA - 1 ) ) )
	{
		ptrA->arrPtrE++;
	}	
#else
	while( ( ( ( uint32 ) ptrA->arrPtrE ) & ( alignBytesA - 1 ) ) )
	{
		ptrA->arrPtrE++;
	}	
#endif

}

/* ------------------------------------------------------------------------- */

void bbs_Int16Arr_size( struct bbs_Context* cpA,
					    struct bbs_Int16Arr* ptrA, 
						uint32 sizeA )
{
	if( ptrA->allocatedSizeE < sizeA )
	{
		bbs_ERROR1( "void bbs_Int16Arr_size( struct bbs_Int16Arr*, uint32 sizeA ):\n"
				   "Unsufficient allocated memory (allocatedSizeE = '%i')",
				   ptrA->allocatedSizeE );
		return;
	}
	ptrA->sizeE = sizeA;
}

/* ------------------------------------------------------------------------- */
	
/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ I/O } -------------------------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
	
uint32 bbs_Int16Arr_memSize( struct bbs_Context* cpA,
							 const struct bbs_Int16Arr* ptrA )
{
	return bbs_SIZEOF16( uint32 ) + bbs_SIZEOF16( ptrA->sizeE ) + 
										ptrA->sizeE * bbs_SIZEOF16( int16 );
}

/* ------------------------------------------------------------------------- */
	
uint32 bbs_Int16Arr_memWrite( struct bbs_Context* cpA,
							  const struct bbs_Int16Arr* ptrA, 
							  uint16* memPtrA )
{
	uint32 memSizeL = bbs_Int16Arr_memSize( cpA, ptrA );
	memPtrA += bbs_memWrite32( &memSizeL, memPtrA );
	memPtrA += bbs_memWrite32( &ptrA->sizeE, memPtrA );
	memPtrA += bbs_memWrite16Arr( cpA, ptrA->arrPtrE, ptrA->sizeE, memPtrA );
	return memSizeL;
}

/* ------------------------------------------------------------------------- */
	
uint32 bbs_Int16Arr_memRead( struct bbs_Context* cpA,
							 struct bbs_Int16Arr* ptrA, 
							 const uint16* memPtrA,
							 struct bbs_MemSeg* mspA )
{
	uint32 memSizeL, sizeL;
	if( bbs_Context_error( cpA ) ) return 0;
	memPtrA += bbs_memRead32( &memSizeL, memPtrA );
	memPtrA += bbs_memRead32( &sizeL, memPtrA );
	bbs_Int16Arr_create( cpA, ptrA, sizeL, mspA );
	memPtrA += bbs_memRead16Arr( cpA, ptrA->arrPtrE, ptrA->sizeE, memPtrA );

	if( memSizeL != bbs_Int16Arr_memSize( cpA, ptrA ) )
	{
		bbs_ERR0( bbs_ERR_CORRUPT_DATA, 
				  "uint32 bbs_Int16Arr_memRead( const struct bbs_Int16Arr*, const uint16* ):\n"
                  "size mismatch" ); 
		return 0;
	}
	return memSizeL;
}

/* ------------------------------------------------------------------------- */
	
/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ exec functions } --------------------------------------------- */
/*                                                                           */
/* ========================================================================= */
	
/* ------------------------------------------------------------------------- */

void bbs_Int16Arr_fill( struct bbs_Context* cpA,
					    struct bbs_Int16Arr* ptrA, 
						int16 valA )
{
	uint32 iL;
	for( iL = 0; iL < ptrA->sizeE; iL++ )
	{
		ptrA->arrPtrE[ iL ] = valA;
	}
}

/* ------------------------------------------------------------------------- */

/* ========================================================================= */


