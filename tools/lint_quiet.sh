#!/bin/bash

function search_lint() {
    cd $2
    for file in `\find . -maxdepth 1 -name '*.h'`; do
        $1 $3/tools/cpplint/cpplint.py --quiet $file > /dev/null
    done
    for file in `\find . -maxdepth 1 -name '*.cpp'`; do
        $1 $3/tools/cpplint/cpplint.py --quiet $file > /dev/null
    done
    cd $3
}


py_exe="python"
if [ `which python3` ]; then
  py_exe="python3"
fi

cd `git rev-parse --show-toplevel`
search_lint $py_exe include/cfd ../..
search_lint $py_exe src ..
