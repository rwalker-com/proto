#!/usr/bin/env bash

find . \! -path '*/.git/*' -a -name \*.h -o -name \*.c -o -name \*.cpp -o -name \*.hpp | while read -r i
do
  [[ $(file -bI "$i") =~ text/.*\;\ charset=us-ascii ]] || continue
  echo $i
  awk '/namespace nl {/ {next};
  /} \/\/ namespace nl/ {next};
  /namespace Weave {/ {$2="chip"; print};
  /} \/\/ namespace Weave/ {$4="chip"; print};
  {print}' < "$i" | sed 's/nl::Weave/chip/g;s/nl:://g' > "$i".deweaved || exit 1
  mv "$i".deweaved "$i"
done

find . \! -path '*/.git/*' -a -type f | while read -r i
do
  [[ $(file -bI "$i") =~ text/.*\;\ charset=us-ascii ]] || continue
  echo $i
  sed 's/WEAVE_/CHIP_/g;s/Weave/chip/g' < "$i" > "$i".deweaved || exit 1
  mv "$i".deweaved "$i"
done
