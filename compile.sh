#!/usr/bin/env bash
function LOG_ERROR()
{
    local content=${1}
    echo -e "\033[31m"${content}"\033[0m"
}

function LOG_INFO()
{
    local content=${1}
    echo -e "\033[34m"${content}"\033[0m"
}

function execute_cmd()
{
    local command="${1}"
    eval ${command}
    local ret=$?
    if [ $ret -ne 0 ];then
        LOG_ERROR "execute command ${command} FAILED"
        exit 1
    else
        LOG_INFO "execute command ${command} SUCCESS"
    fi
}


function pre_steps()
{
    execute_cmd "mkdir -p build"
    execute_cmd "rm -rf build/*"
    execute_cmd "rm -rf deps/src/*-stamp"
    execute_cmd "rm -rf deps/src/*-build"
}

pre_steps
if grep -Eqi "Ubuntu" /etc/issue || grep -Eq "Ubuntu" /etc/*-release; then
    cd build && cmake .. && make
else
    cd build && cmake3 .. && make
fi
