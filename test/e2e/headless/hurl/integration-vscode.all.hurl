GET {{base}}/test/schemas/bundling-single.json
User-Agent: Visual Studio Code (desktop)
HTTP 200
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "Bundling",
  "description": "A bundling example",
  "properties": {
    "foo": {
      "$ref": "#/definitions/http:~1~1localhost:8000~1test~1schemas~1no-id"
    }
  },
  "definitions": {
    "http://localhost:8000/test/schemas/no-id": {
      "$schema": "http://json-schema.org/draft-07/schema#",
      "type": "string"
    }
  }
}

GET {{base}}/test/schemas/bundling-single.json
User-Agent: VSCodium (desktop)
HTTP 200
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "Bundling",
  "description": "A bundling example",
  "properties": {
    "foo": {
      "$ref": "#/definitions/http:~1~1localhost:8000~1test~1schemas~1no-id"
    }
  },
  "definitions": {
    "http://localhost:8000/test/schemas/no-id": {
      "$schema": "http://json-schema.org/draft-07/schema#",
      "type": "string"
    }
  }
}

GET {{base}}/test/schemas/bundling-single.json?bundle=0
User-Agent: VSCodium (desktop)
HTTP 200
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "Bundling",
  "description": "A bundling example",
  "properties": {
    "foo": {
      "$ref": "#/definitions/http:~1~1localhost:8000~1test~1schemas~1no-id"
    }
  },
  "definitions": {
    "http://localhost:8000/test/schemas/no-id": {
      "$schema": "http://json-schema.org/draft-07/schema#",
      "type": "string"
    }
  }
}

GET {{base}}/test/schemas/bundling-single.json?unidentify=0
User-Agent: VSCodium (desktop)
HTTP 200
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "Bundling",
  "description": "A bundling example",
  "properties": {
    "foo": {
      "$ref": "#/definitions/http:~1~1localhost:8000~1test~1schemas~1no-id"
    }
  },
  "definitions": {
    "http://localhost:8000/test/schemas/no-id": {
      "$schema": "http://json-schema.org/draft-07/schema#",
      "type": "string"
    }
  }
}
