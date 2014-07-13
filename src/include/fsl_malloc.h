/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_malloc.h

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines
*//***************************************************************************/

#ifndef __FSL_MALLOC_H
#define __FSL_MALLOC_H

#include "common/types.h"


/**************************************************************************//**
 @Group         fsl_lib_g   Utility Library 
 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines

 @{
*//***************************************************************************/



/**************************************************************************//**
 @Function      fsl_os_malloc

 @Description   allocates contiguous block of memory.

 @Param[in]     size    Number of bytes to allocate.

 @Return        The address of the newly allocated block on success, NULL on failure.
*//***************************************************************************/
void * fsl_os_malloc(size_t size);

/**************************************************************************//**
 @Function     fsl_os_xmalloc

 @Description   Allocates contiguous block of memory in a specified
                alignment and from the specified segment.

 @Param[in]     size                Number of bytes to allocate.
 @Param[in]     mem_partition_id    Memory partition ID; The value zero must
                                    be mapped to the default heap partition.
 @Param[in]     alignment           Required memory alignment (in bytes).

 @Return        The address of the newly allocated block on success, NULL on failure.
*//***************************************************************************/
void *fsl_os_xmalloc(size_t size, int mem_partition_id, uint32_t alignment);

/**************************************************************************//**
 @Function      fsl_os_xfree

 @Description   Frees the memory block pointed to by "p".
                Only for memory allocated by fsl_os_xmalloc().

 @Param[in]     mem     A pointer to the memory block.
*//***************************************************************************/
void fsl_os_xfree(void *mem);

/**************************************************************************//**
 @Function      fsl_os_free

 @Description   frees the memory block pointed to by "p".

 @Param[in]     mem     A pointer to the memory block.
*//***************************************************************************/
void fsl_os_free(void *mem);

/** @} */ /* end of fsl_lib_g  group */


#endif /* __FSL_MALLOC_H */