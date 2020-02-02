#!/usr/bin/env bash

usage(){
    echo "
    [sudo] sh $0 <ProjectRoot>

        ProjectRoot: The root directory of the project.
        "
}
exitWhileError(){
    if test $? -ne 0 ; then
        echo "
        Error Message: $1
        Please Check The Error And Test Again! Exit Now.
        " 1>&2
        exit
    fi
}

if test $# -ne 1 ; then
    usage
    exit
fi

ProjectRootDir=$1

SrcDirInProj=(
    "liblua/core"
    "liblua/libraries"
    "cmod/luasocket"
    "cmod/luanet/src"
);

for dir in ${SrcDirInProj[@]}; do
    find ${ProjectRootDir}/${dir} -type f -name "*.c" | sed "/wsocket.c/d" | sed "s#^jni/##"
done
