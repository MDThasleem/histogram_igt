// SPDX-License-Identifier: MIT
/*
 * Copyright © 2026 Google
 *
 * Authors:
 *   Louis Chauvet <louis.chauvet@bootlin.com>
 */

#include <stdint.h>

#include "igt_core.h"

#include "unigraf.h"
#include "TSI.h"
#include "TSI_types.h"
#include "igt_rc.h"

#define unigraf_debug(fmt, ...)	igt_debug("TSI:%p: " fmt, unigraf_device, ##__VA_ARGS__)

static TSI_HANDLE unigraf_device;
static char *unigraf_default_edid;
static char *unigraf_connector_name;

/**
 * UNIGRAF_NAME_MAX - Maximum name length to be used for TSI functions
 */
#define UNIGRAF_NAME_MAX 1024

/**
 * UNIGRAF_CONFIG_GROUP - Name of the unigraf group in the configuration file
 */
#define UNIGRAF_CONFIG_GROUP "Unigraf"

/**
 * UNIGRAF_CONFIG_DEVICE_NAME - Key of the device name in the configuration file
 */
#define UNIGRAF_CONFIG_DEVICE_NAME "Device"

static void unigraf_close_device(void)
{
	if (!unigraf_device)
		return;

	unigraf_debug("Closing...\n");
	unigraf_assert(TSIX_DEV_CloseDevice(unigraf_device));
	TSI_Clean();
	unigraf_device = NULL;
	free(unigraf_default_edid);
	free(unigraf_connector_name);
}

/**
 * unigraf_exit_handler - Handle the exit signal and clean up unigraf resources.
 * @sig: The signal number received.
 *
 * This function is called when the program receives an exit signal. It ensures
 * that all unigraf resources are properly cleaned up by calling unigraf_deinit
 * for each open instance.
 */
static void unigraf_exit_handler(int sig)
{
	unigraf_close_device();
}

static void unigraf_init(void)
{
	int ret;

	unigraf_debug("Initialize unigraf...\n");
	ret = TSI_Init(TSI_CURRENT_VERSION);
	unigraf_assert(ret);
	igt_install_exit_handler(unigraf_exit_handler);
}

/**
 * unigraf_device_count() - Return the number of scanned devices
 *
 * Must be called after a unigraf_rescan_devices().
 */
static unsigned int unigraf_device_count(void)
{
	return unigraf_assert(TSIX_DEV_GetDeviceCount());
}

static int unigraf_find_device(char *request)
{
	int device_count = unigraf_device_count();

	for (int i = 0; i < device_count; i++) {
		char dev_name[UNIGRAF_NAME_MAX] = "";

		unigraf_assert(TSIX_DEV_GetDeviceName(i, dev_name, UNIGRAF_NAME_MAX));
		unigraf_debug("Detected unigraf device %d: %s\n", i, dev_name);
		if (!strncmp(dev_name, request, UNIGRAF_NAME_MAX))
			return i;
	}
	return -ENODEV;
}

/**
 * unigraf_open_device() - Search and open a device.
 * @drm_fd: File descriptor of the currently used drm device
 *
 * Assert if:
 * - called outside tests / fixup
 * - TSI library raise errors
 * Returns: true if a device was found and initialized, otherwise false.
 *
 * This function searches for a compatible device and opens it.
 */
bool unigraf_open_device(int drm_fd)
{
	TSI_RESULT r;
	GError *cfg_error = NULL;
	char *cfg_device = NULL;
	int device_count;
	int chosen_device = 0;
	int chosen_role = 0;
	int chosen_input = 0;

	assert(igt_can_fail());

	if (unigraf_device)
		return true;

	unigraf_init();

	if (igt_key_file) {
		cfg_device = g_key_file_get_string(igt_key_file, UNIGRAF_CONFIG_GROUP,
						   UNIGRAF_CONFIG_DEVICE_NAME, &cfg_error);
		if (cfg_error) {
			unigraf_debug("No device name configured, uses first device available.\n");
			cfg_device = NULL;
		}
	}

	unigraf_assert(TSIX_DEV_RescanDevices(0, TSI_DEVCAP_VIDEO_CAPTURE, 0));

	device_count = unigraf_device_count();
	if (device_count < 1) {
		unigraf_debug("No device found.\n");
		return false;
	}

	if (cfg_device) {
		chosen_device = unigraf_find_device(cfg_device);
		if (chosen_device < 0) {
			igt_warn("The requested unigraf device %s is not found, err: %d.\n", cfg_device, chosen_device);
			return false;
		}
	}

	unigraf_device = TSIX_DEV_OpenDevice(chosen_device, &r);
	unigraf_assert(r);
	igt_assert(unigraf_device);
	unigraf_debug("Successfully opened the unigraf device %d.\n", chosen_device);

	unigraf_assert(TSIX_DEV_SelectRole(unigraf_device, chosen_role));
	unigraf_assert(TSIX_VIN_Select(unigraf_device, chosen_input));
	unigraf_assert(TSIX_VIN_Enable(unigraf_device, chosen_input));

	return true;
}

/**
 * unigraf_require_device() - Search and open a device.
 * @drm_fd: File descriptor of the currently used drm device
 *
 * This is a shorthand to reduce test boilerplate when a unigraf device must be present.
 */
void unigraf_require_device(int drm_fd)
{
	igt_require(unigraf_open_device(drm_fd));
}
