#! /usr/bin/env bash

usage(){
	echo "
    [sudo] sh $0 <LuaCModName>

        <LuaCModName>: The c module name you want to delete.
		"
}
exitWhileError(){
	if test $? -ne 0 ; then
		echo "
        Error Message: $1
        Please Check The Error And Test Again! Exit Now.
        "
		exit
	fi
}

if test $# -ne 1 ; then
	usage
	exit
fi

cmodName=`echo $1 | sed "s/\/$//"`
# echo ${cmodName}

if ! test -d ${cmodName} ; then
	echo \"${cmodName}\"" module does not exist!!! Exit Now."
	exit
fi

echo "Delete Directories ..."
rm -rf ${cmodName}
exitWhileError "Delete Directories failed"

echo "Delete features from CMakeLists.txt ..."
hostSystem=`uname -s`
if [ $hostSystem == "Darwin" ]; then
	sed -i "" '/add_subdirectory('${cmodName}')/d' CMakeLists.txt
	sed -i "" '/set_property(TARGET '${cmodName}' PROPERTY FOLDER "cmod")/d' CMakeLists.txt
else
	sed -i '/add_subdirectory('${cmodName}')/d' CMakeLists.txt
	sed -i '/set_property(TARGET '${cmodName}' PROPERTY FOLDER "cmod")/d' CMakeLists.txt
fi
exitWhileError "Delete features from CMakeLists.txt failed"

echo "All done."
