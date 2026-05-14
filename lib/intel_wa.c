// SPDX-License-Identifier: MIT
/*
 * Copyright © 2025 Intel Corporation
 */

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>

#include "igt_core.h"
#include "igt_debugfs.h"
#include "igt_sysfs.h"
#include "intel_wa.h"
#include "xe/xe_query.h"

static bool debugfs_file_has_wa(int drm_fd, int debugfs_fd,
			       const char *debugfs_name, const char *wa)
{
	char *debugfs_dump;

	igt_assert(igt_debugfs_exists(drm_fd, debugfs_name, O_RDONLY));

	debugfs_dump = igt_sysfs_get(debugfs_fd, debugfs_name);
	if (debugfs_dump) {
		char *has_wa = strstr(debugfs_dump, wa);

		free(debugfs_dump);

		if (has_wa)
			return true;
	}

	return false;
}

/**
 * igt_has_intel_wa:
 * @drm_fd:	A drm file descriptor
 * @check_wa:	Workaround to be checked
 *
 * Returns:	true if WA present, false otherwise
 */
bool igt_has_intel_wa(int drm_fd, const char *check_wa)
{
	bool ret = false;
	int debugfs_fd;
	unsigned int xe;
	char name[256];

	debugfs_fd = igt_debugfs_dir(drm_fd);
	igt_assert(debugfs_fd >= 0);

	xe_for_each_gt(drm_fd, xe) {
		sprintf(name, "gt%d/workarounds", xe);
		ret = debugfs_file_has_wa(drm_fd, debugfs_fd, name, check_wa);
		if (ret)
			break;
	}

	if (!ret)
		ret = debugfs_file_has_wa(drm_fd, debugfs_fd, "workarounds", check_wa);

	close(debugfs_fd);
	return ret;
}
