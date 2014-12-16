/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <fsl_shbp_aiop.h>
#include <fsl_icontext.h>
#include <fsl_cdma.h>
#include <fsl_spinlock.h>

uint64_t shbp_acquire(struct shbp_aiop *bp)
{
	struct shbp shbp;
	uint32_t offset;
	
	cdma_mutex_lock_take(bp->shbp, CDMA_MUTEX_WRITE_LOCK);
	
	/* TODO maybe read less */
	icontext_dma_read(&bp->ic, sizeof(struct shbp), bp->shbp, &shbp);
	
	/* TODO acquire */
	
	
	offset =  (uint32_t)(&shbp.alloc.deq) - (uint32_t)(&shbp);  
	icontext_dma_write(&bp->ic, sizeof(uint32_t), &shbp.alloc.deq, 
	                   bp->shbp + offset);
	
	cdma_mutex_lock_release(bp->shbp);
	
	return NULL;
}

int shbp_release(struct shbp_aiop *bp, uint64_t buf)
{
	UNUSED(buf);

	cdma_mutex_lock_take(bp->shbp, CDMA_MUTEX_WRITE_LOCK);

	cdma_mutex_lock_release(bp->shbp);
	
	return 0;
}

int shbp_enable(uint16_t swc_id, uint64_t shbp_iova, struct shbp_aiop *bp)
{	
	UNUSED(swc_id);
	UNUSED(shbp_iova);
	
	lock_spinlock(&bp->lock);

	unlock_spinlock(&bp->lock);
	
	return 0;
}