// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright (C) 2025 Asahi Linux contributors

#ifndef ASAHI_IOCTL_H
#define ASAHI_IOCTL_H

#include <stddef.h>
#include <stdint.h>

void igt_asahi_get_params(int fd, uint32_t param_group, void *data, size_t size, int err);

#endif /* ASAHI_IOCTL_H */
