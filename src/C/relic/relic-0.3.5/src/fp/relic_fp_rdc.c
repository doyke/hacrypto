/*
 * RELIC is an Efficient LIbrary for Cryptography
 * Copyright (C) 2007-2013 RELIC Authors
 *
 * This file is part of RELIC. RELIC is legal property of its developers,
 * whose names are not listed here. Please refer to the COPYRIGHT file
 * for contact information.
 *
 * RELIC is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * RELIC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with RELIC. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 *
 * Implementation of the prime field modular reduction functions.
 *
 * @version $Id: relic_fp_rdc.c 1557 2013-08-30 06:31:37Z dfaranha $
 * @ingroup fp
 */

#include "relic_core.h"
#include "relic_bn_low.h"
#include "relic_fp_low.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

#if FP_RDC == BASIC || !defined(STRIP)

void fp_rdc_basic(fp_t c, dv_t a) {
	dv_t t0, t1, t2, t3;

	dv_null(t0);
	dv_null(t1);
	dv_null(t2);
	dv_null(t3);

	TRY {
		dv_new(t0);
		dv_new(t1);
		dv_new(t2);
		dv_new(t3);

		dv_copy(t2, a, 2 * FP_DIGS);
		dv_copy(t3, fp_prime_get(), FP_DIGS);
		bn_divn_low(t0, t1, t2, 2 * FP_DIGS, t3, FP_DIGS);
		fp_copy(c, t1);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		dv_free(t0);
		dv_free(t1);
		dv_free(t2);
		dv_free(t3);
	}
}

#endif

#if FP_RDC == MONTY || !defined(STRIP)

#if FP_MUL == BASIC || !defined(STRIP)

void fp_rdc_monty_basic(fp_t c, dv_t a) {
	int i;
	dig_t r, c0, c1, *tmp, u0;
	const dig_t *p = NULL;

	tmp = a;

	u0 = *(fp_prime_get_rdc());
	p = fp_prime_get();

	c1 = 0;
	for (i = 0; i < FP_DIGS; i++, tmp++) {
		r = (dig_t)(*tmp * u0);
		c0 = fp_mula_low(tmp, fp_prime_get(), r);
		/* We must use this because the size (FP_DIGS - i) is variable. */
		c1 += bn_add1_low(tmp + FP_DIGS, tmp + FP_DIGS, c0, FP_DIGS - i);
	}
	fp_copy(c, a + FP_DIGS);

	for (i = 0; i < c1; i++) {
		fp_subn_low(c, c, p);
	}
	if (fp_cmpn_low(c, p) != CMP_LT) {
		fp_subn_low(c, c, p);
	}
}

#endif

#if FP_MUL == COMBA || FP_MUL == INTEG || !defined(STRIP)

void fp_rdc_monty_comba(fp_t c, dv_t a) {
	fp_rdcn_low(c, a);
}

#endif

#endif

#if FP_RDC == QUICK || !defined(STRIP)

void fp_rdc_quick(fp_t c, dv_t a) {
	fp_rdcs_low(c, a, fp_prime_get());
}

#endif
