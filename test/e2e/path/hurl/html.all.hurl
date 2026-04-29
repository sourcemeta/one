GET {{base}}/v1/catalog
Accept: text/html
HTTP 200
Content-Type: text/html
[Asserts]
xpath "string(/html/head/title)" == "Sourcemeta Schemas"
xpath "boolean(//h6[contains(text(), 'Special directories')])" == true
xpath "boolean(//h6[contains(text(), 'Special directories')]/following-sibling::table[1]//a[@href='/v1/catalog/self/'])" == true
xpath "count(//h6[contains(text(), 'Special directories')]/preceding-sibling::table//a[@href='/v1/catalog/self/'])" == 0
