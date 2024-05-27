#!/bin/bash

# 获取今天的日期
today=$(date +%Y-%m-%d)

# 获取今天一天内的提交，并统计代码行数
git log --since="$today 00:00:00" --until="$today 23:59:59" --pretty=tformat: --numstat | \
awk '{ added += $1; deleted += $2; } END { print "Added lines:", added, "Deleted lines:", deleted, "Total lines changed:", added + deleted }'
