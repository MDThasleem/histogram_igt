/* SPDX-License-Identifier: MIT */

#ifndef _I915_DP_H_
#define _I915_DP_H_

#include "igt_kms.h"

int igt_get_current_link_rate(int drm_fd, igt_output_t *output);
int igt_get_current_lane_count(int drm_fd, igt_output_t *output);
int igt_get_max_link_rate(int drm_fd, igt_output_t *output);
int igt_get_max_lane_count(int drm_fd, igt_output_t *output);
void igt_force_link_retrain(int drm_fd, igt_output_t *output, int retrain_count);
void igt_force_lt_failure(int drm_fd, igt_output_t *output, int failure_count);
bool igt_get_dp_link_retrain_disabled(int drm_fd, igt_output_t *output);
bool igt_has_force_link_training_failure_debugfs(int drmfd, igt_output_t *output);
int igt_get_dp_pending_lt_failures(int drm_fd, igt_output_t *output);
int igt_get_dp_pending_retrain(int drm_fd, igt_output_t *output);
void igt_reset_link_params(int drm_fd, igt_output_t *output);
void igt_set_link_params(int drm_fd, igt_output_t *output,
			 char *link_rate, char *lane_count);

#endif
