#!/bin/bash

BUILD_PATH=$PWD/build;

function usage(){
    printf "Build directory : $BUILD_PATH"
	printf "Usage :\n"
	printf "\t-c,--clear                   : clear the build files;\n"
	printf "\t-u,--uninstall               : uninstall bin;\n"
	printf "\t-r,--rebuild                 : clean all and rebuild;\n"
	printf "\t-d,--debug                   : build (or rebuild) with debug flags;\n"
    printf "\t-v,--verbose                 : verbose build;\n"
    printf "\t-j,--jobs=N                  : [6] number of jobs;\n"
    printf "\t   --doc                     : build documentation;\n"
	printf "\t-h,--help                    : display this message.\n"
}
 
 function build_doc(){
    if [ -d "$BUILD_PATH" ]; then
        cd "$BUILD_PATH";
        make doc;
        cd ..;
    fi
}

function build_uninstall(){
    if [ -d "$BUILD_PATH" ]; then
        cd "$BUILD_PATH";
        make uninstall;
        cd ..;
    fi
}

function build_clear(){
    if [ -d "$BUILD_PATH" ]; then
        rm -rf "$BUILD_PATH";
    fi
}

function build_do(){
#    cd "$BUILD_PATH" && cmake .. && cmake --build . --config Release --target all -- VERBOSE=1 --no-print-directory -j 4 && make install && cd ..
    if $1 ; then
       VERBOSE="VERBOSE=1 --no-print-directory";
    fi
    CONFIG="Release";
    CMAKE_BUILD_TYPE="Release";
    if $2 ; then
       CONFIG="Debug";
       CMAKE_BUILD_TYPE="Debug";
    fi 
    JOBS=$3
    
    if [ ! -d "$BUILD_PATH" ]; then
        mkdir "$BUILD_PATH"
    fi

    cd "$BUILD_PATH" && cmake .. -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE && cmake --build . --config $CONFIG --target all -- $VERBOSE -j $JOBS && make install && cd ..;
}

###### MAIN

OPTS=$( getopt -o hucrvj:d --long uninstall,help,clear,rebuild,verbose,jobs:,debug,doc -- "$@" )

VERBOSE_FLAG=false;
DEBUG_FLAG=false;
JOBS_ARG=6;

DO_UNINSTALL=false;
DO_CLEAR=false;
DO_BUILD=true;
DO_DOC=false;

eval set -- "$OPTS"
while true ; do
    case "$1" in
        -h|--help) 
            usage;
            exit 0;;
    	-u|--uninstall) 
            DO_UNINSTALL=true;
            DO_BUILD=false;
		    shift 1;;
    	-c|--clear) 
            DO_BUILD=false;
            DO_CLEAR=true;
		    shift 1;;
        -r|--rebuild) 
            DO_BUILD=true;
            DO_CLEAR=true;
            DO_DOC=true;
		    shift 1;;
        -v|--verbose) 
            VERBOSE_FLAG=true;
		    shift 1;;
        -d|--debug) 
            DEBUG_FLAG=true;
		    shift 1;;
           --doc) 
            DO_DOC=true;
		    shift 1;;
        -j|--jobs) 
            JOBS_ARG=$2;
		    shift 2;;
        --) shift; break;;
    esac
done

if $DO_UNINSTALL ; then
    build_uninstall;
fi
if $DO_CLEAR ; then
    build_clear;
fi
if $DO_BUILD ; then
    build_do $VERBOSE_FLAG $DEBUG_FLAG $JOBS_ARG;
fi
if $DO_DOC ; then
    build_doc;
fi
exit 0