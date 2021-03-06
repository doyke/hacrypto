/* base64-encode.c

   Copyright (C) 2002 Niels Möller

   This file is part of GNU Nettle.

   GNU Nettle is free software: you can redistribute it and/or
   modify it under the terms of either:

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at your
       option) any later version.

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at your
       option) any later version.

   or both in parallel, as here.

   GNU Nettle is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see http://www.gnu.org/licenses/.
*/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <assert.h>
#include <stdlib.h>

#include "base64.h"

static const uint8_t encode_table[64] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz"
  "0123456789+/";

#define ENCODE(x) (encode_table[0x3F & (x)])

void
base64_encode_raw(uint8_t *dst, size_t length, const uint8_t *src)
{
  const uint8_t *in = src + length;
  uint8_t *out = dst + BASE64_ENCODE_RAW_LENGTH(length);

  unsigned left_over = length % 3;

  if (left_over)
    {
      in -= left_over;
      *--out = '=';
      switch(left_over)
	{
	case 1:
	  *--out = '=';
	  *--out = ENCODE(in[0] << 4);
	  break;
	  
	case 2:
	  *--out = ENCODE( in[1] << 2);
	  *--out = ENCODE((in[0] << 4) | (in[1] >> 4));
	  break;

	default:
	  abort();
	}
      *--out = ENCODE(in[0] >> 2);
    }
  
  while (in > src)
    {
      in -= 3;
      *--out = ENCODE( in[2]);
      *--out = ENCODE((in[1] << 2) | (in[2] >> 6));
      *--out = ENCODE((in[0] << 4) | (in[1] >> 4));
      *--out = ENCODE( in[0] >> 2);      
    }
  assert(in == src);
  assert(out == dst);
}

#if 0
unsigned 
base64_encode(uint8_t *dst,
	      unsigned src_length,
	      const uint8_t *src)
{
  unsigned dst_length = BASE64_ENCODE_RAW_LENGTH(src_length);
  unsigned n = src_length / 3;
  unsigned left_over  = src_length % 3;
  unsigned done = 0;
  
  if (left_over)
    {
      const uint8_t *in = src + n * 3;
      uint8_t *out = dst + dst_length;

      switch(left_over)
	{
	case 1:
	  *--out = '=';
	  *--out = ENCODE(in[0] << 4);
	  break;
	  
	case 2:
	  *--out = ENCODE( in[1] << 2);
	  *--out = ENCODE((in[0] << 4) | (in[1] >> 4));
	  break;

	default:
	  abort();
	}
      *--out = ENCODE(in[0] >> 2);

      done = 4;
    }
  base64_encode_raw(n, dst, src);
  done += n * 4;

  assert(done == dst_length);

  return done;
}
#endif

void
base64_encode_group(uint8_t *dst, uint32_t group)
{
  *dst++ = ENCODE(group >> 18);
  *dst++ = ENCODE(group >> 12);
  *dst++ = ENCODE(group >> 6);
  *dst++ = ENCODE(group);
}

void
base64_encode_init(struct base64_encode_ctx *ctx)
{
  ctx->word = ctx->bits = 0;
}

/* Encodes a single byte. */
size_t
base64_encode_single(struct base64_encode_ctx *ctx,
		     uint8_t *dst,
		     uint8_t src)
{
  unsigned done = 0;
  unsigned word = ctx->word << 8 | src;
  unsigned bits = ctx->bits + 8;
  
  while (bits >= 6)
    {
      bits -= 6;
      dst[done++] = ENCODE(word >> bits);
    }

  ctx->bits = bits;
  ctx->word = word;

  assert(done <= 2);
  
  return done;
}

/* Returns the number of output characters. DST should point to an
 * area of size at least BASE64_ENCODE_LENGTH(length). */
size_t
base64_encode_update(struct base64_encode_ctx *ctx,
		     uint8_t *dst,
		     size_t length,
		     const uint8_t *src)
{
  size_t done = 0;
  size_t left = length;
  unsigned left_over;
  size_t bulk;
  
  while (ctx->bits && left)
    {
      left--;
      done += base64_encode_single(ctx, dst + done, *src++);
    }
  
  left_over = left % 3;
  bulk = left - left_over;
  
  if (bulk)
    {
      assert(!(bulk % 3));
      
      base64_encode_raw(dst + done, bulk, src);
      done += BASE64_ENCODE_RAW_LENGTH(bulk);
      src += bulk;
      left = left_over;
    }

  while (left)
    {
      left--;
      done += base64_encode_single(ctx, dst + done, *src++);
    }

  assert(done <= BASE64_ENCODE_LENGTH(length));

  return done;
}

/* DST should point to an area of size at least
 * BASE64_ENCODE_FINAL_SIZE */
size_t
base64_encode_final(struct base64_encode_ctx *ctx,
		    uint8_t *dst)
{
  unsigned done = 0;
  unsigned bits = ctx->bits;
  
  if (bits)
    {
      dst[done++] = ENCODE(ctx->word << (6 - ctx->bits));
      for (; bits < 6; bits += 2)
	dst[done++] = '=';

      ctx->bits = 0;
    }

  assert(done <= BASE64_ENCODE_FINAL_LENGTH);
  return done;
}
