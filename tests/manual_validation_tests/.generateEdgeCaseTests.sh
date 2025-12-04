#!/bin/bash

# 清理旧文件
rm -rf ./test_cases_edge_cases
echo "🧹 清理了旧的边界情况测试文件..."

mkdir test_cases_edge_cases
cd ./test_cases_edge_cases

# 用于文件名的计数器
i=0

echo "--- A. 数字格式错误 (Invalid Number Formats) ---" # 001~010

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_edge_cases${id}.json << 'EOF'
+123
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
.123
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
123.
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
00123
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
-00123
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
1e
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
1e+
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
1.2.3
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
Infinity
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
NaN
EOF

echo "end = $id"

echo "--- B. 字符串错误 (String Errors) ---" # 011~018

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_edge_cases${id}.json << 'EOF'
'single quotes'
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
"unclosed string
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
"line break
in string"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
"\x41"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
"\u"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
"\u12"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
"\uGHIJ"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
"\"
EOF

echo "end = $id"

echo "--- C. 代理对错误 (Surrogate Pair Errors) ---" # 019~025

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_edge_cases${id}.json << 'EOF'
"\uD800"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
"\uDFFF"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
"\uD800\u0041"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
"\uDC00\uDC00"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
"\uD800\uD800"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
"\uDBFF"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
"\uDC00"
EOF

echo "end = $id"

echo "--- D. 结构错误 (Structure Errors) ---" # 026~040

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_edge_cases${id}.json << 'EOF'
{
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
[
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
[}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
[1, 2
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{"key": "value"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
[1, 2, 3]]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{"a": 1}}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
[,1,2,3]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
[1,,2]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{,"key":"value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{"key":}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{:"value"}
EOF

echo "end = $id"

echo "--- E. 对象键错误 (Object Key Errors) ---" # 041~048

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_edge_cases${id}.json << 'EOF'
{123: "value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{true: "value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{null: "value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{[]: "value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{{}: "value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{key: "value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{'key': 'value'}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{"key" = "value"}
EOF

echo "end = $id"

echo "--- F. 冒号与逗号错误 (Colon and Comma Errors) ---" # 049~056

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_edge_cases${id}.json << 'EOF'
{"key" "value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{"key": "value" "key2": "value2"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
[1 2 3]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{"key":: "value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
[1,, 2]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{"key",, "value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
[1; 2; 3]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{"key"; "value"}
EOF

echo "end = $id"

echo "--- G. 多文档/额外内容 (Multiple Documents / Extra Content) ---" # 057~062

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_edge_cases${id}.json << 'EOF'
{} {}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
[] []
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
null null
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
123 456
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
"hello" "world"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
true false
EOF

echo "end = $id"

echo "--- H. 注释 (Comments - should fail) ---" # 063~066

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_edge_cases${id}.json << 'EOF'
// comment
{"key": "value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{"key": "value"} // comment
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
/* comment */
{"key": "value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
{"key": /* comment */ "value"}
EOF

echo "end = $id"

echo "--- I. 特殊值错误 (Special Value Errors) ---" # 067~074

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_edge_cases${id}.json << 'EOF'
True
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
False
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
Null
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
TRUE
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
FALSE
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
NULL
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
undefined
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_edge_cases${id}.json << 'EOF'
None
EOF

echo "end = $id"

echo "--- J. 深度嵌套溢出 (Very Deep Nesting - potential stack overflow) ---" # 075~076

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
# 创建1000层嵌套数组
printf "[" > test_cases_edge_cases${id}.json
for j in {1..1000}; do
    printf "[" >> test_cases_edge_cases${id}.json
done
printf "1" >> test_cases_edge_cases${id}.json
for j in {1..1000}; do
    printf "]" >> test_cases_edge_cases${id}.json
done
printf "]" >> test_cases_edge_cases${id}.json

i=$((i+1)); id=$(printf "%03d" $i)
# 创建1000层嵌套对象
printf "{\"a\":" > test_cases_edge_cases${id}.json
for j in {2..1000}; do
    printf "{\"a$j\":" >> test_cases_edge_cases${id}.json
done
printf "1" >> test_cases_edge_cases${id}.json
for j in {1..1000}; do
    printf "}" >> test_cases_edge_cases${id}.json
done

echo "end = $id"

echo "--- K. 控制字符 (Unescaped Control Characters) ---" # 077~080

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
# 字符串中包含未转义的Tab (应该失败)
printf '"%s"' "$(printf '\t')" > test_cases_edge_cases${id}.json

i=$((i+1)); id=$(printf "%03d" $i)
# 字符串中包含未转义的换行符 (应该失败)
printf '"%s"' "$(printf '\n')" > test_cases_edge_cases${id}.json

i=$((i+1)); id=$(printf "%03d" $i)
# 字符串中包含未转义的回车符 (应该失败)
printf '"%s"' "$(printf '\r')" > test_cases_edge_cases${id}.json

i=$((i+1)); id=$(printf "%03d" $i)
# 字符串中包含未转义的NULL字符
printf '"' > test_cases_edge_cases${id}.json
printf '\x00' >> test_cases_edge_cases${id}.json
printf '"' >> test_cases_edge_cases${id}.json

echo "end = $id"

echo "✅ 完成! 创建了 $i 个边界情况测试文件 (edge_cases001.json 到 edge_cases${id}.json)."

cd ..

