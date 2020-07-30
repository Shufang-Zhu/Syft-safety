#!/bin/bash

# help
if [ "$1" = "-h" ] || [ "$1" = "--help" ];then
  echo "ssyft-run.sh [-smv | -tlsf] <filename> <workdir>"
  exit 0
fi

# parameters
if [[ -z "$1" || ("$1" != "-tlsf" && "$1" != "-smv") ]];then
  echo "*** Please specify the input file format with -smv or -tlsf."
  exit 1
fi
if [ -z "$2" ];then
  echo "*** Please specify the input file."
  exit 1
fi
if [ -z "$3" ];then
  echo "*** Please specify the working directory."
  exit 1
fi

# path of THIS SCRIPT
script_path=$(sed 's/ssyft-run.sh//' <<< $0)

# aux files
format=$1
file=$2
workdir=$3
name=$(basename -s .tlsf $(basename -s .smv $file))
ltlfile="$workdir/$name.formula"
monafile="$workdir/$name.mona"
dfafile="$workdir/$name.dfa"
partfile="$workdir/$name.part"
syfco="$script_path/bin/syfco"



if [[ $format == "-smv" ]];then
  # ltlfile (ignoring SMV comments "--")
  cp $file $ltlfile
  sed -i -e '/^--/d' $ltlfile
  
  # partition file
  inputs=$(cat $ltlfile | grep '^INPUT' | sed -e 's/INPUT[[:blank:]]*://g' | sed -e 's/,/ /g')
  outputs=$(cat $ltlfile | grep '^OUTPUT' | sed -e 's/OUTPUT[[:blank:]]*://g' | sed -e 's/,/ /g')
  sed -i -e '/^INPUT/d' $ltlfile
  sed -i -e '/^OUTPUT/d' $ltlfile
elif [[ $format == "-tlsf" ]];then
  # tlsf file
  $syfco -f smv -o $ltlfile $file
  # partition file
  inputs=$($syfco -ins $file | sed -e 's/,/ /g')
  outputs=$($syfco -outs $file | sed -e 's/,/ /g')
  # removing SMV header
  grep -e 'LTLSPEC' $ltlfile | sed 's/LTLSPEC//g' > $ltlfile.aux
  mv $ltlfile.aux $ltlfile
else
  printf "Error: format $format not supported.\n"
  exit 1
fi

# partition file
inputsUpp=${inputs^^}
outputsUpp=${outputs^^}
printf ".inputs: $inputsUpp \n.outputs: $outputsUpp \n" > $partfile

# removing newlines
tr '\n' ' ' < $ltlfile > $name.aux 
mv $name.aux $ltlfile



# solving
$script_path/parser/safe2fol/safe2fol NNF $ltlfile > $monafile
if mona -xwu $monafile > $dfafile ; then
#if cat $dfafile | grep -q 'out of memory\|token too large';then
  $script_path/SSyft $dfafile $partfile 1 2>&1
else
  echo "memout"
  cat $dfafile
fi


# cleaning
rm -f $ltlfile
rm -f $monafile
rm -f $dfafile
rm -f $partfile
