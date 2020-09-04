#!/bin/bash

die() {
  echo $0: "$@" >&2
  exit 1
}

usage() {
  cat <<END >&2
USAGE
 ssyft.sh [-smv | -tlsf] [--only-parse] <filename>

OPTIONS
  -smv          SMV file format
  -tlsf         TLSF file format 
  --only-parse  Invocation of parser only
END
}

format=
if [ "$1" = "-smv" ];then
  format="-smv"
elif [ "$1" = "-tlsf" ];then
  format="-tlsf"
else
  usage
  die "Wrong format."
fi

only_parse=""
if [ "$2" == "--only-parse" ];then 
  only_parse="true"
  shift
fi
[ -f "$2" ] || die "Unable to open '$1': No such file or directory"

docker --version > /dev/null || die "Unable to find \`docker'"

docker run --rm -it -v $(pwd):$(pwd) -w $(pwd) \
  lucageatti/ssyft $format $only_parse $2 /tmp
