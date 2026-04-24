/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2026 Google
 *
 * Authors:
 *   Louis Chauvet <louis.chauvet@bootlin.com>
 */

#ifndef UNIGRAF_H
#define UNIGRAF_H

#include <stdbool.h>
#include <stdint.h>

/**
 * unigraf_assert: Helper macro to assert a TSI return value and retrieve a detailed error message.
 * @result: libTSI return value to check
 *
 * This macro checks the return value of a libTSI function call. If the return value indicates an
 * error, it retrieves a detailed error message and asserts with that message.
 * If retrieving the error description fails, it asserts with a generic error message.
 */
#define unigraf_assert(result)									\
({												\
	char msg[256];										\
	TSI_RESULT __r = (result);								\
	if (__r < TSI_SUCCESS) {								\
		TSI_RESULT __r2 = TSI_MISC_GetErrorDescription(__r, msg, sizeof(msg));		\
		if (__r2 < TSI_SUCCESS)								\
			igt_assert_f(false,							\
				     "unigraf error: %d (get error description failed: %d)\n",	\
				     __r, __r2);						\
		else										\
			igt_assert_f(false, "unigraf error: %d (%s)\n", __r, msg);		\
	}											\
	(__r);											\
})

bool unigraf_open_device(int drm_fd);

void unigraf_require_device(int drm_fd);

void unigraf_reset(void);

void unigraf_hpd_deassert(void);

void unigraf_hpd_pulse(int duration);

void unigraf_hpd_assert(void);

#endif // UNIGRAF_H
