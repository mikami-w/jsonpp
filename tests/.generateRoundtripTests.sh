#!/bin/bash

# 清理旧文件
rm -rf ./roundtrip
echo "🧹 清理了旧的往返测试文件..."

mkdir roundtrip
cd ./roundtrip

# 用于文件名的计数器
i=0

echo "--- A. 简单类型往返 (Simple Type Round-trips) ---" # 001~007

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > roundtrip${id}.json << 'EOF'
null
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
true
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
false
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
42
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
-123
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
3.14159
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
"hello world"
EOF

echo "end = $id"

echo "--- B. 数组往返 (Array Round-trips) ---" # 008~012

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > roundtrip${id}.json << 'EOF'
[]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
[1,2,3,4,5]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
[true,false,null]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
["a","b","c"]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
[1,"two",3.0,true,null]
EOF

echo "end = $id"

echo "--- C. 对象往返 (Object Round-trips) ---" # 013~017

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > roundtrip${id}.json << 'EOF'
{}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
{"key":"value"}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
{"a":1,"b":2,"c":3}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
{"string":"text","number":42,"boolean":true,"null_value":null}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
{"name":"John","age":30,"email":"john@example.com"}
EOF

echo "end = $id"

echo "--- D. 嵌套结构往返 (Nested Structure Round-trips) ---" # 018~022

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > roundtrip${id}.json << 'EOF'
[[1,2],[3,4]]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
{"outer":{"inner":"value"}}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
[{"key":"value1"},{"key":"value2"}]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
{"array":[1,2,3],"object":{"nested":true}}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
{"users":[{"id":1,"name":"Alice"},{"id":2,"name":"Bob"}],"count":2}
EOF

echo "end = $id"

echo "--- E. 特殊字符串往返 (Special String Round-trips) ---" # 023~028

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > roundtrip${id}.json << 'EOF'
""
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
"with \"quotes\""
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
"with\\backslash"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
"line1\nline2"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
"\t\r\n\b\f"
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
"unicode:\u4F60\u597D"
EOF

echo "end = $id"

echo "--- F. 复杂实际数据往返 (Complex Real-world Round-trips) ---" # 029~033

