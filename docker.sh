#!/bin/bash
function check_install() {
    which $1 > /dev/null
    if [ $? -ne 0 ]; then
        echo Require command : $1
        exit 1
    fi
}
function assert_zero() {
    if [ $1 -ne 0 ]; then
        echo $2
        exit 1
    fi
} 

check_install cmake
check_install make

rl=`readlink -f $0`
scriptdir=`dirname $rl`
dockercmd="docker run -v $scriptdir/vol:/vol --rm canreplay"

if [ "$1" == "build" -o "$1" == "b" ]; then 
    cd $scriptdir
    mkdir -p ./vol
    mkdir -p ./build
    cd build
    cmake ../canreplay
    make
    assert_zero $? "exit"
    cd $scriptdir
    
    IMAGE_NAME=canreplay
    if [ ! -z "$(docker images | grep $IMAGE_NAME)" ]; then
        docker image rm $IMAGE_NAME
        assert_zero $? "exit"
    fi
    docker build -t $IMAGE_NAME .
    assert_zero $? "exit"
elif [ "$1" == "interactive" -o "$1" == "i" ]; then
    docker run -it -v `pwd`/vol:/vol --rm canreplay /bin/bash
elif [ "$1" == "export" -o "$1" == "e" ]; then
    if [ "$2" == "json" ]; then
        $dockercmd python3 /code/pcapparser/app.py $3
    elif [ "$2" == "csv" ]; then
        $dockercmd python3 /code/pcapparser/app.py $3
    elif [ "$2" == "replay" ]; then
        $dockercmd python3 /code/binarization/app.py $3
    fi
elif [ "$1" == "replay" -o "$1" == "r" ]; then
    $dockercmd /code/canreplay
else 
    echo -e "Using: docker.sh <option>"
    echo -e "\tbuild\tbuild docker image"
    echo -e "\texport\t"
    echo -e "\tinteractive\t"
fi

