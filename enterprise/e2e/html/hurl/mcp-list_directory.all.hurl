# Missing arguments object

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 100,
  "method": "tools/call",
  "params": {
    "name": "list_directory"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 100
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Root listing

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 101,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": {}
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 101
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.path" == "/"
variable "text" jsonpath "$.path" == "/"
jsonpath "$.result.structuredContent.url" == "{{base}}"
variable "text" jsonpath "$.url" == "{{base}}"
jsonpath "$.result.structuredContent.breadcrumb" count == 0
variable "text" jsonpath "$.breadcrumb" count == 0
jsonpath "$.result.structuredContent.schemas" == 113
variable "text" jsonpath "$.schemas" == 113
jsonpath "$.result.structuredContent.entries" count == 2
variable "text" jsonpath "$.entries" count == 2
jsonpath "$.result.structuredContent.entries[0].name" == "self"
variable "text" jsonpath "$.entries[0].name" == "self"
jsonpath "$.result.structuredContent.entries[0].title" == "Self"
variable "text" jsonpath "$.entries[0].title" == "Self"
jsonpath "$.result.structuredContent.entries[0].description" == "The schemas that define the current version of this instance"
variable "text" jsonpath "$.entries[0].description" == "The schemas that define the current version of this instance"
jsonpath "$.result.structuredContent.entries[0].type" == "directory"
variable "text" jsonpath "$.entries[0].type" == "directory"
jsonpath "$.result.structuredContent.entries[0].health" == 100
variable "text" jsonpath "$.entries[0].health" == 100
jsonpath "$.result.structuredContent.entries[0].schemas" == 45
variable "text" jsonpath "$.entries[0].schemas" == 45
jsonpath "$.result.structuredContent.entries[0].path" == "/self/"
variable "text" jsonpath "$.entries[0].path" == "/self/"
jsonpath "$.result.structuredContent.entries[1].name" == "test"
variable "text" jsonpath "$.entries[1].name" == "test"
jsonpath "$.result.structuredContent.entries[1].type" == "directory"
variable "text" jsonpath "$.entries[1].type" == "directory"
jsonpath "$.result.structuredContent.entries[1].health" == 15
variable "text" jsonpath "$.entries[1].health" == 15
jsonpath "$.result.structuredContent.entries[1].schemas" == 68
variable "text" jsonpath "$.entries[1].schemas" == 68
jsonpath "$.result.structuredContent.entries[1].path" == "/test/"
variable "text" jsonpath "$.entries[1].path" == "/test/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Explicit empty string path

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 102,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 102
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.path" == "/"
variable "text" jsonpath "$.path" == "/"
jsonpath "$.result.structuredContent.url" == "{{base}}"
variable "text" jsonpath "$.url" == "{{base}}"
jsonpath "$.result.structuredContent.breadcrumb" count == 0
variable "text" jsonpath "$.breadcrumb" count == 0
jsonpath "$.result.structuredContent.schemas" == 113
variable "text" jsonpath "$.schemas" == 113
jsonpath "$.result.structuredContent.entries" count == 2
variable "text" jsonpath "$.entries" count == 2
jsonpath "$.result.structuredContent.entries[0].name" == "self"
variable "text" jsonpath "$.entries[0].name" == "self"
jsonpath "$.result.structuredContent.entries[0].type" == "directory"
variable "text" jsonpath "$.entries[0].type" == "directory"
jsonpath "$.result.structuredContent.entries[0].health" == 100
variable "text" jsonpath "$.entries[0].health" == 100
jsonpath "$.result.structuredContent.entries[0].schemas" == 45
variable "text" jsonpath "$.entries[0].schemas" == 45
jsonpath "$.result.structuredContent.entries[0].path" == "/self/"
variable "text" jsonpath "$.entries[0].path" == "/self/"
jsonpath "$.result.structuredContent.entries[0].title" == "Self"
variable "text" jsonpath "$.entries[0].title" == "Self"
jsonpath "$.result.structuredContent.entries[0].description" == "The schemas that define the current version of this instance"
variable "text" jsonpath "$.entries[0].description" == "The schemas that define the current version of this instance"
jsonpath "$.result.structuredContent.entries[1].name" == "test"
variable "text" jsonpath "$.entries[1].name" == "test"
jsonpath "$.result.structuredContent.entries[1].type" == "directory"
variable "text" jsonpath "$.entries[1].type" == "directory"
jsonpath "$.result.structuredContent.entries[1].health" == 15
variable "text" jsonpath "$.entries[1].health" == 15
jsonpath "$.result.structuredContent.entries[1].schemas" == 68
variable "text" jsonpath "$.entries[1].schemas" == 68
jsonpath "$.result.structuredContent.entries[1].path" == "/test/"
variable "text" jsonpath "$.entries[1].path" == "/test/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Subdirectory listing

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 103,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 103
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 2
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.path" == "/test"
variable "text" jsonpath "$.path" == "/test"
jsonpath "$.result.structuredContent.url" == "{{base}}/test"
variable "text" jsonpath "$.url" == "{{base}}/test"
jsonpath "$.result.structuredContent.breadcrumb" count == 1
variable "text" jsonpath "$.breadcrumb" count == 1
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "test"
variable "text" jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.result.structuredContent.breadcrumb[0].path" == "/test/"
variable "text" jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.result.structuredContent.schemas" == 68
variable "text" jsonpath "$.schemas" == 68
jsonpath "$.result.structuredContent.health" == 15
variable "text" jsonpath "$.health" == 15
jsonpath "$.result.structuredContent.entries" count == 6
variable "text" jsonpath "$.entries" count == 6
jsonpath "$.result.structuredContent.entries[0].name" == "bare"
variable "text" jsonpath "$.entries[0].name" == "bare"
jsonpath "$.result.structuredContent.entries[0].type" == "directory"
variable "text" jsonpath "$.entries[0].type" == "directory"
jsonpath "$.result.structuredContent.entries[0].health" == 0
variable "text" jsonpath "$.entries[0].health" == 0
jsonpath "$.result.structuredContent.entries[0].schemas" == 50
variable "text" jsonpath "$.entries[0].schemas" == 50
jsonpath "$.result.structuredContent.entries[0].path" == "/test/bare/"
variable "text" jsonpath "$.entries[0].path" == "/test/bare/"
jsonpath "$.result.structuredContent.entries[1].name" == "description-only"
variable "text" jsonpath "$.entries[1].name" == "description-only"
jsonpath "$.result.structuredContent.entries[1].type" == "directory"
variable "text" jsonpath "$.entries[1].type" == "directory"
jsonpath "$.result.structuredContent.entries[1].health" == 0
variable "text" jsonpath "$.entries[1].health" == 0
jsonpath "$.result.structuredContent.entries[1].schemas" == 5
variable "text" jsonpath "$.entries[1].schemas" == 5
jsonpath "$.result.structuredContent.entries[1].path" == "/test/description-only/"
variable "text" jsonpath "$.entries[1].path" == "/test/description-only/"
jsonpath "$.result.structuredContent.entries[2].name" == "draft3"
variable "text" jsonpath "$.entries[2].name" == "draft3"
jsonpath "$.result.structuredContent.entries[2].type" == "directory"
variable "text" jsonpath "$.entries[2].type" == "directory"
jsonpath "$.result.structuredContent.entries[2].health" == 25
variable "text" jsonpath "$.entries[2].health" == 25
jsonpath "$.result.structuredContent.entries[2].schemas" == 2
variable "text" jsonpath "$.entries[2].schemas" == 2
jsonpath "$.result.structuredContent.entries[2].path" == "/test/draft3/"
variable "text" jsonpath "$.entries[2].path" == "/test/draft3/"
jsonpath "$.result.structuredContent.entries[3].name" == "full"
variable "text" jsonpath "$.entries[3].name" == "full"
jsonpath "$.result.structuredContent.entries[3].type" == "directory"
variable "text" jsonpath "$.entries[3].type" == "directory"
jsonpath "$.result.structuredContent.entries[3].health" == 0
variable "text" jsonpath "$.entries[3].health" == 0
jsonpath "$.result.structuredContent.entries[3].schemas" == 5
variable "text" jsonpath "$.entries[3].schemas" == 5
jsonpath "$.result.structuredContent.entries[3].path" == "/test/full/"
variable "text" jsonpath "$.entries[3].path" == "/test/full/"
jsonpath "$.result.structuredContent.entries[4].name" == "title-only"
variable "text" jsonpath "$.entries[4].name" == "title-only"
jsonpath "$.result.structuredContent.entries[4].type" == "directory"
variable "text" jsonpath "$.entries[4].type" == "directory"
jsonpath "$.result.structuredContent.entries[4].health" == 0
variable "text" jsonpath "$.entries[4].health" == 0
jsonpath "$.result.structuredContent.entries[4].schemas" == 5
variable "text" jsonpath "$.entries[4].schemas" == 5
jsonpath "$.result.structuredContent.entries[4].path" == "/test/title-only/"
variable "text" jsonpath "$.entries[4].path" == "/test/title-only/"
jsonpath "$.result.structuredContent.entries[5].name" == "object"
variable "text" jsonpath "$.entries[5].name" == "object"
jsonpath "$.result.structuredContent.entries[5].type" == "schema"
variable "text" jsonpath "$.entries[5].type" == "schema"
jsonpath "$.result.structuredContent.entries[5].health" == 67
variable "text" jsonpath "$.entries[5].health" == 67
jsonpath "$.result.structuredContent.entries[5].path" == "/test/object"
variable "text" jsonpath "$.entries[5].path" == "/test/object"
jsonpath "$.result.structuredContent.entries[5].identifier" == "{{base}}/test/object"
variable "text" jsonpath "$.entries[5].identifier" == "{{base}}/test/object"
jsonpath "$.result.structuredContent.entries[5].title" == "My Object"
variable "text" jsonpath "$.entries[5].title" == "My Object"
jsonpath "$.result.structuredContent.entries[5].description" == "An Object Example"
variable "text" jsonpath "$.entries[5].description" == "An Object Example"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Deep subdirectory listing

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 104,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test/bare" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 104
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 51
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/bare/50"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.content[2].type" == "resource_link"
jsonpath "$.result.content[2].uri" == "{{base}}/test/bare/49"
jsonpath "$.result.content[2].mimeType" == "application/schema+json"
jsonpath "$.result.content[3].type" == "resource_link"
jsonpath "$.result.content[3].uri" == "{{base}}/test/bare/48"
jsonpath "$.result.content[3].mimeType" == "application/schema+json"
jsonpath "$.result.content[4].type" == "resource_link"
jsonpath "$.result.content[4].uri" == "{{base}}/test/bare/47"
jsonpath "$.result.content[4].mimeType" == "application/schema+json"
jsonpath "$.result.content[5].type" == "resource_link"
jsonpath "$.result.content[5].uri" == "{{base}}/test/bare/46"
jsonpath "$.result.content[5].mimeType" == "application/schema+json"
jsonpath "$.result.content[6].type" == "resource_link"
jsonpath "$.result.content[6].uri" == "{{base}}/test/bare/45"
jsonpath "$.result.content[6].mimeType" == "application/schema+json"
jsonpath "$.result.content[7].type" == "resource_link"
jsonpath "$.result.content[7].uri" == "{{base}}/test/bare/44"
jsonpath "$.result.content[7].mimeType" == "application/schema+json"
jsonpath "$.result.content[8].type" == "resource_link"
jsonpath "$.result.content[8].uri" == "{{base}}/test/bare/43"
jsonpath "$.result.content[8].mimeType" == "application/schema+json"
jsonpath "$.result.content[9].type" == "resource_link"
jsonpath "$.result.content[9].uri" == "{{base}}/test/bare/42"
jsonpath "$.result.content[9].mimeType" == "application/schema+json"
jsonpath "$.result.content[10].type" == "resource_link"
jsonpath "$.result.content[10].uri" == "{{base}}/test/bare/41"
jsonpath "$.result.content[10].mimeType" == "application/schema+json"
jsonpath "$.result.content[11].type" == "resource_link"
jsonpath "$.result.content[11].uri" == "{{base}}/test/bare/40"
jsonpath "$.result.content[11].mimeType" == "application/schema+json"
jsonpath "$.result.content[12].type" == "resource_link"
jsonpath "$.result.content[12].uri" == "{{base}}/test/bare/39"
jsonpath "$.result.content[12].mimeType" == "application/schema+json"
jsonpath "$.result.content[13].type" == "resource_link"
jsonpath "$.result.content[13].uri" == "{{base}}/test/bare/38"
jsonpath "$.result.content[13].mimeType" == "application/schema+json"
jsonpath "$.result.content[14].type" == "resource_link"
jsonpath "$.result.content[14].uri" == "{{base}}/test/bare/37"
jsonpath "$.result.content[14].mimeType" == "application/schema+json"
jsonpath "$.result.content[15].type" == "resource_link"
jsonpath "$.result.content[15].uri" == "{{base}}/test/bare/36"
jsonpath "$.result.content[15].mimeType" == "application/schema+json"
jsonpath "$.result.content[16].type" == "resource_link"
jsonpath "$.result.content[16].uri" == "{{base}}/test/bare/35"
jsonpath "$.result.content[16].mimeType" == "application/schema+json"
jsonpath "$.result.content[17].type" == "resource_link"
jsonpath "$.result.content[17].uri" == "{{base}}/test/bare/34"
jsonpath "$.result.content[17].mimeType" == "application/schema+json"
jsonpath "$.result.content[18].type" == "resource_link"
jsonpath "$.result.content[18].uri" == "{{base}}/test/bare/33"
jsonpath "$.result.content[18].mimeType" == "application/schema+json"
jsonpath "$.result.content[19].type" == "resource_link"
jsonpath "$.result.content[19].uri" == "{{base}}/test/bare/32"
jsonpath "$.result.content[19].mimeType" == "application/schema+json"
jsonpath "$.result.content[20].type" == "resource_link"
jsonpath "$.result.content[20].uri" == "{{base}}/test/bare/31"
jsonpath "$.result.content[20].mimeType" == "application/schema+json"
jsonpath "$.result.content[21].type" == "resource_link"
jsonpath "$.result.content[21].uri" == "{{base}}/test/bare/30"
jsonpath "$.result.content[21].mimeType" == "application/schema+json"
jsonpath "$.result.content[22].type" == "resource_link"
jsonpath "$.result.content[22].uri" == "{{base}}/test/bare/29"
jsonpath "$.result.content[22].mimeType" == "application/schema+json"
jsonpath "$.result.content[23].type" == "resource_link"
jsonpath "$.result.content[23].uri" == "{{base}}/test/bare/28"
jsonpath "$.result.content[23].mimeType" == "application/schema+json"
jsonpath "$.result.content[24].type" == "resource_link"
jsonpath "$.result.content[24].uri" == "{{base}}/test/bare/27"
jsonpath "$.result.content[24].mimeType" == "application/schema+json"
jsonpath "$.result.content[25].type" == "resource_link"
jsonpath "$.result.content[25].uri" == "{{base}}/test/bare/26"
jsonpath "$.result.content[25].mimeType" == "application/schema+json"
jsonpath "$.result.content[26].type" == "resource_link"
jsonpath "$.result.content[26].uri" == "{{base}}/test/bare/25"
jsonpath "$.result.content[26].mimeType" == "application/schema+json"
jsonpath "$.result.content[27].type" == "resource_link"
jsonpath "$.result.content[27].uri" == "{{base}}/test/bare/24"
jsonpath "$.result.content[27].mimeType" == "application/schema+json"
jsonpath "$.result.content[28].type" == "resource_link"
jsonpath "$.result.content[28].uri" == "{{base}}/test/bare/23"
jsonpath "$.result.content[28].mimeType" == "application/schema+json"
jsonpath "$.result.content[29].type" == "resource_link"
jsonpath "$.result.content[29].uri" == "{{base}}/test/bare/22"
jsonpath "$.result.content[29].mimeType" == "application/schema+json"
jsonpath "$.result.content[30].type" == "resource_link"
jsonpath "$.result.content[30].uri" == "{{base}}/test/bare/21"
jsonpath "$.result.content[30].mimeType" == "application/schema+json"
jsonpath "$.result.content[31].type" == "resource_link"
jsonpath "$.result.content[31].uri" == "{{base}}/test/bare/20"
jsonpath "$.result.content[31].mimeType" == "application/schema+json"
jsonpath "$.result.content[32].type" == "resource_link"
jsonpath "$.result.content[32].uri" == "{{base}}/test/bare/19"
jsonpath "$.result.content[32].mimeType" == "application/schema+json"
jsonpath "$.result.content[33].type" == "resource_link"
jsonpath "$.result.content[33].uri" == "{{base}}/test/bare/18"
jsonpath "$.result.content[33].mimeType" == "application/schema+json"
jsonpath "$.result.content[34].type" == "resource_link"
jsonpath "$.result.content[34].uri" == "{{base}}/test/bare/17"
jsonpath "$.result.content[34].mimeType" == "application/schema+json"
jsonpath "$.result.content[35].type" == "resource_link"
jsonpath "$.result.content[35].uri" == "{{base}}/test/bare/16"
jsonpath "$.result.content[35].mimeType" == "application/schema+json"
jsonpath "$.result.content[36].type" == "resource_link"
jsonpath "$.result.content[36].uri" == "{{base}}/test/bare/15"
jsonpath "$.result.content[36].mimeType" == "application/schema+json"
jsonpath "$.result.content[37].type" == "resource_link"
jsonpath "$.result.content[37].uri" == "{{base}}/test/bare/14"
jsonpath "$.result.content[37].mimeType" == "application/schema+json"
jsonpath "$.result.content[38].type" == "resource_link"
jsonpath "$.result.content[38].uri" == "{{base}}/test/bare/13"
jsonpath "$.result.content[38].mimeType" == "application/schema+json"
jsonpath "$.result.content[39].type" == "resource_link"
jsonpath "$.result.content[39].uri" == "{{base}}/test/bare/12"
jsonpath "$.result.content[39].mimeType" == "application/schema+json"
jsonpath "$.result.content[40].type" == "resource_link"
jsonpath "$.result.content[40].uri" == "{{base}}/test/bare/11"
jsonpath "$.result.content[40].mimeType" == "application/schema+json"
jsonpath "$.result.content[41].type" == "resource_link"
jsonpath "$.result.content[41].uri" == "{{base}}/test/bare/10"
jsonpath "$.result.content[41].mimeType" == "application/schema+json"
jsonpath "$.result.content[42].type" == "resource_link"
jsonpath "$.result.content[42].uri" == "{{base}}/test/bare/01"
jsonpath "$.result.content[42].mimeType" == "application/schema+json"
jsonpath "$.result.content[43].type" == "resource_link"
jsonpath "$.result.content[43].uri" == "{{base}}/test/bare/02"
jsonpath "$.result.content[43].mimeType" == "application/schema+json"
jsonpath "$.result.content[44].type" == "resource_link"
jsonpath "$.result.content[44].uri" == "{{base}}/test/bare/03"
jsonpath "$.result.content[44].mimeType" == "application/schema+json"
jsonpath "$.result.content[45].type" == "resource_link"
jsonpath "$.result.content[45].uri" == "{{base}}/test/bare/04"
jsonpath "$.result.content[45].mimeType" == "application/schema+json"
jsonpath "$.result.content[46].type" == "resource_link"
jsonpath "$.result.content[46].uri" == "{{base}}/test/bare/05"
jsonpath "$.result.content[46].mimeType" == "application/schema+json"
jsonpath "$.result.content[47].type" == "resource_link"
jsonpath "$.result.content[47].uri" == "{{base}}/test/bare/06"
jsonpath "$.result.content[47].mimeType" == "application/schema+json"
jsonpath "$.result.content[48].type" == "resource_link"
jsonpath "$.result.content[48].uri" == "{{base}}/test/bare/07"
jsonpath "$.result.content[48].mimeType" == "application/schema+json"
jsonpath "$.result.content[49].type" == "resource_link"
jsonpath "$.result.content[49].uri" == "{{base}}/test/bare/08"
jsonpath "$.result.content[49].mimeType" == "application/schema+json"
jsonpath "$.result.content[50].type" == "resource_link"
jsonpath "$.result.content[50].uri" == "{{base}}/test/bare/09"
jsonpath "$.result.content[50].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.path" == "/test/bare"
variable "text" jsonpath "$.path" == "/test/bare"
jsonpath "$.result.structuredContent.url" == "{{base}}/test/bare"
variable "text" jsonpath "$.url" == "{{base}}/test/bare"
jsonpath "$.result.structuredContent.breadcrumb" count == 2
variable "text" jsonpath "$.breadcrumb" count == 2
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "test"
variable "text" jsonpath "$.breadcrumb[0].name" == "test"
jsonpath "$.result.structuredContent.breadcrumb[0].path" == "/test/"
variable "text" jsonpath "$.breadcrumb[0].path" == "/test/"
jsonpath "$.result.structuredContent.breadcrumb[1].name" == "bare"
variable "text" jsonpath "$.breadcrumb[1].name" == "bare"
jsonpath "$.result.structuredContent.breadcrumb[1].path" == "/test/bare/"
variable "text" jsonpath "$.breadcrumb[1].path" == "/test/bare/"
jsonpath "$.result.structuredContent.schemas" == 50
variable "text" jsonpath "$.schemas" == 50
jsonpath "$.result.structuredContent.health" == 0
variable "text" jsonpath "$.health" == 0
jsonpath "$.result.structuredContent.entries" count == 50
variable "text" jsonpath "$.entries" count == 50
jsonpath "$.result.structuredContent.entries[0].name" == "50"
variable "text" jsonpath "$.entries[0].name" == "50"
jsonpath "$.result.structuredContent.entries[0].type" == "schema"
variable "text" jsonpath "$.entries[0].type" == "schema"
jsonpath "$.result.structuredContent.entries[0].path" == "/test/bare/50"
variable "text" jsonpath "$.entries[0].path" == "/test/bare/50"
jsonpath "$.result.structuredContent.entries[0].identifier" == "{{base}}/test/bare/50"
variable "text" jsonpath "$.entries[0].identifier" == "{{base}}/test/bare/50"
jsonpath "$.result.structuredContent.entries[0].health" == 0
variable "text" jsonpath "$.entries[0].health" == 0
jsonpath "$.result.structuredContent.entries[0].bytes" == 130
variable "text" jsonpath "$.entries[0].bytes" == 130
jsonpath "$.result.structuredContent.entries[0].bytesBundled" == 130
variable "text" jsonpath "$.entries[0].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[0].dependencies" == 0
variable "text" jsonpath "$.entries[0].dependencies" == 0
jsonpath "$.result.structuredContent.entries[0].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[0].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[0].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[0].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[1].name" == "49"
variable "text" jsonpath "$.entries[1].name" == "49"
jsonpath "$.result.structuredContent.entries[1].type" == "schema"
variable "text" jsonpath "$.entries[1].type" == "schema"
jsonpath "$.result.structuredContent.entries[1].path" == "/test/bare/49"
variable "text" jsonpath "$.entries[1].path" == "/test/bare/49"
jsonpath "$.result.structuredContent.entries[1].identifier" == "{{base}}/test/bare/49"
variable "text" jsonpath "$.entries[1].identifier" == "{{base}}/test/bare/49"
jsonpath "$.result.structuredContent.entries[1].health" == 0
variable "text" jsonpath "$.entries[1].health" == 0
jsonpath "$.result.structuredContent.entries[1].bytes" == 130
variable "text" jsonpath "$.entries[1].bytes" == 130
jsonpath "$.result.structuredContent.entries[1].bytesBundled" == 130
variable "text" jsonpath "$.entries[1].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[1].dependencies" == 0
variable "text" jsonpath "$.entries[1].dependencies" == 0
jsonpath "$.result.structuredContent.entries[1].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[1].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[1].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[1].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[2].name" == "48"
variable "text" jsonpath "$.entries[2].name" == "48"
jsonpath "$.result.structuredContent.entries[2].type" == "schema"
variable "text" jsonpath "$.entries[2].type" == "schema"
jsonpath "$.result.structuredContent.entries[2].path" == "/test/bare/48"
variable "text" jsonpath "$.entries[2].path" == "/test/bare/48"
jsonpath "$.result.structuredContent.entries[2].identifier" == "{{base}}/test/bare/48"
variable "text" jsonpath "$.entries[2].identifier" == "{{base}}/test/bare/48"
jsonpath "$.result.structuredContent.entries[2].health" == 0
variable "text" jsonpath "$.entries[2].health" == 0
jsonpath "$.result.structuredContent.entries[2].bytes" == 130
variable "text" jsonpath "$.entries[2].bytes" == 130
jsonpath "$.result.structuredContent.entries[2].bytesBundled" == 130
variable "text" jsonpath "$.entries[2].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[2].dependencies" == 0
variable "text" jsonpath "$.entries[2].dependencies" == 0
jsonpath "$.result.structuredContent.entries[2].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[2].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[2].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[2].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[3].name" == "47"
variable "text" jsonpath "$.entries[3].name" == "47"
jsonpath "$.result.structuredContent.entries[3].type" == "schema"
variable "text" jsonpath "$.entries[3].type" == "schema"
jsonpath "$.result.structuredContent.entries[3].path" == "/test/bare/47"
variable "text" jsonpath "$.entries[3].path" == "/test/bare/47"
jsonpath "$.result.structuredContent.entries[3].identifier" == "{{base}}/test/bare/47"
variable "text" jsonpath "$.entries[3].identifier" == "{{base}}/test/bare/47"
jsonpath "$.result.structuredContent.entries[3].health" == 0
variable "text" jsonpath "$.entries[3].health" == 0
jsonpath "$.result.structuredContent.entries[3].bytes" == 130
variable "text" jsonpath "$.entries[3].bytes" == 130
jsonpath "$.result.structuredContent.entries[3].bytesBundled" == 130
variable "text" jsonpath "$.entries[3].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[3].dependencies" == 0
variable "text" jsonpath "$.entries[3].dependencies" == 0
jsonpath "$.result.structuredContent.entries[3].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[3].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[3].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[3].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[4].name" == "46"
variable "text" jsonpath "$.entries[4].name" == "46"
jsonpath "$.result.structuredContent.entries[4].type" == "schema"
variable "text" jsonpath "$.entries[4].type" == "schema"
jsonpath "$.result.structuredContent.entries[4].path" == "/test/bare/46"
variable "text" jsonpath "$.entries[4].path" == "/test/bare/46"
jsonpath "$.result.structuredContent.entries[4].identifier" == "{{base}}/test/bare/46"
variable "text" jsonpath "$.entries[4].identifier" == "{{base}}/test/bare/46"
jsonpath "$.result.structuredContent.entries[4].health" == 0
variable "text" jsonpath "$.entries[4].health" == 0
jsonpath "$.result.structuredContent.entries[4].bytes" == 130
variable "text" jsonpath "$.entries[4].bytes" == 130
jsonpath "$.result.structuredContent.entries[4].bytesBundled" == 130
variable "text" jsonpath "$.entries[4].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[4].dependencies" == 0
variable "text" jsonpath "$.entries[4].dependencies" == 0
jsonpath "$.result.structuredContent.entries[4].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[4].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[4].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[4].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[5].name" == "45"
variable "text" jsonpath "$.entries[5].name" == "45"
jsonpath "$.result.structuredContent.entries[5].type" == "schema"
variable "text" jsonpath "$.entries[5].type" == "schema"
jsonpath "$.result.structuredContent.entries[5].path" == "/test/bare/45"
variable "text" jsonpath "$.entries[5].path" == "/test/bare/45"
jsonpath "$.result.structuredContent.entries[5].identifier" == "{{base}}/test/bare/45"
variable "text" jsonpath "$.entries[5].identifier" == "{{base}}/test/bare/45"
jsonpath "$.result.structuredContent.entries[5].health" == 0
variable "text" jsonpath "$.entries[5].health" == 0
jsonpath "$.result.structuredContent.entries[5].bytes" == 130
variable "text" jsonpath "$.entries[5].bytes" == 130
jsonpath "$.result.structuredContent.entries[5].bytesBundled" == 130
variable "text" jsonpath "$.entries[5].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[5].dependencies" == 0
variable "text" jsonpath "$.entries[5].dependencies" == 0
jsonpath "$.result.structuredContent.entries[5].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[5].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[5].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[5].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[6].name" == "44"
variable "text" jsonpath "$.entries[6].name" == "44"
jsonpath "$.result.structuredContent.entries[6].type" == "schema"
variable "text" jsonpath "$.entries[6].type" == "schema"
jsonpath "$.result.structuredContent.entries[6].path" == "/test/bare/44"
variable "text" jsonpath "$.entries[6].path" == "/test/bare/44"
jsonpath "$.result.structuredContent.entries[6].identifier" == "{{base}}/test/bare/44"
variable "text" jsonpath "$.entries[6].identifier" == "{{base}}/test/bare/44"
jsonpath "$.result.structuredContent.entries[6].health" == 0
variable "text" jsonpath "$.entries[6].health" == 0
jsonpath "$.result.structuredContent.entries[6].bytes" == 130
variable "text" jsonpath "$.entries[6].bytes" == 130
jsonpath "$.result.structuredContent.entries[6].bytesBundled" == 130
variable "text" jsonpath "$.entries[6].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[6].dependencies" == 0
variable "text" jsonpath "$.entries[6].dependencies" == 0
jsonpath "$.result.structuredContent.entries[6].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[6].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[6].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[6].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[7].name" == "43"
variable "text" jsonpath "$.entries[7].name" == "43"
jsonpath "$.result.structuredContent.entries[7].type" == "schema"
variable "text" jsonpath "$.entries[7].type" == "schema"
jsonpath "$.result.structuredContent.entries[7].path" == "/test/bare/43"
variable "text" jsonpath "$.entries[7].path" == "/test/bare/43"
jsonpath "$.result.structuredContent.entries[7].identifier" == "{{base}}/test/bare/43"
variable "text" jsonpath "$.entries[7].identifier" == "{{base}}/test/bare/43"
jsonpath "$.result.structuredContent.entries[7].health" == 0
variable "text" jsonpath "$.entries[7].health" == 0
jsonpath "$.result.structuredContent.entries[7].bytes" == 130
variable "text" jsonpath "$.entries[7].bytes" == 130
jsonpath "$.result.structuredContent.entries[7].bytesBundled" == 130
variable "text" jsonpath "$.entries[7].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[7].dependencies" == 0
variable "text" jsonpath "$.entries[7].dependencies" == 0
jsonpath "$.result.structuredContent.entries[7].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[7].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[7].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[7].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[8].name" == "42"
variable "text" jsonpath "$.entries[8].name" == "42"
jsonpath "$.result.structuredContent.entries[8].type" == "schema"
variable "text" jsonpath "$.entries[8].type" == "schema"
jsonpath "$.result.structuredContent.entries[8].path" == "/test/bare/42"
variable "text" jsonpath "$.entries[8].path" == "/test/bare/42"
jsonpath "$.result.structuredContent.entries[8].identifier" == "{{base}}/test/bare/42"
variable "text" jsonpath "$.entries[8].identifier" == "{{base}}/test/bare/42"
jsonpath "$.result.structuredContent.entries[8].health" == 0
variable "text" jsonpath "$.entries[8].health" == 0
jsonpath "$.result.structuredContent.entries[8].bytes" == 130
variable "text" jsonpath "$.entries[8].bytes" == 130
jsonpath "$.result.structuredContent.entries[8].bytesBundled" == 130
variable "text" jsonpath "$.entries[8].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[8].dependencies" == 0
variable "text" jsonpath "$.entries[8].dependencies" == 0
jsonpath "$.result.structuredContent.entries[8].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[8].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[8].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[8].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[9].name" == "41"
variable "text" jsonpath "$.entries[9].name" == "41"
jsonpath "$.result.structuredContent.entries[9].type" == "schema"
variable "text" jsonpath "$.entries[9].type" == "schema"
jsonpath "$.result.structuredContent.entries[9].path" == "/test/bare/41"
variable "text" jsonpath "$.entries[9].path" == "/test/bare/41"
jsonpath "$.result.structuredContent.entries[9].identifier" == "{{base}}/test/bare/41"
variable "text" jsonpath "$.entries[9].identifier" == "{{base}}/test/bare/41"
jsonpath "$.result.structuredContent.entries[9].health" == 0
variable "text" jsonpath "$.entries[9].health" == 0
jsonpath "$.result.structuredContent.entries[9].bytes" == 130
variable "text" jsonpath "$.entries[9].bytes" == 130
jsonpath "$.result.structuredContent.entries[9].bytesBundled" == 130
variable "text" jsonpath "$.entries[9].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[9].dependencies" == 0
variable "text" jsonpath "$.entries[9].dependencies" == 0
jsonpath "$.result.structuredContent.entries[9].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[9].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[9].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[9].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[10].name" == "40"
variable "text" jsonpath "$.entries[10].name" == "40"
jsonpath "$.result.structuredContent.entries[10].type" == "schema"
variable "text" jsonpath "$.entries[10].type" == "schema"
jsonpath "$.result.structuredContent.entries[10].path" == "/test/bare/40"
variable "text" jsonpath "$.entries[10].path" == "/test/bare/40"
jsonpath "$.result.structuredContent.entries[10].identifier" == "{{base}}/test/bare/40"
variable "text" jsonpath "$.entries[10].identifier" == "{{base}}/test/bare/40"
jsonpath "$.result.structuredContent.entries[10].health" == 0
variable "text" jsonpath "$.entries[10].health" == 0
jsonpath "$.result.structuredContent.entries[10].bytes" == 130
variable "text" jsonpath "$.entries[10].bytes" == 130
jsonpath "$.result.structuredContent.entries[10].bytesBundled" == 130
variable "text" jsonpath "$.entries[10].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[10].dependencies" == 0
variable "text" jsonpath "$.entries[10].dependencies" == 0
jsonpath "$.result.structuredContent.entries[10].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[10].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[10].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[10].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[11].name" == "39"
variable "text" jsonpath "$.entries[11].name" == "39"
jsonpath "$.result.structuredContent.entries[11].type" == "schema"
variable "text" jsonpath "$.entries[11].type" == "schema"
jsonpath "$.result.structuredContent.entries[11].path" == "/test/bare/39"
variable "text" jsonpath "$.entries[11].path" == "/test/bare/39"
jsonpath "$.result.structuredContent.entries[11].identifier" == "{{base}}/test/bare/39"
variable "text" jsonpath "$.entries[11].identifier" == "{{base}}/test/bare/39"
jsonpath "$.result.structuredContent.entries[11].health" == 0
variable "text" jsonpath "$.entries[11].health" == 0
jsonpath "$.result.structuredContent.entries[11].bytes" == 130
variable "text" jsonpath "$.entries[11].bytes" == 130
jsonpath "$.result.structuredContent.entries[11].bytesBundled" == 130
variable "text" jsonpath "$.entries[11].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[11].dependencies" == 0
variable "text" jsonpath "$.entries[11].dependencies" == 0
jsonpath "$.result.structuredContent.entries[11].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[11].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[11].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[11].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[12].name" == "38"
variable "text" jsonpath "$.entries[12].name" == "38"
jsonpath "$.result.structuredContent.entries[12].type" == "schema"
variable "text" jsonpath "$.entries[12].type" == "schema"
jsonpath "$.result.structuredContent.entries[12].path" == "/test/bare/38"
variable "text" jsonpath "$.entries[12].path" == "/test/bare/38"
jsonpath "$.result.structuredContent.entries[12].identifier" == "{{base}}/test/bare/38"
variable "text" jsonpath "$.entries[12].identifier" == "{{base}}/test/bare/38"
jsonpath "$.result.structuredContent.entries[12].health" == 0
variable "text" jsonpath "$.entries[12].health" == 0
jsonpath "$.result.structuredContent.entries[12].bytes" == 130
variable "text" jsonpath "$.entries[12].bytes" == 130
jsonpath "$.result.structuredContent.entries[12].bytesBundled" == 130
variable "text" jsonpath "$.entries[12].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[12].dependencies" == 0
variable "text" jsonpath "$.entries[12].dependencies" == 0
jsonpath "$.result.structuredContent.entries[12].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[12].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[12].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[12].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[13].name" == "37"
variable "text" jsonpath "$.entries[13].name" == "37"
jsonpath "$.result.structuredContent.entries[13].type" == "schema"
variable "text" jsonpath "$.entries[13].type" == "schema"
jsonpath "$.result.structuredContent.entries[13].path" == "/test/bare/37"
variable "text" jsonpath "$.entries[13].path" == "/test/bare/37"
jsonpath "$.result.structuredContent.entries[13].identifier" == "{{base}}/test/bare/37"
variable "text" jsonpath "$.entries[13].identifier" == "{{base}}/test/bare/37"
jsonpath "$.result.structuredContent.entries[13].health" == 0
variable "text" jsonpath "$.entries[13].health" == 0
jsonpath "$.result.structuredContent.entries[13].bytes" == 130
variable "text" jsonpath "$.entries[13].bytes" == 130
jsonpath "$.result.structuredContent.entries[13].bytesBundled" == 130
variable "text" jsonpath "$.entries[13].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[13].dependencies" == 0
variable "text" jsonpath "$.entries[13].dependencies" == 0
jsonpath "$.result.structuredContent.entries[13].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[13].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[13].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[13].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[14].name" == "36"
variable "text" jsonpath "$.entries[14].name" == "36"
jsonpath "$.result.structuredContent.entries[14].type" == "schema"
variable "text" jsonpath "$.entries[14].type" == "schema"
jsonpath "$.result.structuredContent.entries[14].path" == "/test/bare/36"
variable "text" jsonpath "$.entries[14].path" == "/test/bare/36"
jsonpath "$.result.structuredContent.entries[14].identifier" == "{{base}}/test/bare/36"
variable "text" jsonpath "$.entries[14].identifier" == "{{base}}/test/bare/36"
jsonpath "$.result.structuredContent.entries[14].health" == 0
variable "text" jsonpath "$.entries[14].health" == 0
jsonpath "$.result.structuredContent.entries[14].bytes" == 130
variable "text" jsonpath "$.entries[14].bytes" == 130
jsonpath "$.result.structuredContent.entries[14].bytesBundled" == 130
variable "text" jsonpath "$.entries[14].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[14].dependencies" == 0
variable "text" jsonpath "$.entries[14].dependencies" == 0
jsonpath "$.result.structuredContent.entries[14].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[14].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[14].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[14].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[15].name" == "35"
variable "text" jsonpath "$.entries[15].name" == "35"
jsonpath "$.result.structuredContent.entries[15].type" == "schema"
variable "text" jsonpath "$.entries[15].type" == "schema"
jsonpath "$.result.structuredContent.entries[15].path" == "/test/bare/35"
variable "text" jsonpath "$.entries[15].path" == "/test/bare/35"
jsonpath "$.result.structuredContent.entries[15].identifier" == "{{base}}/test/bare/35"
variable "text" jsonpath "$.entries[15].identifier" == "{{base}}/test/bare/35"
jsonpath "$.result.structuredContent.entries[15].health" == 0
variable "text" jsonpath "$.entries[15].health" == 0
jsonpath "$.result.structuredContent.entries[15].bytes" == 130
variable "text" jsonpath "$.entries[15].bytes" == 130
jsonpath "$.result.structuredContent.entries[15].bytesBundled" == 130
variable "text" jsonpath "$.entries[15].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[15].dependencies" == 0
variable "text" jsonpath "$.entries[15].dependencies" == 0
jsonpath "$.result.structuredContent.entries[15].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[15].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[15].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[15].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[16].name" == "34"
variable "text" jsonpath "$.entries[16].name" == "34"
jsonpath "$.result.structuredContent.entries[16].type" == "schema"
variable "text" jsonpath "$.entries[16].type" == "schema"
jsonpath "$.result.structuredContent.entries[16].path" == "/test/bare/34"
variable "text" jsonpath "$.entries[16].path" == "/test/bare/34"
jsonpath "$.result.structuredContent.entries[16].identifier" == "{{base}}/test/bare/34"
variable "text" jsonpath "$.entries[16].identifier" == "{{base}}/test/bare/34"
jsonpath "$.result.structuredContent.entries[16].health" == 0
variable "text" jsonpath "$.entries[16].health" == 0
jsonpath "$.result.structuredContent.entries[16].bytes" == 130
variable "text" jsonpath "$.entries[16].bytes" == 130
jsonpath "$.result.structuredContent.entries[16].bytesBundled" == 130
variable "text" jsonpath "$.entries[16].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[16].dependencies" == 0
variable "text" jsonpath "$.entries[16].dependencies" == 0
jsonpath "$.result.structuredContent.entries[16].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[16].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[16].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[16].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[17].name" == "33"
variable "text" jsonpath "$.entries[17].name" == "33"
jsonpath "$.result.structuredContent.entries[17].type" == "schema"
variable "text" jsonpath "$.entries[17].type" == "schema"
jsonpath "$.result.structuredContent.entries[17].path" == "/test/bare/33"
variable "text" jsonpath "$.entries[17].path" == "/test/bare/33"
jsonpath "$.result.structuredContent.entries[17].identifier" == "{{base}}/test/bare/33"
variable "text" jsonpath "$.entries[17].identifier" == "{{base}}/test/bare/33"
jsonpath "$.result.structuredContent.entries[17].health" == 0
variable "text" jsonpath "$.entries[17].health" == 0
jsonpath "$.result.structuredContent.entries[17].bytes" == 130
variable "text" jsonpath "$.entries[17].bytes" == 130
jsonpath "$.result.structuredContent.entries[17].bytesBundled" == 130
variable "text" jsonpath "$.entries[17].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[17].dependencies" == 0
variable "text" jsonpath "$.entries[17].dependencies" == 0
jsonpath "$.result.structuredContent.entries[17].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[17].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[17].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[17].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[18].name" == "32"
variable "text" jsonpath "$.entries[18].name" == "32"
jsonpath "$.result.structuredContent.entries[18].type" == "schema"
variable "text" jsonpath "$.entries[18].type" == "schema"
jsonpath "$.result.structuredContent.entries[18].path" == "/test/bare/32"
variable "text" jsonpath "$.entries[18].path" == "/test/bare/32"
jsonpath "$.result.structuredContent.entries[18].identifier" == "{{base}}/test/bare/32"
variable "text" jsonpath "$.entries[18].identifier" == "{{base}}/test/bare/32"
jsonpath "$.result.structuredContent.entries[18].health" == 0
variable "text" jsonpath "$.entries[18].health" == 0
jsonpath "$.result.structuredContent.entries[18].bytes" == 130
variable "text" jsonpath "$.entries[18].bytes" == 130
jsonpath "$.result.structuredContent.entries[18].bytesBundled" == 130
variable "text" jsonpath "$.entries[18].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[18].dependencies" == 0
variable "text" jsonpath "$.entries[18].dependencies" == 0
jsonpath "$.result.structuredContent.entries[18].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[18].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[18].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[18].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[19].name" == "31"
variable "text" jsonpath "$.entries[19].name" == "31"
jsonpath "$.result.structuredContent.entries[19].type" == "schema"
variable "text" jsonpath "$.entries[19].type" == "schema"
jsonpath "$.result.structuredContent.entries[19].path" == "/test/bare/31"
variable "text" jsonpath "$.entries[19].path" == "/test/bare/31"
jsonpath "$.result.structuredContent.entries[19].identifier" == "{{base}}/test/bare/31"
variable "text" jsonpath "$.entries[19].identifier" == "{{base}}/test/bare/31"
jsonpath "$.result.structuredContent.entries[19].health" == 0
variable "text" jsonpath "$.entries[19].health" == 0
jsonpath "$.result.structuredContent.entries[19].bytes" == 130
variable "text" jsonpath "$.entries[19].bytes" == 130
jsonpath "$.result.structuredContent.entries[19].bytesBundled" == 130
variable "text" jsonpath "$.entries[19].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[19].dependencies" == 0
variable "text" jsonpath "$.entries[19].dependencies" == 0
jsonpath "$.result.structuredContent.entries[19].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[19].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[19].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[19].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[20].name" == "30"
variable "text" jsonpath "$.entries[20].name" == "30"
jsonpath "$.result.structuredContent.entries[20].type" == "schema"
variable "text" jsonpath "$.entries[20].type" == "schema"
jsonpath "$.result.structuredContent.entries[20].path" == "/test/bare/30"
variable "text" jsonpath "$.entries[20].path" == "/test/bare/30"
jsonpath "$.result.structuredContent.entries[20].identifier" == "{{base}}/test/bare/30"
variable "text" jsonpath "$.entries[20].identifier" == "{{base}}/test/bare/30"
jsonpath "$.result.structuredContent.entries[20].health" == 0
variable "text" jsonpath "$.entries[20].health" == 0
jsonpath "$.result.structuredContent.entries[20].bytes" == 130
variable "text" jsonpath "$.entries[20].bytes" == 130
jsonpath "$.result.structuredContent.entries[20].bytesBundled" == 130
variable "text" jsonpath "$.entries[20].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[20].dependencies" == 0
variable "text" jsonpath "$.entries[20].dependencies" == 0
jsonpath "$.result.structuredContent.entries[20].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[20].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[20].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[20].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[21].name" == "29"
variable "text" jsonpath "$.entries[21].name" == "29"
jsonpath "$.result.structuredContent.entries[21].type" == "schema"
variable "text" jsonpath "$.entries[21].type" == "schema"
jsonpath "$.result.structuredContent.entries[21].path" == "/test/bare/29"
variable "text" jsonpath "$.entries[21].path" == "/test/bare/29"
jsonpath "$.result.structuredContent.entries[21].identifier" == "{{base}}/test/bare/29"
variable "text" jsonpath "$.entries[21].identifier" == "{{base}}/test/bare/29"
jsonpath "$.result.structuredContent.entries[21].health" == 0
variable "text" jsonpath "$.entries[21].health" == 0
jsonpath "$.result.structuredContent.entries[21].bytes" == 130
variable "text" jsonpath "$.entries[21].bytes" == 130
jsonpath "$.result.structuredContent.entries[21].bytesBundled" == 130
variable "text" jsonpath "$.entries[21].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[21].dependencies" == 0
variable "text" jsonpath "$.entries[21].dependencies" == 0
jsonpath "$.result.structuredContent.entries[21].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[21].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[21].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[21].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[22].name" == "28"
variable "text" jsonpath "$.entries[22].name" == "28"
jsonpath "$.result.structuredContent.entries[22].type" == "schema"
variable "text" jsonpath "$.entries[22].type" == "schema"
jsonpath "$.result.structuredContent.entries[22].path" == "/test/bare/28"
variable "text" jsonpath "$.entries[22].path" == "/test/bare/28"
jsonpath "$.result.structuredContent.entries[22].identifier" == "{{base}}/test/bare/28"
variable "text" jsonpath "$.entries[22].identifier" == "{{base}}/test/bare/28"
jsonpath "$.result.structuredContent.entries[22].health" == 0
variable "text" jsonpath "$.entries[22].health" == 0
jsonpath "$.result.structuredContent.entries[22].bytes" == 130
variable "text" jsonpath "$.entries[22].bytes" == 130
jsonpath "$.result.structuredContent.entries[22].bytesBundled" == 130
variable "text" jsonpath "$.entries[22].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[22].dependencies" == 0
variable "text" jsonpath "$.entries[22].dependencies" == 0
jsonpath "$.result.structuredContent.entries[22].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[22].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[22].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[22].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[23].name" == "27"
variable "text" jsonpath "$.entries[23].name" == "27"
jsonpath "$.result.structuredContent.entries[23].type" == "schema"
variable "text" jsonpath "$.entries[23].type" == "schema"
jsonpath "$.result.structuredContent.entries[23].path" == "/test/bare/27"
variable "text" jsonpath "$.entries[23].path" == "/test/bare/27"
jsonpath "$.result.structuredContent.entries[23].identifier" == "{{base}}/test/bare/27"
variable "text" jsonpath "$.entries[23].identifier" == "{{base}}/test/bare/27"
jsonpath "$.result.structuredContent.entries[23].health" == 0
variable "text" jsonpath "$.entries[23].health" == 0
jsonpath "$.result.structuredContent.entries[23].bytes" == 130
variable "text" jsonpath "$.entries[23].bytes" == 130
jsonpath "$.result.structuredContent.entries[23].bytesBundled" == 130
variable "text" jsonpath "$.entries[23].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[23].dependencies" == 0
variable "text" jsonpath "$.entries[23].dependencies" == 0
jsonpath "$.result.structuredContent.entries[23].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[23].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[23].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[23].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[24].name" == "26"
variable "text" jsonpath "$.entries[24].name" == "26"
jsonpath "$.result.structuredContent.entries[24].type" == "schema"
variable "text" jsonpath "$.entries[24].type" == "schema"
jsonpath "$.result.structuredContent.entries[24].path" == "/test/bare/26"
variable "text" jsonpath "$.entries[24].path" == "/test/bare/26"
jsonpath "$.result.structuredContent.entries[24].identifier" == "{{base}}/test/bare/26"
variable "text" jsonpath "$.entries[24].identifier" == "{{base}}/test/bare/26"
jsonpath "$.result.structuredContent.entries[24].health" == 0
variable "text" jsonpath "$.entries[24].health" == 0
jsonpath "$.result.structuredContent.entries[24].bytes" == 130
variable "text" jsonpath "$.entries[24].bytes" == 130
jsonpath "$.result.structuredContent.entries[24].bytesBundled" == 130
variable "text" jsonpath "$.entries[24].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[24].dependencies" == 0
variable "text" jsonpath "$.entries[24].dependencies" == 0
jsonpath "$.result.structuredContent.entries[24].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[24].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[24].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[24].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[25].name" == "25"
variable "text" jsonpath "$.entries[25].name" == "25"
jsonpath "$.result.structuredContent.entries[25].type" == "schema"
variable "text" jsonpath "$.entries[25].type" == "schema"
jsonpath "$.result.structuredContent.entries[25].path" == "/test/bare/25"
variable "text" jsonpath "$.entries[25].path" == "/test/bare/25"
jsonpath "$.result.structuredContent.entries[25].identifier" == "{{base}}/test/bare/25"
variable "text" jsonpath "$.entries[25].identifier" == "{{base}}/test/bare/25"
jsonpath "$.result.structuredContent.entries[25].health" == 0
variable "text" jsonpath "$.entries[25].health" == 0
jsonpath "$.result.structuredContent.entries[25].bytes" == 130
variable "text" jsonpath "$.entries[25].bytes" == 130
jsonpath "$.result.structuredContent.entries[25].bytesBundled" == 130
variable "text" jsonpath "$.entries[25].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[25].dependencies" == 0
variable "text" jsonpath "$.entries[25].dependencies" == 0
jsonpath "$.result.structuredContent.entries[25].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[25].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[25].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[25].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[26].name" == "24"
variable "text" jsonpath "$.entries[26].name" == "24"
jsonpath "$.result.structuredContent.entries[26].type" == "schema"
variable "text" jsonpath "$.entries[26].type" == "schema"
jsonpath "$.result.structuredContent.entries[26].path" == "/test/bare/24"
variable "text" jsonpath "$.entries[26].path" == "/test/bare/24"
jsonpath "$.result.structuredContent.entries[26].identifier" == "{{base}}/test/bare/24"
variable "text" jsonpath "$.entries[26].identifier" == "{{base}}/test/bare/24"
jsonpath "$.result.structuredContent.entries[26].health" == 0
variable "text" jsonpath "$.entries[26].health" == 0
jsonpath "$.result.structuredContent.entries[26].bytes" == 130
variable "text" jsonpath "$.entries[26].bytes" == 130
jsonpath "$.result.structuredContent.entries[26].bytesBundled" == 130
variable "text" jsonpath "$.entries[26].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[26].dependencies" == 0
variable "text" jsonpath "$.entries[26].dependencies" == 0
jsonpath "$.result.structuredContent.entries[26].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[26].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[26].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[26].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[27].name" == "23"
variable "text" jsonpath "$.entries[27].name" == "23"
jsonpath "$.result.structuredContent.entries[27].type" == "schema"
variable "text" jsonpath "$.entries[27].type" == "schema"
jsonpath "$.result.structuredContent.entries[27].path" == "/test/bare/23"
variable "text" jsonpath "$.entries[27].path" == "/test/bare/23"
jsonpath "$.result.structuredContent.entries[27].identifier" == "{{base}}/test/bare/23"
variable "text" jsonpath "$.entries[27].identifier" == "{{base}}/test/bare/23"
jsonpath "$.result.structuredContent.entries[27].health" == 0
variable "text" jsonpath "$.entries[27].health" == 0
jsonpath "$.result.structuredContent.entries[27].bytes" == 130
variable "text" jsonpath "$.entries[27].bytes" == 130
jsonpath "$.result.structuredContent.entries[27].bytesBundled" == 130
variable "text" jsonpath "$.entries[27].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[27].dependencies" == 0
variable "text" jsonpath "$.entries[27].dependencies" == 0
jsonpath "$.result.structuredContent.entries[27].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[27].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[27].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[27].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[28].name" == "22"
variable "text" jsonpath "$.entries[28].name" == "22"
jsonpath "$.result.structuredContent.entries[28].type" == "schema"
variable "text" jsonpath "$.entries[28].type" == "schema"
jsonpath "$.result.structuredContent.entries[28].path" == "/test/bare/22"
variable "text" jsonpath "$.entries[28].path" == "/test/bare/22"
jsonpath "$.result.structuredContent.entries[28].identifier" == "{{base}}/test/bare/22"
variable "text" jsonpath "$.entries[28].identifier" == "{{base}}/test/bare/22"
jsonpath "$.result.structuredContent.entries[28].health" == 0
variable "text" jsonpath "$.entries[28].health" == 0
jsonpath "$.result.structuredContent.entries[28].bytes" == 130
variable "text" jsonpath "$.entries[28].bytes" == 130
jsonpath "$.result.structuredContent.entries[28].bytesBundled" == 130
variable "text" jsonpath "$.entries[28].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[28].dependencies" == 0
variable "text" jsonpath "$.entries[28].dependencies" == 0
jsonpath "$.result.structuredContent.entries[28].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[28].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[28].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[28].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[29].name" == "21"
variable "text" jsonpath "$.entries[29].name" == "21"
jsonpath "$.result.structuredContent.entries[29].type" == "schema"
variable "text" jsonpath "$.entries[29].type" == "schema"
jsonpath "$.result.structuredContent.entries[29].path" == "/test/bare/21"
variable "text" jsonpath "$.entries[29].path" == "/test/bare/21"
jsonpath "$.result.structuredContent.entries[29].identifier" == "{{base}}/test/bare/21"
variable "text" jsonpath "$.entries[29].identifier" == "{{base}}/test/bare/21"
jsonpath "$.result.structuredContent.entries[29].health" == 0
variable "text" jsonpath "$.entries[29].health" == 0
jsonpath "$.result.structuredContent.entries[29].bytes" == 130
variable "text" jsonpath "$.entries[29].bytes" == 130
jsonpath "$.result.structuredContent.entries[29].bytesBundled" == 130
variable "text" jsonpath "$.entries[29].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[29].dependencies" == 0
variable "text" jsonpath "$.entries[29].dependencies" == 0
jsonpath "$.result.structuredContent.entries[29].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[29].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[29].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[29].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[30].name" == "20"
variable "text" jsonpath "$.entries[30].name" == "20"
jsonpath "$.result.structuredContent.entries[30].type" == "schema"
variable "text" jsonpath "$.entries[30].type" == "schema"
jsonpath "$.result.structuredContent.entries[30].path" == "/test/bare/20"
variable "text" jsonpath "$.entries[30].path" == "/test/bare/20"
jsonpath "$.result.structuredContent.entries[30].identifier" == "{{base}}/test/bare/20"
variable "text" jsonpath "$.entries[30].identifier" == "{{base}}/test/bare/20"
jsonpath "$.result.structuredContent.entries[30].health" == 0
variable "text" jsonpath "$.entries[30].health" == 0
jsonpath "$.result.structuredContent.entries[30].bytes" == 130
variable "text" jsonpath "$.entries[30].bytes" == 130
jsonpath "$.result.structuredContent.entries[30].bytesBundled" == 130
variable "text" jsonpath "$.entries[30].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[30].dependencies" == 0
variable "text" jsonpath "$.entries[30].dependencies" == 0
jsonpath "$.result.structuredContent.entries[30].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[30].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[30].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[30].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[31].name" == "19"
variable "text" jsonpath "$.entries[31].name" == "19"
jsonpath "$.result.structuredContent.entries[31].type" == "schema"
variable "text" jsonpath "$.entries[31].type" == "schema"
jsonpath "$.result.structuredContent.entries[31].path" == "/test/bare/19"
variable "text" jsonpath "$.entries[31].path" == "/test/bare/19"
jsonpath "$.result.structuredContent.entries[31].identifier" == "{{base}}/test/bare/19"
variable "text" jsonpath "$.entries[31].identifier" == "{{base}}/test/bare/19"
jsonpath "$.result.structuredContent.entries[31].health" == 0
variable "text" jsonpath "$.entries[31].health" == 0
jsonpath "$.result.structuredContent.entries[31].bytes" == 130
variable "text" jsonpath "$.entries[31].bytes" == 130
jsonpath "$.result.structuredContent.entries[31].bytesBundled" == 130
variable "text" jsonpath "$.entries[31].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[31].dependencies" == 0
variable "text" jsonpath "$.entries[31].dependencies" == 0
jsonpath "$.result.structuredContent.entries[31].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[31].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[31].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[31].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[32].name" == "18"
variable "text" jsonpath "$.entries[32].name" == "18"
jsonpath "$.result.structuredContent.entries[32].type" == "schema"
variable "text" jsonpath "$.entries[32].type" == "schema"
jsonpath "$.result.structuredContent.entries[32].path" == "/test/bare/18"
variable "text" jsonpath "$.entries[32].path" == "/test/bare/18"
jsonpath "$.result.structuredContent.entries[32].identifier" == "{{base}}/test/bare/18"
variable "text" jsonpath "$.entries[32].identifier" == "{{base}}/test/bare/18"
jsonpath "$.result.structuredContent.entries[32].health" == 0
variable "text" jsonpath "$.entries[32].health" == 0
jsonpath "$.result.structuredContent.entries[32].bytes" == 130
variable "text" jsonpath "$.entries[32].bytes" == 130
jsonpath "$.result.structuredContent.entries[32].bytesBundled" == 130
variable "text" jsonpath "$.entries[32].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[32].dependencies" == 0
variable "text" jsonpath "$.entries[32].dependencies" == 0
jsonpath "$.result.structuredContent.entries[32].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[32].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[32].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[32].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[33].name" == "17"
variable "text" jsonpath "$.entries[33].name" == "17"
jsonpath "$.result.structuredContent.entries[33].type" == "schema"
variable "text" jsonpath "$.entries[33].type" == "schema"
jsonpath "$.result.structuredContent.entries[33].path" == "/test/bare/17"
variable "text" jsonpath "$.entries[33].path" == "/test/bare/17"
jsonpath "$.result.structuredContent.entries[33].identifier" == "{{base}}/test/bare/17"
variable "text" jsonpath "$.entries[33].identifier" == "{{base}}/test/bare/17"
jsonpath "$.result.structuredContent.entries[33].health" == 0
variable "text" jsonpath "$.entries[33].health" == 0
jsonpath "$.result.structuredContent.entries[33].bytes" == 130
variable "text" jsonpath "$.entries[33].bytes" == 130
jsonpath "$.result.structuredContent.entries[33].bytesBundled" == 130
variable "text" jsonpath "$.entries[33].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[33].dependencies" == 0
variable "text" jsonpath "$.entries[33].dependencies" == 0
jsonpath "$.result.structuredContent.entries[33].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[33].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[33].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[33].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[34].name" == "16"
variable "text" jsonpath "$.entries[34].name" == "16"
jsonpath "$.result.structuredContent.entries[34].type" == "schema"
variable "text" jsonpath "$.entries[34].type" == "schema"
jsonpath "$.result.structuredContent.entries[34].path" == "/test/bare/16"
variable "text" jsonpath "$.entries[34].path" == "/test/bare/16"
jsonpath "$.result.structuredContent.entries[34].identifier" == "{{base}}/test/bare/16"
variable "text" jsonpath "$.entries[34].identifier" == "{{base}}/test/bare/16"
jsonpath "$.result.structuredContent.entries[34].health" == 0
variable "text" jsonpath "$.entries[34].health" == 0
jsonpath "$.result.structuredContent.entries[34].bytes" == 130
variable "text" jsonpath "$.entries[34].bytes" == 130
jsonpath "$.result.structuredContent.entries[34].bytesBundled" == 130
variable "text" jsonpath "$.entries[34].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[34].dependencies" == 0
variable "text" jsonpath "$.entries[34].dependencies" == 0
jsonpath "$.result.structuredContent.entries[34].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[34].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[34].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[34].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[35].name" == "15"
variable "text" jsonpath "$.entries[35].name" == "15"
jsonpath "$.result.structuredContent.entries[35].type" == "schema"
variable "text" jsonpath "$.entries[35].type" == "schema"
jsonpath "$.result.structuredContent.entries[35].path" == "/test/bare/15"
variable "text" jsonpath "$.entries[35].path" == "/test/bare/15"
jsonpath "$.result.structuredContent.entries[35].identifier" == "{{base}}/test/bare/15"
variable "text" jsonpath "$.entries[35].identifier" == "{{base}}/test/bare/15"
jsonpath "$.result.structuredContent.entries[35].health" == 0
variable "text" jsonpath "$.entries[35].health" == 0
jsonpath "$.result.structuredContent.entries[35].bytes" == 130
variable "text" jsonpath "$.entries[35].bytes" == 130
jsonpath "$.result.structuredContent.entries[35].bytesBundled" == 130
variable "text" jsonpath "$.entries[35].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[35].dependencies" == 0
variable "text" jsonpath "$.entries[35].dependencies" == 0
jsonpath "$.result.structuredContent.entries[35].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[35].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[35].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[35].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[36].name" == "14"
variable "text" jsonpath "$.entries[36].name" == "14"
jsonpath "$.result.structuredContent.entries[36].type" == "schema"
variable "text" jsonpath "$.entries[36].type" == "schema"
jsonpath "$.result.structuredContent.entries[36].path" == "/test/bare/14"
variable "text" jsonpath "$.entries[36].path" == "/test/bare/14"
jsonpath "$.result.structuredContent.entries[36].identifier" == "{{base}}/test/bare/14"
variable "text" jsonpath "$.entries[36].identifier" == "{{base}}/test/bare/14"
jsonpath "$.result.structuredContent.entries[36].health" == 0
variable "text" jsonpath "$.entries[36].health" == 0
jsonpath "$.result.structuredContent.entries[36].bytes" == 130
variable "text" jsonpath "$.entries[36].bytes" == 130
jsonpath "$.result.structuredContent.entries[36].bytesBundled" == 130
variable "text" jsonpath "$.entries[36].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[36].dependencies" == 0
variable "text" jsonpath "$.entries[36].dependencies" == 0
jsonpath "$.result.structuredContent.entries[36].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[36].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[36].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[36].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[37].name" == "13"
variable "text" jsonpath "$.entries[37].name" == "13"
jsonpath "$.result.structuredContent.entries[37].type" == "schema"
variable "text" jsonpath "$.entries[37].type" == "schema"
jsonpath "$.result.structuredContent.entries[37].path" == "/test/bare/13"
variable "text" jsonpath "$.entries[37].path" == "/test/bare/13"
jsonpath "$.result.structuredContent.entries[37].identifier" == "{{base}}/test/bare/13"
variable "text" jsonpath "$.entries[37].identifier" == "{{base}}/test/bare/13"
jsonpath "$.result.structuredContent.entries[37].health" == 0
variable "text" jsonpath "$.entries[37].health" == 0
jsonpath "$.result.structuredContent.entries[37].bytes" == 130
variable "text" jsonpath "$.entries[37].bytes" == 130
jsonpath "$.result.structuredContent.entries[37].bytesBundled" == 130
variable "text" jsonpath "$.entries[37].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[37].dependencies" == 0
variable "text" jsonpath "$.entries[37].dependencies" == 0
jsonpath "$.result.structuredContent.entries[37].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[37].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[37].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[37].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[38].name" == "12"
variable "text" jsonpath "$.entries[38].name" == "12"
jsonpath "$.result.structuredContent.entries[38].type" == "schema"
variable "text" jsonpath "$.entries[38].type" == "schema"
jsonpath "$.result.structuredContent.entries[38].path" == "/test/bare/12"
variable "text" jsonpath "$.entries[38].path" == "/test/bare/12"
jsonpath "$.result.structuredContent.entries[38].identifier" == "{{base}}/test/bare/12"
variable "text" jsonpath "$.entries[38].identifier" == "{{base}}/test/bare/12"
jsonpath "$.result.structuredContent.entries[38].health" == 0
variable "text" jsonpath "$.entries[38].health" == 0
jsonpath "$.result.structuredContent.entries[38].bytes" == 130
variable "text" jsonpath "$.entries[38].bytes" == 130
jsonpath "$.result.structuredContent.entries[38].bytesBundled" == 130
variable "text" jsonpath "$.entries[38].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[38].dependencies" == 0
variable "text" jsonpath "$.entries[38].dependencies" == 0
jsonpath "$.result.structuredContent.entries[38].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[38].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[38].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[38].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[39].name" == "11"
variable "text" jsonpath "$.entries[39].name" == "11"
jsonpath "$.result.structuredContent.entries[39].type" == "schema"
variable "text" jsonpath "$.entries[39].type" == "schema"
jsonpath "$.result.structuredContent.entries[39].path" == "/test/bare/11"
variable "text" jsonpath "$.entries[39].path" == "/test/bare/11"
jsonpath "$.result.structuredContent.entries[39].identifier" == "{{base}}/test/bare/11"
variable "text" jsonpath "$.entries[39].identifier" == "{{base}}/test/bare/11"
jsonpath "$.result.structuredContent.entries[39].health" == 0
variable "text" jsonpath "$.entries[39].health" == 0
jsonpath "$.result.structuredContent.entries[39].bytes" == 130
variable "text" jsonpath "$.entries[39].bytes" == 130
jsonpath "$.result.structuredContent.entries[39].bytesBundled" == 130
variable "text" jsonpath "$.entries[39].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[39].dependencies" == 0
variable "text" jsonpath "$.entries[39].dependencies" == 0
jsonpath "$.result.structuredContent.entries[39].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[39].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[39].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[39].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[40].name" == "10"
variable "text" jsonpath "$.entries[40].name" == "10"
jsonpath "$.result.structuredContent.entries[40].type" == "schema"
variable "text" jsonpath "$.entries[40].type" == "schema"
jsonpath "$.result.structuredContent.entries[40].path" == "/test/bare/10"
variable "text" jsonpath "$.entries[40].path" == "/test/bare/10"
jsonpath "$.result.structuredContent.entries[40].identifier" == "{{base}}/test/bare/10"
variable "text" jsonpath "$.entries[40].identifier" == "{{base}}/test/bare/10"
jsonpath "$.result.structuredContent.entries[40].health" == 0
variable "text" jsonpath "$.entries[40].health" == 0
jsonpath "$.result.structuredContent.entries[40].bytes" == 130
variable "text" jsonpath "$.entries[40].bytes" == 130
jsonpath "$.result.structuredContent.entries[40].bytesBundled" == 130
variable "text" jsonpath "$.entries[40].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[40].dependencies" == 0
variable "text" jsonpath "$.entries[40].dependencies" == 0
jsonpath "$.result.structuredContent.entries[40].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[40].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[40].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[40].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[41].name" == "01"
variable "text" jsonpath "$.entries[41].name" == "01"
jsonpath "$.result.structuredContent.entries[41].type" == "schema"
variable "text" jsonpath "$.entries[41].type" == "schema"
jsonpath "$.result.structuredContent.entries[41].path" == "/test/bare/01"
variable "text" jsonpath "$.entries[41].path" == "/test/bare/01"
jsonpath "$.result.structuredContent.entries[41].identifier" == "{{base}}/test/bare/01"
variable "text" jsonpath "$.entries[41].identifier" == "{{base}}/test/bare/01"
jsonpath "$.result.structuredContent.entries[41].health" == 0
variable "text" jsonpath "$.entries[41].health" == 0
jsonpath "$.result.structuredContent.entries[41].bytes" == 130
variable "text" jsonpath "$.entries[41].bytes" == 130
jsonpath "$.result.structuredContent.entries[41].bytesBundled" == 130
variable "text" jsonpath "$.entries[41].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[41].dependencies" == 0
variable "text" jsonpath "$.entries[41].dependencies" == 0
jsonpath "$.result.structuredContent.entries[41].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[41].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[41].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[41].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[42].name" == "02"
variable "text" jsonpath "$.entries[42].name" == "02"
jsonpath "$.result.structuredContent.entries[42].type" == "schema"
variable "text" jsonpath "$.entries[42].type" == "schema"
jsonpath "$.result.structuredContent.entries[42].path" == "/test/bare/02"
variable "text" jsonpath "$.entries[42].path" == "/test/bare/02"
jsonpath "$.result.structuredContent.entries[42].identifier" == "{{base}}/test/bare/02"
variable "text" jsonpath "$.entries[42].identifier" == "{{base}}/test/bare/02"
jsonpath "$.result.structuredContent.entries[42].health" == 0
variable "text" jsonpath "$.entries[42].health" == 0
jsonpath "$.result.structuredContent.entries[42].bytes" == 130
variable "text" jsonpath "$.entries[42].bytes" == 130
jsonpath "$.result.structuredContent.entries[42].bytesBundled" == 130
variable "text" jsonpath "$.entries[42].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[42].dependencies" == 0
variable "text" jsonpath "$.entries[42].dependencies" == 0
jsonpath "$.result.structuredContent.entries[42].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[42].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[42].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[42].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[43].name" == "03"
variable "text" jsonpath "$.entries[43].name" == "03"
jsonpath "$.result.structuredContent.entries[43].type" == "schema"
variable "text" jsonpath "$.entries[43].type" == "schema"
jsonpath "$.result.structuredContent.entries[43].path" == "/test/bare/03"
variable "text" jsonpath "$.entries[43].path" == "/test/bare/03"
jsonpath "$.result.structuredContent.entries[43].identifier" == "{{base}}/test/bare/03"
variable "text" jsonpath "$.entries[43].identifier" == "{{base}}/test/bare/03"
jsonpath "$.result.structuredContent.entries[43].health" == 0
variable "text" jsonpath "$.entries[43].health" == 0
jsonpath "$.result.structuredContent.entries[43].bytes" == 130
variable "text" jsonpath "$.entries[43].bytes" == 130
jsonpath "$.result.structuredContent.entries[43].bytesBundled" == 130
variable "text" jsonpath "$.entries[43].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[43].dependencies" == 0
variable "text" jsonpath "$.entries[43].dependencies" == 0
jsonpath "$.result.structuredContent.entries[43].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[43].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[43].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[43].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[44].name" == "04"
variable "text" jsonpath "$.entries[44].name" == "04"
jsonpath "$.result.structuredContent.entries[44].type" == "schema"
variable "text" jsonpath "$.entries[44].type" == "schema"
jsonpath "$.result.structuredContent.entries[44].path" == "/test/bare/04"
variable "text" jsonpath "$.entries[44].path" == "/test/bare/04"
jsonpath "$.result.structuredContent.entries[44].identifier" == "{{base}}/test/bare/04"
variable "text" jsonpath "$.entries[44].identifier" == "{{base}}/test/bare/04"
jsonpath "$.result.structuredContent.entries[44].health" == 0
variable "text" jsonpath "$.entries[44].health" == 0
jsonpath "$.result.structuredContent.entries[44].bytes" == 130
variable "text" jsonpath "$.entries[44].bytes" == 130
jsonpath "$.result.structuredContent.entries[44].bytesBundled" == 130
variable "text" jsonpath "$.entries[44].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[44].dependencies" == 0
variable "text" jsonpath "$.entries[44].dependencies" == 0
jsonpath "$.result.structuredContent.entries[44].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[44].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[44].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[44].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[45].name" == "05"
variable "text" jsonpath "$.entries[45].name" == "05"
jsonpath "$.result.structuredContent.entries[45].type" == "schema"
variable "text" jsonpath "$.entries[45].type" == "schema"
jsonpath "$.result.structuredContent.entries[45].path" == "/test/bare/05"
variable "text" jsonpath "$.entries[45].path" == "/test/bare/05"
jsonpath "$.result.structuredContent.entries[45].identifier" == "{{base}}/test/bare/05"
variable "text" jsonpath "$.entries[45].identifier" == "{{base}}/test/bare/05"
jsonpath "$.result.structuredContent.entries[45].health" == 0
variable "text" jsonpath "$.entries[45].health" == 0
jsonpath "$.result.structuredContent.entries[45].bytes" == 130
variable "text" jsonpath "$.entries[45].bytes" == 130
jsonpath "$.result.structuredContent.entries[45].bytesBundled" == 130
variable "text" jsonpath "$.entries[45].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[45].dependencies" == 0
variable "text" jsonpath "$.entries[45].dependencies" == 0
jsonpath "$.result.structuredContent.entries[45].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[45].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[45].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[45].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[46].name" == "06"
variable "text" jsonpath "$.entries[46].name" == "06"
jsonpath "$.result.structuredContent.entries[46].type" == "schema"
variable "text" jsonpath "$.entries[46].type" == "schema"
jsonpath "$.result.structuredContent.entries[46].path" == "/test/bare/06"
variable "text" jsonpath "$.entries[46].path" == "/test/bare/06"
jsonpath "$.result.structuredContent.entries[46].identifier" == "{{base}}/test/bare/06"
variable "text" jsonpath "$.entries[46].identifier" == "{{base}}/test/bare/06"
jsonpath "$.result.structuredContent.entries[46].health" == 0
variable "text" jsonpath "$.entries[46].health" == 0
jsonpath "$.result.structuredContent.entries[46].bytes" == 130
variable "text" jsonpath "$.entries[46].bytes" == 130
jsonpath "$.result.structuredContent.entries[46].bytesBundled" == 130
variable "text" jsonpath "$.entries[46].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[46].dependencies" == 0
variable "text" jsonpath "$.entries[46].dependencies" == 0
jsonpath "$.result.structuredContent.entries[46].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[46].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[46].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[46].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[47].name" == "07"
variable "text" jsonpath "$.entries[47].name" == "07"
jsonpath "$.result.structuredContent.entries[47].type" == "schema"
variable "text" jsonpath "$.entries[47].type" == "schema"
jsonpath "$.result.structuredContent.entries[47].path" == "/test/bare/07"
variable "text" jsonpath "$.entries[47].path" == "/test/bare/07"
jsonpath "$.result.structuredContent.entries[47].identifier" == "{{base}}/test/bare/07"
variable "text" jsonpath "$.entries[47].identifier" == "{{base}}/test/bare/07"
jsonpath "$.result.structuredContent.entries[47].health" == 0
variable "text" jsonpath "$.entries[47].health" == 0
jsonpath "$.result.structuredContent.entries[47].bytes" == 130
variable "text" jsonpath "$.entries[47].bytes" == 130
jsonpath "$.result.structuredContent.entries[47].bytesBundled" == 130
variable "text" jsonpath "$.entries[47].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[47].dependencies" == 0
variable "text" jsonpath "$.entries[47].dependencies" == 0
jsonpath "$.result.structuredContent.entries[47].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[47].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[47].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[47].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[48].name" == "08"
variable "text" jsonpath "$.entries[48].name" == "08"
jsonpath "$.result.structuredContent.entries[48].type" == "schema"
variable "text" jsonpath "$.entries[48].type" == "schema"
jsonpath "$.result.structuredContent.entries[48].path" == "/test/bare/08"
variable "text" jsonpath "$.entries[48].path" == "/test/bare/08"
jsonpath "$.result.structuredContent.entries[48].identifier" == "{{base}}/test/bare/08"
variable "text" jsonpath "$.entries[48].identifier" == "{{base}}/test/bare/08"
jsonpath "$.result.structuredContent.entries[48].health" == 0
variable "text" jsonpath "$.entries[48].health" == 0
jsonpath "$.result.structuredContent.entries[48].bytes" == 130
variable "text" jsonpath "$.entries[48].bytes" == 130
jsonpath "$.result.structuredContent.entries[48].bytesBundled" == 130
variable "text" jsonpath "$.entries[48].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[48].dependencies" == 0
variable "text" jsonpath "$.entries[48].dependencies" == 0
jsonpath "$.result.structuredContent.entries[48].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[48].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[48].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[48].dialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[49].name" == "09"
variable "text" jsonpath "$.entries[49].name" == "09"
jsonpath "$.result.structuredContent.entries[49].type" == "schema"
variable "text" jsonpath "$.entries[49].type" == "schema"
jsonpath "$.result.structuredContent.entries[49].path" == "/test/bare/09"
variable "text" jsonpath "$.entries[49].path" == "/test/bare/09"
jsonpath "$.result.structuredContent.entries[49].identifier" == "{{base}}/test/bare/09"
variable "text" jsonpath "$.entries[49].identifier" == "{{base}}/test/bare/09"
jsonpath "$.result.structuredContent.entries[49].health" == 0
variable "text" jsonpath "$.entries[49].health" == 0
jsonpath "$.result.structuredContent.entries[49].bytes" == 130
variable "text" jsonpath "$.entries[49].bytes" == 130
jsonpath "$.result.structuredContent.entries[49].bytesBundled" == 130
variable "text" jsonpath "$.entries[49].bytesBundled" == 130
jsonpath "$.result.structuredContent.entries[49].dependencies" == 0
variable "text" jsonpath "$.entries[49].dependencies" == 0
jsonpath "$.result.structuredContent.entries[49].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[49].baseDialect" == "https://json-schema.org/draft/2020-12/schema"
jsonpath "$.result.structuredContent.entries[49].dialect" == "https://json-schema.org/draft/2020-12/schema"
variable "text" jsonpath "$.entries[49].dialect" == "https://json-schema.org/draft/2020-12/schema"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Round-trip: capture a schema's resource_link URI from a directory listing
# and read the schema back through MCP `resources/read`.

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 106,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
text: jsonpath "$.result.content[0].text"
schema_uri: jsonpath "$.result.content[1].uri"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 106
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 2
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[1].type" == "resource_link"
jsonpath "$.result.content[1].uri" == "{{base}}/test/object"
jsonpath "$.result.content[1].name" == "My Object"
jsonpath "$.result.content[1].description" == "An Object Example"
jsonpath "$.result.content[1].mimeType" == "application/schema+json"
jsonpath "$.result.structuredContent.path" == "/test"
variable "text" jsonpath "$.path" == "/test"
jsonpath "$.result.structuredContent.url" == "{{base}}/test"
variable "text" jsonpath "$.url" == "{{base}}/test"
jsonpath "$.result.structuredContent.schemas" == 68
variable "text" jsonpath "$.schemas" == 68
jsonpath "$.result.structuredContent.health" == 15
variable "text" jsonpath "$.health" == 15
jsonpath "$.result.structuredContent.entries" count == 6
variable "text" jsonpath "$.entries" count == 6
jsonpath "$.result.structuredContent.entries[5].name" == "object"
variable "text" jsonpath "$.entries[5].name" == "object"
jsonpath "$.result.structuredContent.entries[5].type" == "schema"
variable "text" jsonpath "$.entries[5].type" == "schema"
jsonpath "$.result.structuredContent.entries[5].identifier" == "{{base}}/test/object"
variable "text" jsonpath "$.entries[5].identifier" == "{{base}}/test/object"
jsonpath "$.result.structuredContent.entries[5].title" == "My Object"
variable "text" jsonpath "$.entries[5].title" == "My Object"
jsonpath "$.result.structuredContent.entries[5].description" == "An Object Example"
variable "text" jsonpath "$.entries[5].description" == "An Object Example"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

GET {{base}}/test/object
HTTP 200
[Captures]
schema_text: body

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 107,
  "method": "resources/read",
  "params": {
    "uri": "{{schema_uri}}"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 107
jsonpath "$.result.contents" count == 1
jsonpath "$.result.contents[0].uri" == "{{schema_uri}}"
jsonpath "$.result.contents[0].mimeType" == "application/schema+json"
jsonpath "$.result.contents[0].text" == "{{schema_text}}"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Subdirectory listing with title and description

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 105,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "self" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 105
jsonpath "$.result.isError" == false
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.structuredContent.path" == "/self"
variable "text" jsonpath "$.path" == "/self"
jsonpath "$.result.structuredContent.url" == "{{base}}/self"
variable "text" jsonpath "$.url" == "{{base}}/self"
jsonpath "$.result.structuredContent.title" == "Self"
variable "text" jsonpath "$.title" == "Self"
jsonpath "$.result.structuredContent.description" == "The schemas that define the current version of this instance"
variable "text" jsonpath "$.description" == "The schemas that define the current version of this instance"
jsonpath "$.result.structuredContent.breadcrumb" count == 1
variable "text" jsonpath "$.breadcrumb" count == 1
jsonpath "$.result.structuredContent.breadcrumb[0].name" == "self"
variable "text" jsonpath "$.breadcrumb[0].name" == "self"
jsonpath "$.result.structuredContent.breadcrumb[0].path" == "/self/"
variable "text" jsonpath "$.breadcrumb[0].path" == "/self/"
jsonpath "$.result.structuredContent.schemas" == 45
variable "text" jsonpath "$.schemas" == 45
jsonpath "$.result.structuredContent.health" == 100
variable "text" jsonpath "$.health" == 100
jsonpath "$.result.structuredContent.entries" count == 1
variable "text" jsonpath "$.entries" count == 1
jsonpath "$.result.structuredContent.entries[0].name" == "v1"
variable "text" jsonpath "$.entries[0].name" == "v1"
jsonpath "$.result.structuredContent.entries[0].type" == "directory"
variable "text" jsonpath "$.entries[0].type" == "directory"
jsonpath "$.result.structuredContent.entries[0].health" == 100
variable "text" jsonpath "$.entries[0].health" == 100
jsonpath "$.result.structuredContent.entries[0].schemas" == 45
variable "text" jsonpath "$.entries[0].schemas" == 45
jsonpath "$.result.structuredContent.entries[0].path" == "/self/v1/"
variable "text" jsonpath "$.entries[0].path" == "/self/v1/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Non-existent path

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 110,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "doesnotexist" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 110
jsonpath "$.result.isError" == true
jsonpath "$.result.content" count == 1
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Directory not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Path points at a schema, not a directory

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 111,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test/object" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 111
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Directory not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Absolute path

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 112,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "/test" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 112
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Bare ".." segment

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 113,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": ".." }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 113
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Mid-path ".." segment

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 114,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test/.." }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 114
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Bare "." segment

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 115,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "." }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 115
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Mid-path "." segment

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 116,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test/." }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 116
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Percent-encoded ".." treated as literal segment name

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 117,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "..%2F..%2Fetc" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 117
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Directory not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Backslash in path

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 118,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test\\bare" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 118
jsonpath "$.result.isError" == true
jsonpath "$.result.content[0].type" == "text"
jsonpath "$.result.content[0].text" == "Directory not found"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Path is an integer

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 119,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": 42 }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 119
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Path is an array

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 120,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": [] }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 120
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Extra property

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 121,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": { "path": "test", "extra": "foo" }
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 121
jsonpath "$.error.code" == -32602
jsonpath "$.error.message" == "Invalid params"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Arguments is null

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 122,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": null
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 122
jsonpath "$.error.code" == -32600
jsonpath "$.error.message" == "Invalid Request"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# Arguments is a string

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": 123,
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": "test"
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == 123
jsonpath "$.error.code" == -32600
jsonpath "$.error.message" == "Invalid Request"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true

# String id

POST {{base}}/self/v1/mcp
Content-Type: application/json
```
{
  "jsonrpc": "2.0",
  "id": "list-call-string-id",
  "method": "tools/call",
  "params": {
    "name": "list_directory",
    "arguments": {}
  }
}
```
HTTP 200
Content-Type: application/json
Access-Control-Allow-Origin: {{base}}
Link: </self/v1/schemas/mcp/response>; rel="describedby"
[Captures]
last_response: body
schema_path: header "Link" regex "<([^>]+)>"
text: jsonpath "$.result.content[0].text"
[Asserts]
jsonpath "$.jsonrpc" == "2.0"
jsonpath "$.id" == "list-call-string-id"
jsonpath "$.result.isError" == false
jsonpath "$.result.structuredContent.path" == "/"
variable "text" jsonpath "$.path" == "/"

POST {{base}}/self/v1/api/schemas/evaluate{{schema_path}}
```
{{last_response}}
```
HTTP 200
[Asserts]
jsonpath "$.valid" == true
