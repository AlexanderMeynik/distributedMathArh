#!/bin/bash

set -e

DOXYFILE="Doxyfile.in"

mkdir -p build/doc
cp -r doc/. build/doc
cd build/doc
doxygen $DOXYFILE

SRC_DIR="../../src"
DOXYGEN_OUTPUT_DIR="doxygen-xml"
DOC_COVERAGE_FILE="doc-coverage.info"

mkdir -p $DOXYGEN_OUTPUT_DIR
python3 -m coverxygen --xml-dir $DOXYGEN_OUTPUT_DIR --src-dir $SRC_DIR --output $DOC_COVERAGE_FILE --format summary

cat $DOC_COVERAGE_FILE

SUMMARY=$(tail -1 $DOC_COVERAGE_FILE | cut -d ":" -f 2 | awk '{$1=$1;print}')


if [ ! -z "$GITHUB_ACTIONS" ]; then
  echo "DOC_COVERAGE_LABEL=$SUMMARY" >> $GITHUB_ENV
  echo "DOC_COVERAGE_VALUE=$(echo $SUMMARY | cut -d "%" -f 1)" >> $GITHUB_ENV
fi
