#!/usr/bin/bash
function changeBrightness
{
    local -r display="$1"
    local -r brightness_val="$2"
    xrandr --output "$display" --brightness "$brightness_val"
    echo "Set $display to $brightness_val"
}

function getConnectedDisplays
{
    local -r cached_xrandr_verbose_file="$1"
    grep -E '\bconnected\b' "$cached_xrandr_verbose_file" | cut -d' ' -f1
}

function getBrightnessPerDisplay
{
    local -r cached_xrandr_verbose_file="$1"
    grep 'Brightness' "$cached_xrandr_verbose_file" | cut -d' ' -f2
}

function makeSnapshotSoftwareBrightness
{
    local -r cached_xrandr_verbose_file="$1"
    paste -d ' ' <(getConnectedDisplays "$cached_xrandr_verbose_file") <(getBrightnessPerDisplay "$cached_xrandr_verbose_file")
}

function isAlreadyDim
{
    local -r software_brightness_store_file="$1"
    [[ -s "$software_brightness_store_file" ]]
}

function unDim
{
    local -r software_brightness_store_file="$1"
    while read -r line; do
        # shellcheck disable=SC2086
        changeBrightness $line; # want line to be split into two differnet args <display> <brightness_val>
    done < "$software_brightness_store_file"
    truncate --size=0 "$software_brightness_store_file" # clear file to designate not dim yet
}

function doDim
{
    local -r dim_value="$1"
    local -r software_brightness_store_file="$2"
    local -r cached_xrandr_verbose_file="$3"

    makeSnapshotSoftwareBrightness "$cached_xrandr_verbose_file" >| "$software_brightness_store_file"
    for display in $(getConnectedDisplays "$cached_xrandr_verbose_file"); do
        changeBrightness "$display" "$dim_value"
    done
}

function main
{
    dim_value="$1"
    software_brightness_store_file="$2"
    cached_xrandr_verbose_file="$3"

    xrandr --verbose >| "$cached_xrandr_verbose_file" # xrandr takes quite a while to excute, do only once.
    if isAlreadyDim "$software_brightness_store_file"; then
        unDim "$software_brightness_store_file"
    else
        doDim "$dim_value" "$software_brightness_store_file" "$cached_xrandr_verbose_file"
    fi
}

if [[ "$#" -lt 3 ]]; then
    echo "Usage: $0 <SOFTWARE DIM BRIGHTNESS VALUE [0.0 - 1.0]> <STORE CURRENT SOFTWARE BRIGHTNESS FILE> <TEMP BUFFER FILE>"
    exit 1
else
    main "$@"
fi




