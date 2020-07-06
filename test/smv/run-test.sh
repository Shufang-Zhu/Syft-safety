#!/bin/bash

find . -name '*.smv' > test.index

while IFS= read -r line; do
  name=$(basename -s .smv $line)
  result=$(cat $line | grep '^--' | awk '{printf $2}')
  ../../ssyft-run.sh -smv $line . 2> $name.err 1> $name.out
  ssyftresult=$(cat $name.out | grep '^unrealizable')
  ssyftresult=$ssyftresult$(cat $name.out | grep '^realizable')
  ssyftres=${ssyftresult^^}
  # check the results
  printf "$line\t"
  if [[ $result == $ssyftres ]];then
    printf "ok\n"
  else
    printf "WRONG (see $name.[err|out])\n"
  fi
done < test.index


# cleaning
rm -f test.index
