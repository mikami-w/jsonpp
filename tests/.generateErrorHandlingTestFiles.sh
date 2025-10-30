#!/bin/bash

# 清理旧的错误测试文件
rm -rf error_handling
echo "🧹 清理了旧的错误处理测试文件..."

mkdir error_handling

# 用于文件名的计数器
i=0

echo "--- A. 空输入 / 无效字面量 (Empty / Invalid Literals) ---" # 1~5

i=$((i+1))
echo "begin = $i"
cat > error_handling${i}.json << 'EOF'

EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
 
    
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
tru
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
fals
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
nul
EOF

echo "end = $i"

echo "--- B. 未闭合结构 / 字符串 (Unclosed Structures / Strings) ---" # 6~8

i=$((i+1))
echo "begin = $i"
cat > error_handling${i}.json << 'EOF'
[1, 2, 3
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
{"key": "value"
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
"hello
EOF

echo "end = $i"

echo "--- C. 无效转义 / Unicode (Invalid Escapes / Unicode) ---" # 9~13

i=$((i+1))
echo "begin = $i"
cat > error_handling${i}.json << 'EOF'
"\z"
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
"\u123"
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
"\uDEFG"
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
"\uD83D"
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
"\uD83D\u1234"
EOF

echo "end = $i"

echo "--- D. 结构错误 (Commas, Keys, Colons) ---" # 14~18

i=$((i+1))
echo "begin = $i"
cat > error_handling${i}.json << 'EOF'
[1 2]
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
{"a": 1 "b": 2}
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
{ : "value"}
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
{123: "value"}
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
{"key" "value"}
EOF

echo "end = $i"

echo "--- E. 多余逗号 / 额外内容 (Trailing Commas / Extra Content) ---" # 19~21

i=$((i+1))
echo "begin = $i"
cat > error_handling${i}.json << 'EOF'
[1, 2,]
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
{"a": 1,}
EOF

i=$((i+1))
cat > error_handling${i}.json << 'EOF'
[1, 2] "extra"
EOF

echo "end = $i"

echo "✅ 完成! 创建了 $i 个错误测试文件 (error_handling1.json 到 error_handling${i}.json)."