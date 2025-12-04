#!/bin/bash

# 清理旧的错误测试文件
rm -rf test_cases_error_handling
echo "🧹 清理了旧的错误处理测试文件..."

mkdir test_cases_error_handling
cd ./test_cases_error_handling

# 用于文件名的计数器
i=0

echo "--- A.  无效字面量 (Invalid Literals) ---" # 001~003

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_error_handling${id}.json << 'EOF'
tru
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_error_handling${id}.json << 'EOF'
fals
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_error_handling${id}.json << 'EOF'
nul
EOF

echo "end = $id"

echo "--- B. 未闭合结构 / 字符串 (Unclosed Structures / Strings) ---" # 004~006

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_error_handling${id}.json << 'EOF'
[1, 2, 3
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_error_handling${id}.json << 'EOF'
{"key": "value"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_error_handling${id}.json << 'EOF'
"hello
EOF

echo "end = $id"

echo "--- C. 无效转义 / Unicode (Invalid Escapes / Unicode) ---" # 007~011

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_error_handling${id}.json << 'EOF'
"\z"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_error_handling${id}.json << 'EOF'
"\u123"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_error_handling${id}.json << 'EOF'
"\uDEFG"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_error_handling${id}.json << 'EOF'
"\uD83D"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_error_handling${id}.json << 'EOF'
"\uD83D\u1234"
EOF

echo "end = $id"

echo "--- D. 结构错误 (Commas, Keys, Colons) ---" # 012~016

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_error_handling${id}.json << 'EOF'
[1 2]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_error_handling${id}.json << 'EOF'
{"a": 1 "b": 2}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_error_handling${id}.json << 'EOF'
{ : "value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_error_handling${id}.json << 'EOF'
{123: "value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_error_handling${id}.json << 'EOF'
{"key" "value"}
EOF

echo "end = $id"

echo "--- E. 多余逗号 / 额外内容 (Trailing Commas / Extra Content) ---" # 017~019

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_error_handling${id}.json << 'EOF'
[1, 2,]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_error_handling${id}.json << 'EOF'
{"a": 1,}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_error_handling${id}.json << 'EOF'
[1, 2] "extra"
EOF

echo "end = $id"

echo "✅ 完成! 创建了 $i 个错误测试文件 (error_handling001.json 到 error_handling${id}.json)."

cd ..