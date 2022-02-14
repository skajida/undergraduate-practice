#!/usr/bin/env bash

if [ $# -eq 0 ]; then
    printf "\033[1;31mfilename expected as argument\n"
    exit 1
fi

folder=$(dirname ${1})
filename=$(basename ${1})
extension="${filename##*.}"
filename="${filename%.*}"

cd $(dirname $0)
mkdir -p build
if pdflatex -output-directory=build ${folder}/${filename}.${extension} && \
   pdflatex -output-directory=build ${folder}/${filename}.${extension} && \
   mv build/${filename}.pdf ${filename}.pdf; then
    printf "\033[1;35mpdf report successfully moved to the project root directory\n"
else
    printf "\033[1;31mpdf report couldn't be moved to the project root directory: no such file\n"
fi
