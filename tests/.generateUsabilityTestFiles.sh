#!/bin/bash

# 清理旧文件
rm -rf ./usability
echo "🧹 清理了旧的测试文件..."

mkdir usability
cd ./usability

# 用于文件名的计数器
i=0

# 定义一个函数或逻辑来更新 i 并生成带补零的 id
# 这里为了保持你原有的脚本结构，我们在每次 increment 后手动格式化

echo "--- .. 空输入 / 结尾无空白字符 (Empty / End without whitespace) ---" # 001~003

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
touch usability${id}.json

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
 
    
EOF

i=$((i+1)); id=$(printf "%03d" $i)
printf "null" > usability${id}.json

echo "end = $id"

echo "--- A. 基本类型 (Primitives) ---" # 004~014

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > usability${id}.json << 'EOF'
null
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
true
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
false
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
0
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
12345
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
-789
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
3.14159
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
-0.001
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
1.0
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
2.5e3
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
1.23E-2
EOF

echo "end = $id"

echo "--- B. 字符串 (Strings) ---" # 015~020

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > usability${id}.json << 'EOF'
""
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
"Hello, world!"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
"\"\\/\b\f\n\r\t"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
"\\u4F60\\u597D"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
"\\uD83D\\uDE00"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
"A string with \"quotes\" and a \n newline."
EOF

echo "end = $id"

echo "--- C. 数组 (Arrays) ---" # 021~025

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > usability${id}.json << 'EOF'
[]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
[1, 2, 3, 4, 5]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
[1, "hello", true, null, 3.14]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
[1, [2, 3], [4, [5]], 6]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
[{"a": 1}, {"b": 2}]
EOF

echo "end = $id"

echo "--- D. 对象 (Objects) ---" # 026~031

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > usability${id}.json << 'EOF'
{}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
{"key": "value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
{
  "string": "test",
  "number": 123,
  "boolean": false,
  "nothing": null
}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
{
  "user": {
    "name": "Test User",
    "id": "abc-123"
  },
  "active": true
}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
{
  "items": [1, 2, 3],
  "status": "ok"
}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
{"key\nwith\\escape": "value"}
EOF

echo "end = $id"

echo "--- E. 空白字符 (Whitespace) ---" # 032~033

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > usability${id}.json << 'EOF'
{ "key" : "value" }
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > usability${id}.json << 'EOF'
[
  {
    "id": 1,
    "name": "Item 1"
  },
  null
]
EOF

echo "end = $id"

echo "✅ 完成! 创建了 $i 个可用性测试文件 (usability001.json 到 usability${id}.json)."

cd ..