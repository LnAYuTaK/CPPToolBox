#!/bin/bash

# 递归遍历文件夹下所有.cpp和.h文件，并使用clang-format进行格式化

# 定义要格式化的文件类型
file_types=("*.cpp" "*.h")

# 定义要格式化的文件夹路径
folder_path="./src"

# 遍历文件类型
for file_type in "${file_types[@]}"; do
    # 查找匹配的文件
    files=$(find "${folder_path}" -type f -name "${file_type}")
    
    # 遍历匹配的文件
    for file in ${files}; do
        echo "Formatting ${file}..."
        clang-format -style=file -i "${file}" # 使用clang-format进行格式化
    done
done

echo "Formatting completed."
