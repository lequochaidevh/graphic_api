#!/usr/bin/bash

set_default_if_not_exist() {
    local var_name=$1
    local default_val=$2

    if [ -z "${!var_name}" ]; then
        export "$var_name"="$default_val"
        echo "➜ $var_name set to default: ${!var_name}"
    else
        echo "➜ $var_name existing: ${!var_name}"
    fi
}

SCRIPT_PATH=$(readlink -f "$0")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

echo $SCRIPT_DIR

set_default_if_not_exist "WORKING_DIR" "$(ls | grep "001" | head -n 1)"
WORKING_DIR=$(readlink -f "$WORKING_DIR")

cd $WORKING_DIR

    
while [[ $# -gt 0 ]]; do
    case "$1" in
        --project|-p)
            # Push variable to CMAKE
            export SRC_NAME=$2
            echo "Cmake build file: $SRC_NAME"
            shift
            shift
            ;;
        --clear|--clean|-c)
            echo "Action: RM_BUILD=1 ..."
            RM_BUILD=1
            shift
            ;;
        --help|-h)
            echo "Usage: ./build.sh "
            echo "-p [file.cpp]"
            echo "-c (or --clean|--clear) to remove build after run"
            exit 0
            ;;
        *)
            OTHER_ARGS+=("$1")
            shift
            ;;
    esac
done

set_default_if_not_exist "SRC_NAME" $(ls 001*cpp | head -1)

rm -rf build

mkdir build

cd build

cmake ..

make

./excute

if [[ $RM_BUILD -eq 1 ]]; then
    echo "Action: Cleaning... $WORKING_DIR/build"
    rm -rf $WORKING_DIR/build
fi