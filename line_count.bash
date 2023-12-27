#!/bin/bash

# 统计.cpp和.h文件的行数
count=0

# 递归函数
function count_lines_recursive {
    local current_dir="$1"

    # 遍历当前目录下的所有文件和文件夹
    for item in "$current_dir"/*; do
        if [ -d "$item" ]; then
            # 如果是文件夹，则递归调用函数
            count_lines_recursive "$item"
        elif [ -f "$item" ]; then
            # 如果是文件，则判断是否为.cpp或.h文件，并执行行数统计
            if [[ "$item" == *.cpp ]] || [[ "$item" == *.h ]]; then
                lines=$(wc -l < "$item")
                count=$((count + lines))
            fi
        fi
    done
}

# 调用递归函数，从当前目录下的src文件夹开始统计
count_lines_recursive "./gwcpp"

echo "lines of code: $count"
