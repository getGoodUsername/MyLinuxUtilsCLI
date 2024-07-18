#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

/**
 * og_hardware_dim meant to be used with displays whose backlight can be
 * adjusted through software, usually laptops
 */

int main(const int argc, const char *const *argv);
bool isFileOnlyControlledByRoot(const char* fname);
FILE* fileOpener(const char* const fname, const char* const mode, int error_code);
void changeFileMode(FILE* fhandle, const char* fpath, const char* mode, int error_code);
int clamp(int value, int min_inclusive, int max_inclusive);

// such a short lil script, will let OS cleanup by itself (files and memory).
// executable is expected to owned by root and have setuid active in file permissions, else can't edit brightness file
int main(const int argc, const char *const *argv)
{
    if (!(argc > 4)) {
        fprintf(stderr, "Usage: %s <DIM BRIGHTNESS VALUE> <STORE CURRENT HARDWARE BRIGHTNESS FILE> <HARDWARE BRIGHTNESS CONTROL FILE> <MAX HARDWARE BRIGHTNESS FILE>\n", argv[0]);
        return 0;
    }

    int error_code_num = 1;
    const int user_input_dim_brightness_value = atoi(argv[1]);
    const char* const pre_dimstate_brightness_fname = argv[2];
    const char* const curr_brightness_ctl_fname = argv[3];
    const char* const max_brightness_fname = argv[4];
    FILE* const pre_dimstate_brightness_fhandle = fileOpener(pre_dimstate_brightness_fname, "r", error_code_num++);
    FILE* const curr_brightness_ctl_fhandle = fileOpener(curr_brightness_ctl_fname, "r", error_code_num++);
    FILE* const max_brightness_fhandle = fileOpener(max_brightness_fname, "r", error_code_num++);

    const int MIN_BRIGHTNESS = 1;
    int pre_dimstate_brightness = 1;
    int curr_brightness = 1;
    int MAX_BRIGHTNESS = 1;

    if (fscanf(max_brightness_fhandle, "%d", &MAX_BRIGHTNESS) < 1) {
        fprintf(stderr, "ERROR: Max brightness integer value not found in %s\n", max_brightness_fname);
        exit(4);
    }

    if (fscanf(curr_brightness_ctl_fhandle, "%d", &curr_brightness) < 1) {
        fprintf(stderr, "ERROR: Current brightness integer value not found in %s\n", curr_brightness_ctl_fname);
        exit(5);
    }
    {
        const int old_curr_brightness = curr_brightness;
        curr_brightness = clamp(curr_brightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
        if (curr_brightness != old_curr_brightness) fprintf(stderr, "WARNING: Current brightness value was invalid! Changed to %d. Value was: %d\n", curr_brightness, old_curr_brightness);
    }

    bool is_already_dim = (fscanf(pre_dimstate_brightness_fhandle, "%d", &pre_dimstate_brightness) == 1);
    {
        // just incase user accidentally/maliciously changed value in pre_dimstate_brightness file
        const int old_pre_dimstate_brightness = pre_dimstate_brightness;
        pre_dimstate_brightness = clamp(pre_dimstate_brightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
        if (pre_dimstate_brightness != old_pre_dimstate_brightness) fprintf(stderr, "WARNING: Pre dimstate brightness value was invalid! Changed to %d. Value was: %d\n", pre_dimstate_brightness, old_pre_dimstate_brightness);
    }
    // clear files contents to get ready for replacement values
    changeFileMode(curr_brightness_ctl_fhandle, curr_brightness_ctl_fname, "w", error_code_num++);
    changeFileMode(pre_dimstate_brightness_fhandle, pre_dimstate_brightness_fname, "w", error_code_num++); // pre dimstate file is to be empty when not in dimstate

    if (is_already_dim) {
        fprintf(curr_brightness_ctl_fhandle, "%d", pre_dimstate_brightness);
        printf("Set brightness back to: %d\n", pre_dimstate_brightness);
        return 0;
    }

    const int DIM_BRIGHTNESS_VALUE = clamp(user_input_dim_brightness_value, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    if (user_input_dim_brightness_value != DIM_BRIGHTNESS_VALUE) fprintf(stderr, "WARNING: Input dim brightness value was invalid! Changed to %d. Value was: %d\n", DIM_BRIGHTNESS_VALUE, user_input_dim_brightness_value);

    fprintf(pre_dimstate_brightness_fhandle, "%d", curr_brightness);
    fprintf(curr_brightness_ctl_fhandle, "%d", DIM_BRIGHTNESS_VALUE);
    printf("Dimmed brightness to: %d\n", DIM_BRIGHTNESS_VALUE);
    return 0;
}

bool isFileOnlyControlledByRoot(const char* fname)
{
    struct stat statbuf;

    if (stat(fname, &statbuf) == -1) {
        fprintf(stderr, "ERROR: %s, stat wasn't able to get info about %s\n", strerror(errno), fname);
        exit(127);
    }

    const bool is_owned_by_root = statbuf.st_uid == 0 && statbuf.st_gid == 0;
    const bool is_r_owner = statbuf.st_mode & S_IRUSR;
    const bool is_r_group = statbuf.st_mode & S_IRGRP;
    const bool is_at_most_r_other = !(statbuf.st_mode & S_IWOTH) && !(statbuf.st_mode & S_IXOTH);

    return is_owned_by_root && is_r_owner && is_r_group && is_at_most_r_other;
}

FILE* fileOpener(const char* const fname, const char* const mode, int error_code)
{
    // done to reduce attack vectors and minimize accidental changes of files outside of og_hardware_dim program
    // don't require write permissions here since files defining max_brightness are often read only
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
        fprintf(stderr, "ERROR: Wasn't able to open: %s\n", fname);
        exit(error_code);
    }

    return fp;
}

void changeFileMode(FILE* fhandle, const char* fpath, const char* mode, int error_code)
{
    FILE* fp = freopen(NULL, mode, fhandle);

    if (fp == NULL) {
        fprintf(stderr, "ERROR: %s, Wasn't able to change file mode to '%s' on %s\n", strerror(errno), mode, fpath);
        exit(error_code);
    }
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
