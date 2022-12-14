#include "obj.h"
#include "random.h"
#include "DMA.h"

#include <gba_video.h>
#include <string.h>

#define SPRITE_NUM 128
static obj_t __spr_buffer[SPRITE_NUM];
static bool __spr_free_indexes[SPRITE_NUM];


static uint spr_get_free_index()
{
	for(uint i = 0; i < SPRITE_NUM; i++) {
		if(!__spr_free_indexes[i])
		{
			__spr_free_indexes[i] = true;
			return i;
		}
	}

	return -1;
}


obj_t *spr_alloc(const u16 x, const u16 y, const u16 tile)
{
	uint index = spr_get_free_index();

	obj_t *ptr = &__spr_buffer[index];
	ptr->attr0 = ptr->attr1 = ptr->attr2 = 0;

	spr_move(ptr, x, y);
	spr_set_tile(ptr, tile);
	spr_show(ptr);
	return ptr;
}


void spr_free(obj_t *obj)
{
	spr_hide(obj);
	for(uint i = 0; i < SPRITE_NUM; i++)
	{
		if(obj == &__spr_buffer[i])
		{
			spr_copy(obj, i);
			__spr_free_indexes[i] = false;
			return;
		}
	}
}


void spr_init()
{
	memset(__spr_free_indexes, false, sizeof(__spr_free_indexes));
	REG_DISPCNT |= OBJ_ON | OBJ_1D_MAP;
}


inline void spr_copy(obj_t *obj, const u16 index)
{
	oam_mem[index] = *obj;
}


void spr_copy_all()
{
	volatile obj_t *dst = oam_mem;
	obj_t *src = __spr_buffer;
	bool *isFree = __spr_free_indexes;
	for(uint i = 0; i < SPRITE_NUM; i++)
	{
		if(*isFree)
			*dst = *src;
		
		dst++, src++, isFree++;
	}
}


void spr_copy_all_DMA()
{
	obj_t *dst = (obj_t*)oam_mem;
	obj_t *src = __spr_buffer;
	uint size = SPRITE_NUM << 2;

	dma_copy(DMA_CHANNEL_3, src, dst, size);
}


void spr_move(obj_t *obj, const u16 x, const u16 y)
{
	obj->attr1 &= 0xFE00;
	obj->attr1 |= x & 0x01FF;

	obj->attr0 &= 0xFF00;
	obj->attr0 |= y & 0xFF;
}


inline uint spr_get_x(const obj_t *obj)
{
	return obj->attr1 & 0x01FF;
}


inline uint spr_get_y(const obj_t *obj)
{
	return obj->attr0 & 0x00FF;
}

inline uint spr_get_tile(const obj_t *obj)
{
	return obj->attr2 & 0x01FF;
}


inline void spr_flip(obj_t *obj, const spr_flip_mask mask)
{
	obj->attr1 &= 0xCFFF;
	obj->attr1 |= mask;
}


/**
 * Hides a sprite
 */
inline void spr_hide(obj_t *obj)
{
	obj->attr0 &= 0xFCFF;
	obj->attr0 |= 1 << 0x9;
}


/**
 * Shows a sprite. Default is shown
 */
inline void spr_show(obj_t *obj)
{
	obj->attr0 &= ~(BIT(8)|BIT(9));
}


void spr_set_color_mode(obj_t *obj, const bool is8bpp)
{
	if(!is8bpp)
		obj->attr0 &= 0xDFFF;
	else
		obj->attr0 |= BIT(0xD);
}


void spr_set_priority(obj_t *obj, spr_priority_t priority)
{
	obj->attr2 &= ~SPR_PRIORITY_HIGHEST;
	obj->attr2 |= priority;
}


void spr_set_pal(obj_t *obj, u8 pal)
{
	obj->attr2 &= 0x0FFF;
	obj->attr2 |= (pal & 0xF) << 0xC;
}

inline void spr_set_tile(obj_t *obj, u16 tile)
{
	obj->attr2 &= ~0x01FF;
	obj->attr2 |= tile & 0x01FF;
}


void spr_set_size(obj_t *obj, const spr_size_mask size)
{
	const u16 sh = size >> 2, se = size & 0x3;

	obj->attr0 &= 0x3FF;
	obj->attr0 |= sh << 0xE;
	obj->attr1 &= 0x3FF;
	obj->attr1 |= se << 0xE;
}


spr_size_mask spr_get_size(const obj_t *obj)
{
	return ((obj->attr0 >> 0xE) << 2) | (obj->attr1 >> 0xE);
}


inline void spr_set_gfx_mode(obj_t *obj, spr_gfx_mode_t mode)
{
	obj->attr0 &= 0xF3FF;
	obj->attr0 |= (mode & 0x3) << 0xA;
}

inline void spr_set_affine_status(obj_t *obj, bool affine)
{
	obj->attr0 &= 0xFCFF;
	obj->attr0 |= affine << 0x8;
	obj->attr1 |= BIT(0x9);
}


#define BLEND_CNT			*(vu16*)(0x04000050)	// Alpha control
#define BLEND_ALPHA		    *(vu16*)(0x04000052)	// Fade level
// #define REG_BLDY			*(vu16*)(0x04000054)	// Blend levels


inline void blnd_set(const blnd_cnt_mask_t mask, const blnd_mode_t mode)
{
	BLEND_CNT = mask | mode;
	blnd_set_weights(28, 4);
}


inline void blnd_set_weights(uint top, uint bot)
{
	BLEND_ALPHA = (top & 0x1F) + ((bot & 0x1F) << 8);
}


inline void blnd_set_fade(uint amt)
{
	*(vu16*)(0x04000054) = amt;
}