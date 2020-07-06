#!/bin/bash

find . -name '*.tlsf' > test.index

while IFS= read -r line; do
  name=$(basename -s .tlsf $line)
  result=$(cat $line | grep '^//STATUS' | awk '{printf $3}')
  ../../ssyft-run.sh -tlsf $line . 2> $name.err 1> $name.out
  ssyftresult=$(cat $name.out | grep '^unrealizable')
  ssyftresult=$ssyftresult$(cat $name.out | grep '^realizable')
  ssyftres=${ssyftresult}
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
