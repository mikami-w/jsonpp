#!/bin/bash

# 清理旧文件
rm -rf ./test_cases_advanced_parsing
echo "🧹 清理了旧的高级解析测试文件..."

mkdir test_cases_advanced_parsing
cd ./test_cases_advanced_parsing

# 用于文件名的计数器
i=0

echo "--- A. 数字边界测试 (Number Boundary Tests) ---" # 001~010

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_advanced_parsing${id}.json << 'EOF'
9223372036854775807
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
-9223372036854775808
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
1.7976931348623157e308
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
2.2250738585072014e-308
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
0.0
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
-0.0
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
1e308
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
-1e308
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
1.23456789012345678901234567890
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
0.000000000000001
EOF

echo "end = $id"

echo "--- B. 深层嵌套 (Deep Nesting) ---" # 011~015

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_advanced_parsing${id}.json << 'EOF'
[[[[[[[[[[1]]]]]]]]]]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
{"a":{"b":{"c":{"d":{"e":{"f":{"g":{"h":{"i":{"j":"deep"}}}}}}}}}}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
[{"a":[{"b":[{"c":[{"d":[1,2,3]}]}]}]}]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
{"level1":{"level2":{"level3":{"level4":{"level5":{"level6":{"level7":{"level8":{"level9":{"level10":"invalid"}}}}}}}}}}}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
[[[[[[[[[[[[[[[[[[[["very deep but invalid"]]]]]]]]]]]]]]]]]]]
EOF

echo "end = $id"

echo "--- C. Unicode 与特殊字符 (Unicode and Special Characters) ---" # 016~025

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"Hello\u0020World"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"\u0041\u0042\u0043"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"\u4e2d\u6587\u6d4b\u8bd5"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"\uD834\uDD1E"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"\uD83D\uDC4D\uD83D\uDE00\uD83C\uDF89"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"Tab:\tNewline:\nReturn:\rBackspace:\b"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"\u0000"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"\u001f"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"\""
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"\\\/"
EOF

echo "end = $id"

echo "--- D. 大型数组 (Large Arrays) ---" # 026~028

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
# 创建包含100个元素的数组
printf "[" > test_cases_advanced_parsing${id}.json
for j in {1..100}; do
    if [ $j -eq 100 ]; then
        printf "$j]" >> test_cases_advanced_parsing${id}.json
    else
        printf "$j," >> test_cases_advanced_parsing${id}.json
    fi
done

i=$((i+1)); id=$(printf "%03d" $i)
# 创建包含100个字符串的数组
printf "[" > test_cases_advanced_parsing${id}.json
for j in {1..100}; do
    if [ $j -eq 100 ]; then
        printf "\"item$j\"]" >> test_cases_advanced_parsing${id}.json
    else
        printf "\"item$j\"," >> test_cases_advanced_parsing${id}.json
    fi
done

i=$((i+1)); id=$(printf "%03d" $i)
# 创建包含混合类型的大数组
cat > test_cases_advanced_parsing${id}.json << 'EOF'
[1, "two", 3.0, true, null, {"key": "value"}, [1, 2, 3], false, 9, "ten", 11, 12.5, null, true, {"nested": "object"}, [4, 5, 6], 17, "eighteen", 19, 20.0, null, false, 23, "twenty-four", 25]
EOF

echo "end = $id"

echo "--- E. 大型对象 (Large Objects) ---" # 029~030

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
# 创建包含50个键值对的对象
printf "{" > test_cases_advanced_parsing${id}.json
for j in {1..50}; do
    if [ $j -eq 50 ]; then
        printf "\"key$j\":$j}" >> test_cases_advanced_parsing${id}.json
    else
        printf "\"key$j\":$j," >> test_cases_advanced_parsing${id}.json
    fi
done

i=$((i+1)); id=$(printf "%03d" $i)
# 创建包含混合值类型的大对象
printf "{" > test_cases_advanced_parsing${id}.json
for j in {1..30}; do
    if [ $j -eq 30 ]; then
        printf "\"field$j\":\"value$j\"}" >> test_cases_advanced_parsing${id}.json
    else
        printf "\"field$j\":\"value$j\"," >> test_cases_advanced_parsing${id}.json
    fi
done

echo "end = $id"

echo "--- F. 空白字符变体 (Whitespace Variations) ---" # 031~035

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_advanced_parsing${id}.json << 'EOF'
     {"key":"value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
printf "\t\t{\t\"key\"\t:\t\"value\"\t}\t\t" > test_cases_advanced_parsing${id}.json

i=$((i+1)); id=$(printf "%03d" $i)
printf "\n\n\n{\"key\":\"value\"}\n\n\n" > test_cases_advanced_parsing${id}.json

i=$((i+1)); id=$(printf "%03d" $i)
printf "\r\n{\r\n\"key\"\r\n:\r\n\"value\"\r\n}\r\n" > test_cases_advanced_parsing${id}.json

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
{
    "array": [
        1,
        2,
        3
    ],
    "nested": {
        "key": "value"
    }
}
EOF

echo "end = $id"

echo "--- G. 空结构组合 (Empty Structure Combinations) ---" # 036~040

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_advanced_parsing${id}.json << 'EOF'
{"empty_array":[],"empty_object":{}}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
[[],[],[]]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
{"a":{},"b":{},"c":{}}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
[{},{"nested":{}},{},{"nested":[]}]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
{"deeply":{"nested":{"empty":{"structures":{"here":{}}}}}}
EOF

echo "end = $id"

echo "--- H. 特殊字符串内容 (Special String Content) ---" # 041~048

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"   "
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"!!!@@@###$$$%%%^^^&&&***((()))"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"Line1\nLine2\nLine3"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"Quote: \" Backslash: \\ Slash: \/"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"Mixed: 123 abc !@# 你好 \u0041\u0042\u0043"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"http://example.com/path?query=value&foo=bar"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"C:\\Users\\Name\\Documents\\file.txt"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
"{\"not\": \"a real object\"}"
EOF

echo "end = $id"

echo "--- I. 数字格式变体 (Number Format Variations) ---" # 049~055

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_advanced_parsing${id}.json << 'EOF'
1e10
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
1E10
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
1e+10
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
1e-10
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
1.23e10
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
-1.23e-10
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
0e0
EOF

echo "end = $id"

echo "--- J. 复杂实际场景 (Complex Real-world Scenarios) ---" # 056~060

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > test_cases_advanced_parsing${id}.json << 'EOF'
{
  "users": [
    {"id": 1, "name": "Alice", "email": "alice@example.com", "active": true},
    {"id": 2, "name": "Bob", "email": "bob@example.com", "active": false},
    {"id": 3, "name": "Charlie", "email": "charlie@example.com", "active": true}
  ],
  "total_count": 3,
  "page": 1,
  "per_page": 10
}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
{
  "config": {
    "database": {
      "host": "localhost",
      "port": 5432,
      "name": "mydb",
      "credentials": {
        "username": "admin",
        "password": "secret123"
      }
    },
    "cache": {
      "enabled": true,
      "ttl": 3600,
      "max_size": 1000
    }
  }
}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
{
  "data": {
    "type": "articles",
    "id": "1",
    "attributes": {
      "title": "JSON API paints my bikeshed!",
      "body": "The shortest article. Ever.",
      "created": "2015-05-22T14:56:29.000Z",
      "updated": "2015-05-22T14:56:28.000Z"
    },
    "relationships": {
      "author": {
        "data": {"id": "42", "type": "people"}
      }
    }
  }
}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
{
  "glossary": {
    "title": "example glossary",
    "GlossDiv": {
      "title": "S",
      "GlossList": {
        "GlossEntry": {
          "ID": "SGML",
          "SortAs": "SGML",
          "GlossTerm": "Standard Generalized Markup Language",
          "Acronym": "SGML",
          "Abbrev": "ISO 8879:1986",
          "GlossDef": {
            "para": "A meta-markup language",
            "GlossSeeAlso": ["GML", "XML"]
          },
          "GlossSee": "markup"
        }
      }
    }
  }
}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > test_cases_advanced_parsing${id}.json << 'EOF'
{
  "menu": {
    "id": "file",
    "value": "File",
    "popup": {
      "menuitem": [
        {"value": "New", "onclick": "CreateNewDoc()"},
        {"value": "Open", "onclick": "OpenDoc()"},
        {"value": "Close", "onclick": "CloseDoc()"}
      ]
    }
  }
}
EOF

echo "end = $id"

echo "✅ 完成! 创建了 $i 个高级解析测试文件 (advanced_parsing001.json 到 advanced_parsing${id}.json)."

cd ..

