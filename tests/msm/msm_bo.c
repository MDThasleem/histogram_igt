// SPDX-License-Identifier: MIT
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include <errno.h>

#include "igt.h"
#include "igt_msm.h"

/*
 * Tests for MSM buffer object allocation and information
 * Tests DRM_IOCTL_MSM_GEM_NEW and DRM_IOCTL_MSM_GEM_INFO functionality
 */

#define DEFAULT_BUFFER_SIZE 4096

int igt_main()
{
	struct msm_device *dev = NULL;

	igt_fixture() {
		dev = igt_msm_dev_open();
	}

	/* Buffer Allocation Tests */
	igt_describe("Test basic buffer object allocation");
	igt_subtest("bo-alloc-basic") {
		struct msm_bo *bo;

		bo = igt_msm_bo_new(dev, DEFAULT_BUFFER_SIZE, MSM_BO_WC);
		igt_assert_f(bo, "Failed to allocate buffer object\n");
		igt_assert_f(bo->handle != 0, "Buffer object handle is 0\n");
		igt_assert_f(bo->size == DEFAULT_BUFFER_SIZE,
			     "Buffer size mismatch: expected %d, got %u\n",
			     (unsigned int)DEFAULT_BUFFER_SIZE, (unsigned int)bo->size);

		igt_msm_bo_free(bo);
	}

	igt_describe("Test write-combine buffer object allocation");
	igt_subtest("bo-alloc-writecombine") {
		struct msm_bo *bo;

		bo = igt_msm_bo_new(dev, DEFAULT_BUFFER_SIZE, MSM_BO_WC);
		igt_assert_f(bo, "Failed to allocate WC buffer object\n");
		igt_assert_f(bo->handle != 0, "Buffer object handle is 0\n");

		igt_msm_bo_free(bo);
	}

	igt_describe("Test uncached buffer object allocation (deprecated flag)");
	igt_subtest("bo-alloc-uncached") {
		struct drm_msm_gem_new req = { 0 };
		int ret;

		req.size = DEFAULT_BUFFER_SIZE;
		req.flags = MSM_BO_UNCACHED;

		ret = igt_ioctl(dev->fd, DRM_IOCTL_MSM_GEM_NEW, &req);
		if (ret == 0) {
			igt_info("MSM_BO_UNCACHED supported on this kernel\n");
			gem_close(dev->fd, req.handle);
		} else {
			igt_assert_eq(errno, EINVAL);
			igt_info("MSM_BO_UNCACHED not supported (deprecated)\n");
		}
	}

	igt_describe("Test allocation with zero size (should fail)");
	igt_subtest("bo-alloc-zero-size") {
		struct drm_msm_gem_new req = { 0 };

		req.size = 0;

		igt_assert_eq(igt_ioctl(dev->fd, DRM_IOCTL_MSM_GEM_NEW, &req), -1);
		igt_assert_eq(errno, EINVAL);
	}

	igt_describe("Test allocation with invalid flags (should fail)");
	igt_subtest("bo-alloc-invalid-flags") {
		struct drm_msm_gem_new req = { 0 };

		req.size = DEFAULT_BUFFER_SIZE;
		req.flags = ~0; /* Invalid flags */

		igt_assert_eq(igt_ioctl(dev->fd, DRM_IOCTL_MSM_GEM_NEW, &req), -1);
		igt_assert_eq(errno, EINVAL);
	}

	igt_describe("Test allocation with unaligned size");
	igt_subtest("bo-alloc-unaligned-size") {
		struct msm_bo *bo;
		size_t size = DEFAULT_BUFFER_SIZE - 3; /* Unaligned size */

		bo = igt_msm_bo_new(dev, size, MSM_BO_WC);
		igt_assert_f(bo, "Failed to allocate buffer with unaligned size\n");
		igt_assert_f(bo->handle != 0, "Buffer object handle is 0\n");

		igt_msm_bo_free(bo);
	}

	igt_fixture() {
		igt_msm_dev_close(dev);
	}
}
