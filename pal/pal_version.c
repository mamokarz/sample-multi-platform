// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.
// See LICENSE file in the project root for full license information.

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define PAL_VERSION_STR STR(PAL_VERSION_MAJOR) "." STR(PAL_VERSION_MINOR) "." STR(PAL_VERSION_PATCH)
#define MAJOR (int)PAL_VERSION_MAJOR
#define MINOR (int)PAL_VERSION_MINOR
#define PATCH (int)PAL_VERSION_PATCH

const char* pal_version_get_string(void)
{
    return PAL_VERSION_STR;
}

const int pal_version_get_major(void)
{
    return MAJOR;
}

const int pal_version_get_minor(void)
{
    return MINOR;
}

const int pal_version_get_patch(void)
{
    return PATCH;
}
