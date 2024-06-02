# kingjson
This is a fast, simple and powerful tool to parse, modify, create json file.

# parse json
1. CKingJson kingJson;
2. kingJson.OpenFileA("d:/data/data.json", 0);
3. const char * pValue = kingJson.GetValue(NULL, "name", NULL);

# create json
1. CKingJson kingJson;
2. PKINGJSON pNode = kingJson.AddNode(NULL, "product", false);
3. kingJson.AddItem(pNode, "value", 1234);

# modify json
1. CKingJson kingJson;
2. kingJson.OpenFileA("d:/data/data.json", 0);
3. PKINGJSON pNode = kingJson.AddNode(NULL, "product", false);
4. kingJson.DelItem(pNode, "value");
5. kingJson.ModItem(pNode, "value", 1234);

# export json
1. create json.
2. const char * pText = kingJson.FormatTxt(NULL);
3. kingJson.SaveToFile(NULL, "d:/temp/data.json");
   