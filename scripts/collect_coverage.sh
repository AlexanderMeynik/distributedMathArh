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