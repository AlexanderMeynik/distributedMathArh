#!/bin/bash

set -e

source /.venv/bin/activate
cd ${GITHUB_WORKSPACE}/src

mkdir metrics && cd metrics

metrix++ collect --db-file=source.db --exclude-files ".*test.*" \
--std.general.size --std.code.length.total --std.code.filelines.total --std.code.lines.total \
--std.code.filelines.code --std.code.lines.code --std.code.filelines.preprocessor --std.code.lines.preprocessor \
--std.code.filelines.comments --std.code.lines.comments --std.code.ratio.comments \
--std.code.complexity.cyclomatic --std.code.complexity.cyclomatic_switch_case_once --std.code.complexity.maxindent \
--std.code.magic.numbers --std.code.todo.comments --std.code.todo.strings --std.code.todo.tags=todo,TODO,ToDo,FIXME,FixMe,TBD,HACK,XXX  \
--std.general.proctime --std.general.procerrors --std.code.maintindex.simple \
--std.code.longlines --std.code.member.fields --std.code.member.classes --std.code.member.structs --std.code.member.types \
-- ../computationalLib/ ../computationalNode/ ../mainNode/ ../utilLib/ ../client

metrix++ collect --db-file=wtests.db \
--std.general.size --std.code.length.total --std.code.filelines.total --std.code.lines.total \
--std.code.filelines.code --std.code.lines.code --std.code.filelines.preprocessor --std.code.lines.preprocessor \
--std.code.filelines.comments --std.code.lines.comments --std.code.ratio.comments \
--std.code.complexity.cyclomatic --std.code.complexity.cyclomatic_switch_case_once --std.code.complexity.maxindent \
--std.code.magic.numbers --std.code.todo.comments --std.code.todo.strings --std.code.todo.tags=todo,TODO,ToDo,FIXME,FixMe,TBD,HACK,XXX  \
--std.general.proctime --std.general.procerrors --std.code.maintindex.simple \
--std.code.longlines --std.code.member.fields --std.code.member.classes --std.code.member.structs --std.code.member.types \
-- ../computationalLib/ ../computationalNode/ ../mainNode/ ../utilLib/ ../client


metrix++ view --db-file=source.db > source.txt
metrix++ view --db-file=wtests.db > wtests.txt
metrix++ view --db-file=source.db --db-file-prev=wtests.db > sourceTestDiff.txt

metrix++ export --db-file=source.db > source.csv
metrix++ export --db-file=wtests.db > wtests.csv

metrix++ report --db-file=source.db --format=txt \
--max-limit=std.code.complexity:cyclomatic:6 --max-limit=std.code.filelines:total:600 \
--max-limit=std.code.magic:numbers:6 --max-limit=std.code.todo:comments:2 > source_report.txt

metrix++ report --db-file=wtests.db --format=txt \
--max-limit=std.code.complexity:cyclomatic:6     --max-limit=std.code.filelines:total:600 \
--max-limit=std.code.magic:numbers:6 --max-limit=std.code.todo:comments:2 > wtests_report.txt


metrix++ report --db-file=source.db --format=json \
--max-limit=std.code.complexity:cyclomatic:6 --max-limit=std.code.filelines:total:600 \
--max-limit=std.code.magic:numbers:6 --max-limit=std.code.todo:comments:2 > source_report.json
metrix++ report --db-file=wtests.db --format=json \
--max-limit=std.code.complexity:cyclomatic:6 --max-limit=std.code.filelines:total:600 \
--max-limit=std.code.magic:numbers:6 --max-limit=std.code.todo:comments:2 > wtests_report.json

metrix++ report --db-file=source.db --format=doxygen \
--max-limit=std.code.complexity:cyclomatic:6 --max-limit=std.code.filelines:total:600 \
--max-limit=std.code.magic:numbers:6 --max-limit=std.code.todo:comments:2 > source_report.dox
metrix++ report --db-file=wtests.db --format=doxygen \
--max-limit=std.code.complexity:cyclomatic:6 --max-limit=std.code.filelines:total:600 \
--max-limit=std.code.magic:numbers:6 --max-limit=std.code.todo:comments:2 > wtests_report.dox