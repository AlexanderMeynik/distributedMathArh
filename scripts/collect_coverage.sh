#!/bin/bash

set -e
cd ${GITHUB_WORKSPACE}/src
mypath=$(pwd)/*
cd ..
mkdir cov_report && cd cov_report
lcov -t "project_cov" -o project_cov.info -c -d ../$coverageD/
lcov --extract project_cov.info "$mypath" -o project_cov_mycode.info
lcov --remove project_cov_mycode.info '*/test/*' -o project_cov_source.info
genhtml -o report project_cov_source.info


REPORT_DIR="report"
INDEX_HTML="$REPORT_DIR/index.html"


if [ ! -f "$INDEX_HTML" ]; then
    echo "Error: $INDEX_HTML not found."
    exit 1
fi

CONTENT=$(sed -e 's/<[^>]*>//g' "$INDEX_HTML"| tr -s ' '|
sed "s/^//g"| grep -v "^$" | tr '\n' ' ')

COVERAGE_DATA=$(echo "$CONTENT" | grep --line-buffered -oE \
 "Lines:..[0-9]+..[0-9]+..[0-9]+[.][0-9]+.%")

COVERAGE_STR=$(echo "$COVERAGE_DATA" | awk '{print $4" "$5"\t("$3"/"$2")"}')

if [ ! -z "$GITHUB_ACTIONS" ]; then
  echo "COVERAGE_LABEL=$COVERAGE_STR" >> $GITHUB_ENV
  echo "COVERAGE_VALUE=$(echo $COVERAGE_STR | cut -d "%" -f 1)" >> $GITHUB_ENV
fi