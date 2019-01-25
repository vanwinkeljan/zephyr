/*
 * Copyright (c) 2019 Jan Van Winkel (jan.van_winkel@dxplore.eu)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <shell/shell.h>

static int cmd_ls(const struct shell *shell, size_t argc, char **argv)
{
	return 0;
}

SHELL_CREATE_STATIC_SUBCMD_SET(sub_settings)
{
	SHELL_CMD(ls, NULL, "List settings", cmd_ls),
	SHELL_SUBCMD_SET_END
};

SHELL_CMD_REGISTER(settings, &sub_settings, "Settings commands", NULL);
