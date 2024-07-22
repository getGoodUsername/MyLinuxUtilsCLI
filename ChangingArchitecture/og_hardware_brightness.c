#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

/**
 * og_hardware_brightness is meant to be used with displays whose
 * backlight can be adjusted through software, usually laptops
**/


/**
 * brightness control file should not be changing often. This avoids
 * having to take in input to decide which file to write to which is
 * scary when this is meant to be run as root with set uid.
**/
#ifndef HW_BRIGHTNESS_CTL_FNAME
    #error Please use -D during compilation to define HW_BRIGHTNESS_CTL_FNAME where it evaluates to path for backlight control file
#endif

#ifndef HW_BRIGHTNESS_MAX_INT
    #error Please use -D during compilation to define HW_BRIGHTNESS_MAX_INT where it evaluates int value for max brightness allowed
#endif

#ifndef HW_BRIGHTNESS_MIN_INT
    #error Please use -D during compilation to define HW_BRIGHTNESS_MIN_INT where it evaluates int value for min brightness allowed
#endif

#define STRINGIFY(name) #name

int main(const int argc, const char *const *argv);
bool isFileOnlyControlledByRoot(const char* fname);
FILE* fileOpener(const char* const fname, const char* const mode, int error_code);
int clamp(int value, int min_inclusive, int max_inclusive);

// such a short lil script, will let OS cleanup by itself (files and memory).
// executable is expected to owned by root and have setuid active in file permissions, else can't edit brightness file
int main(const int argc, const char *const *argv)
{
    if (!(argc > 1)) {
        fprintf(stderr, "Usage: %s <BRIGHTNESS INT VALUE>\n", argv[0]);
        return 0;
    }

    const int brightness = clamp(atoi(argv[1]), HW_BRIGHTNESS_MIN_INT, HW_BRIGHTNESS_MAX_INT);
    const char* const brightness_ctl_fname = argv[3];
    FILE* const brightness_ctl_fhandle = fileOpener(STRINGIFY(HW_BRIGHTNESS_CTL_FNAME), "w", 1);
    fprintf(brightness_ctl_fhandle, "%d", brightness);

    return 0;
}

bool isFileOnlyControlledByRoot(const char* fname)
{
    struct stat statbuf;

    if (stat(fname, &statbuf) == -1) {
        fprintf(stderr, "ERROR: %s, stat wasn't able to get info about %s\n", strerror(errno), fname);
        exit(126);
    }

    const bool is_owned_by_root = statbuf.st_uid == 0 && statbuf.st_gid == 0;
    const bool is_r_owner = statbuf.st_mode & S_IRUSR;
    const bool is_r_group = statbuf.st_mode & S_IRGRP;
    const bool is_at_most_r_other = !(statbuf.st_mode & S_IWOTH) && !(statbuf.st_mode & S_IXOTH);

    return is_owned_by_root && is_r_owner && is_r_group && is_at_most_r_other;
}

FILE* fileOpener(const char* const fname, const char* const mode, int error_code)
{
    /** since program is expected to have set uid on, need to be stricter.
     * done to reduce attack vectors and minimize accidental changes of
     * files outside of og_hardware_brightness program don't require write
     * permissions here since files defining max_brightness are often read only
    **/
    if (!isFileOnlyControlledByRoot(fname)) {
        fprintf(stderr,
        "ERROR: %s must meet the following requirements:\n"
            "\towner = root\n"
            "\tgroup = root\n"
            "\towner permissions must include r\n"
            "\tgroup permissions must include r\n"
            "\tother's permissions only have r at most\n"
            "\tEx. -r--r--r-- root root %s\n"
            "\tEx. -rw-r----- root root %s\n",
        fname, fname, fname);
        exit(127);
    }

    FILE* fp = fopen(fname, mode);

    if (fp == NULL) {
        fprintf(stderr, "ERROR: %s, Wasn't able to open: %s\n", strerror(errno), fname);
        exit(error_code);
    }

    return fp;
}


/**
 * @brief clamp value to be within min_inclusive and max_inclusive
 * 
 * @param value
 * @param max_inclusive expected to be greater than or eq to min_inclusive
 * @param min_inclusive expected to be less than or eq to max_inclusive
 * @return int
 */
int clamp(int value, int min_inclusive, int max_inclusive)
{
    value = (value > max_inclusive) ? max_inclusive : value;
    return (value < min_inclusive) ? min_inclusive : value;
}