i=$((i+1)); id=$(printf "%03d" $i)
echo "begin = $id"
cat > roundtrip${id}.json << 'EOF'
{"menu":{"id":"file","value":"File","popup":{"menuitem":[{"value":"New","onclick":"CreateNewDoc()"},{"value":"Open","onclick":"OpenDoc()"},{"value":"Close","onclick":"CloseDoc()"}]}}}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
{"widget":{"debug":"on","window":{"title":"Sample Konfabulator Widget","name":"main_window","width":500,"height":500},"image":{"src":"Images/Sun.png","name":"sun1","hOffset":250,"vOffset":250,"alignment":"center"},"text":{"data":"Click Here","size":36,"style":"bold","name":"text1","hOffset":250,"vOffset":100,"alignment":"center","onMouseUp":"sun1.opacity = (sun1.opacity / 100) * 90;"}}}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
{"web-app":{"servlet":[{"servlet-name":"cofaxCDS","servlet-class":"org.cofax.cds.CDSServlet","init-param":{"configGlossary:installationAt":"Philadelphia, PA","configGlossary:adminEmail":"ksm@pobox.com","configGlossary:poweredBy":"Cofax","configGlossary:poweredByIcon":"/images/cofax.gif","configGlossary:staticPath":"/content/static","templateProcessorClass":"org.cofax.WysiwygTemplate","templateLoaderClass":"org.cofax.FilesTemplateLoader","templatePath":"templates","templateOverridePath":"","defaultListTemplate":"listTemplate.htm","defaultFileTemplate":"articleTemplate.htm","useJSP":false,"jspListTemplate":"listTemplate.jsp","jspFileTemplate":"articleTemplate.jsp","cachePackageTagsTrack":200,"cachePackageTagsStore":200,"cachePackageTagsRefresh":60,"cacheTemplatesTrack":100,"cacheTemplatesStore":50,"cacheTemplatesRefresh":15,"cachePagesTrack":200,"cachePagesStore":100,"cachePagesRefresh":10,"cachePagesDirtyRead":10,"searchEngineListTemplate":"forSearchEnginesList.htm","searchEngineFileTemplate":"forSearchEngines.htm","searchEngineRobotsDb":"WEB-INF/robots.db","useDataStore":true,"dataStoreClass":"org.cofax.SqlDataStore","redirectionClass":"org.cofax.SqlRedirection","dataStoreName":"cofax","dataStoreDriver":"com.microsoft.jdbc.sqlserver.SQLServerDriver","dataStoreUrl":"jdbc:microsoft:sqlserver://LOCALHOST:1433;DatabaseName=goon","dataStoreUser":"sa","dataStorePassword":"dataStoreTestQuery","dataStoreTestQuery":"SET NOCOUNT ON;select test='test';","dataStoreLogFile":"/usr/local/tomcat/logs/datastore.log","dataStoreInitConns":10,"dataStoreMaxConns":100,"dataStoreConnUsageLimit":100,"dataStoreLogLevel":"debug","maxUrlLength":500}},{"servlet-name":"cofaxEmail","servlet-class":"org.cofax.cds.EmailServlet","init-param":{"mailHost":"mail1","mailHostOverride":"mail2"}},{"servlet-name":"cofaxAdmin","servlet-class":"org.cofax.cds.AdminServlet"},{"servlet-name":"fileServlet","servlet-class":"org.cofax.cds.FileServlet"},{"servlet-name":"cofaxTools","servlet-class":"org.cofax.cms.CofaxToolsServlet","init-param":{"templatePath":"toolstemplates/","log":1,"logLocation":"/usr/local/tomcat/logs/CofaxTools.log","logMaxSize":"","dataLog":1,"dataLogLocation":"/usr/local/tomcat/logs/dataLog.log","dataLogMaxSize":"","removePageCache":"/content/admin/remove?cache=pages&id=","removeTemplateCache":"/content/admin/remove?cache=templates&id=","fileTransferFolder":"/usr/local/tomcat/webapps/content/fileTransferFolder","lookInContext":1,"adminGroupID":4,"betaServer":true}}],"servlet-mapping":{"cofaxCDS":"/","cofaxEmail":"/cofaxutil/aemail/*","cofaxAdmin":"/admin/*","fileServlet":"/static/*","cofaxTools":"/tools/*"},"taglib":{"taglib-uri":"cofax.tld","taglib-location":"/WEB-INF/tlds/cofax.tld"}}}
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
[{"_id":"5b5d3b7c5f4d2c1a3b6e8f9a","index":0,"guid":"1a2b3c4d-5e6f-7g8h-9i0j-1k2l3m4n5o6p","isActive":true,"balance":"$3,456.78","picture":"http://placehold.it/32x32","age":32,"eyeColor":"brown","name":"John Doe","gender":"male","company":"ACME","email":"john.doe@acme.com","phone":"+1 (555) 123-4567","address":"123 Main St, Anytown, USA 12345","about":"Lorem ipsum dolor sit amet, consectetur adipiscing elit.","registered":"2018-07-29T12:34:56 -08:00","latitude":37.7749,"longitude":-122.4194,"tags":["tag1","tag2","tag3"],"friends":[{"id":0,"name":"Jane Smith"},{"id":1,"name":"Bob Johnson"},{"id":2,"name":"Alice Williams"}],"greeting":"Hello, John Doe! You have 5 unread messages.","favoriteFruit":"apple"}]
EOF

i=$((i+1)); id=$(printf "%03d" $i)
cat > roundtrip${id}.json << 'EOF'
{"glossary":{"title":"example glossary","GlossDiv":{"title":"S","GlossList":{"GlossEntry":{"ID":"SGML","SortAs":"SGML","GlossTerm":"Standard Generalized Markup Language","Acronym":"SGML","Abbrev":"ISO 8879:1986","GlossDef":{"para":"A meta-markup language, used to create markup languages such as DocBook.","GlossSeeAlso":["GML","XML"]},"GlossSee":"markup"}}}}}
EOF

echo "end = $id"

echo "✅ 完成! 创建了 $i 个往返测试文件 (roundtrip001.json 到 roundtrip${id}.json)."

cd ..

