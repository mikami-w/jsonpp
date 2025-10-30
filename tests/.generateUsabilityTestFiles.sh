#!/bin/bash

# 清理旧文件
rm -rf ./usability
echo "🧹 清理了旧的测试文件..."

mkdir usability

# 用于文件名的计数器
i=0

echo "--- A. 基本类型 (Primitives) ---" # 1~11

i=$((i+1))
echo "begin = $i"
cat > usability${i}.json << 'EOF'
null
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
true
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
false
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
0
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
12345
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
-789
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
3.14159
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
-0.001
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
1.0
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
2.5e3
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
1.23E-2
EOF

echo "end = $i"

echo "--- B. 字符串 (Strings) ---" # 12~17

i=$((i+1))
echo "begin = $i"
cat > usability${i}.json << 'EOF'
""
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
"Hello, world!"
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
"\"\\/\b\f\n\r\t"
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
"\\u4F60\\u597D"
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
"\\uD83D\\uDE00"
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
"A string with \"quotes\" and a \n newline."
EOF

echo "end = $i"

echo "--- C. 数组 (Arrays) ---" # 18~22

i=$((i+1))
echo "begin = $i"
cat > usability${i}.json << 'EOF'
[]
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
[1, 2, 3, 4, 5]
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
[1, "hello", true, null, 3.14]
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
[1, [2, 3], [4, [5]], 6]
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
[{"a": 1}, {"b": 2}]
EOF

echo "end = $i"

echo "--- D. 对象 (Objects) ---" # 23~28

i=$((i+1))
echo "begin = $i"
cat > usability${i}.json << 'EOF'
{}
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
{"key": "value"}
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
{
  "string": "test",
  "number": 123,
  "boolean": false,
  "nothing": null
}
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
{
  "user": {
    "name": "Test User",
    "id": "abc-123"
  },
  "active": true
}
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
{
  "items": [1, 2, 3],
  "status": "ok"
}
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
{"key\nwith\\escape": "value"}
EOF

echo "end = $i"

echo "--- E. 空白字符 (Whitespace) ---" # 29~30

i=$((i+1))
echo "begin = $i"
cat > usability${i}.json << 'EOF'
{ "key" : "value" }
EOF

i=$((i+1))
cat > usability${i}.json << 'EOF'
[
  {
    "id": 1,
    "name": "Item 1"
  },
  null
]
EOF

echo "end = $i"

echo "✅ 完成! 创建了 $i 个可用性测试文件 (usability1.json 到 usability${i}.json)."