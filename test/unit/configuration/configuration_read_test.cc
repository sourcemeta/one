#include <gtest/gtest.h>

#include <sourcemeta/one/configuration.h>

#include <string>        // std::string
#include <unordered_set> // std::unordered_set

auto replace_all(std::string &text, const std::string &from,
                 const std::string &to) -> void {
  assert(!from.empty());
  std::size_t cursor{0};
  while ((cursor = text.find(from, cursor)) != std::string::npos) {
    text.replace(cursor, from.length(), to);
    cursor += to.length();
  }
}

TEST(Configuration_read, read_valid_001) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_001.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "url": "http://localhost:8000",
    "html": {
      "name": "Title",
      "description": "Description"
    },
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json",
                "baseUri": "http://localhost:8000"
              }
            }
          }
        }
      },
      "example": {
        "title": "Sourcemeta",
        "description": "My description",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "extension": {
            "title": "Test",
            "baseUri": "https://example.com/extension",
            "path": "STUB_DIRECTORY/schemas/example/extension",
            "x-sourcemeta-one:path": "STUB_DIRECTORY/read_valid_001.json",
            "defaultDialect": "http://json-schema.org/draft-07/schema#",
            "resolve": {
              "https://other.com/single.json": "/foo.json"
            }
          }
        }
      },
      "test": {
        "title": "A sample schema folder",
        "description": "For testing purposes",
        "github": "sourcemeta/one"
      }
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  replace_all(text, "BASE_URI", "http://localhost:8000");
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_valid_002) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_002.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "url": "http://localhost:8000",
    "html": {
      "name": "Sourcemeta",
      "description": "The next-generation JSON Schema platform"
    },
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json",
                "baseUri": "http://localhost:8000"
              }
            }
          }
        }
      },
      "example": {
        "title": "Nested",
        "contents": {
          "nested": {
            "baseUri": "https://example.com/extension",
            "path": "STUB_DIRECTORY/folder/schemas/example/extension",
            "x-sourcemeta-one:path": "STUB_DIRECTORY/folder/jsonschema.json",
            "defaultDialect": "http://json-schema.org/draft-07/schema#"
          }
        }
      }
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  replace_all(text, "BASE_URI", "http://localhost:8000");
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_valid_003) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_003.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "url": "http://localhost:8000",
    "html": {
      "name": "Sourcemeta",
      "description": "The next-generation JSON Schema platform"
    },
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json",
                "baseUri": "http://localhost:8000"
              }
            }
          }
        }
      },
      "example": {
        "title": "Nested without file name",
        "contents": {
          "nested": {
            "baseUri": "https://example.com/extension",
            "path": "STUB_DIRECTORY/folder/schemas/example/extension",
            "x-sourcemeta-one:path": "STUB_DIRECTORY/folder/jsonschema.json",
            "defaultDialect": "http://json-schema.org/draft-07/schema#"
          }
        }
      }
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_valid_004) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_004.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "url": "http://localhost:8000",
    "html": {
      "name": "Sourcemeta",
      "description": "The next-generation JSON Schema platform"
    },
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json",
                "baseUri": "http://localhost:8000"
              }
            }
          }
        }
      },
      "other": {
        "title": "Other"
      },
      "example": {
        "title": "Nested",
        "contents": {
          "nested": {
            "baseUri": "https://example.com/extension",
            "path": "STUB_DIRECTORY/folder/schemas/example/extension",
            "x-sourcemeta-one:path": "STUB_DIRECTORY/folder/jsonschema.json",
            "defaultDialect": "http://json-schema.org/draft-07/schema#"
          }
        }
      }
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_valid_005) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_005.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "url": "http://localhost:8000",
    "html": {
      "name": "Sourcemeta",
      "description": "The next-generation JSON Schema platform"
    },
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json",
                "baseUri": "http://localhost:8000"
              }
            }
          }
        }
      },
      "other": {
        "title": "Other"
      },
      "example": {
        "title": "Nested",
        "contents": {
          "nested": {
            "baseUri": "https://example.com/extension",
            "path": "STUB_DIRECTORY/folder/schemas/example/extension",
            "x-sourcemeta-one:path": "STUB_DIRECTORY/folder/jsonschema.json",
            "defaultDialect": "http://json-schema.org/draft-07/schema#"
          }
        }
      }
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_valid_006) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_006.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "url": "http://localhost:8000",
    "html": {
      "name": "Sourcemeta",
      "description": "The next-generation JSON Schema platform"
    },
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json",
                "baseUri": "http://localhost:8000"
              }
            }
          }
        }
      },
      "here": {
        "contents": {
          "test": {
            "title": "Imported utility"
          }
        }
      }
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_valid_007) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_007.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "url": "http://localhost:8000",
    "html": {
      "name": "Sourcemeta",
      "description": "The next-generation JSON Schema platform"
    },
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json",
                "baseUri": "http://localhost:8000"
              }
            }
          }
        }
      },
      "here": {
        "title": "With standard name"
      }
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_valid_008) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_008.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "url": "http://localhost:8000",
    "html": false,
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json",
                "baseUri": "http://localhost:8000"
              }
            }
          }
        }
      },
      "test": {
        "title": "A sample schema folder",
        "description": "For testing purposes",
        "github": "sourcemeta/one"
      }
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_valid_009) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_009.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "url": "http://localhost:8000",
    "html": {
      "name": "Sourcemeta",
      "description": "The next-generation JSON Schema platform"
    },
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json",
                "baseUri": "http://localhost:8000"
              }
            }
          }
        }
      },
      "test": {
        "title": "A sample schema folder",
        "description": "For testing purposes",
        "github": "sourcemeta/one"
      }
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_valid_010) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_010.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "url": "http://localhost:8000",
    "html": {
      "name": "Sourcemeta",
      "description": "The next-generation JSON Schema platform"
    },
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json",
                "baseUri": "http://localhost:8000"
              }
            }
          }
        }
      },
      "test": {
        "title": "A sample schema folder",
        "description": "For testing purposes",
        "github": "sourcemeta/one"
      }
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_valid_011) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_011.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "url": "http://localhost:8000",
    "html": false,
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json",
                "baseUri": "http://localhost:8000"
              }
            }
          }
        }
      },
      "test": {
        "title": "A sample schema folder",
        "description": "For testing purposes",
        "github": "sourcemeta/one"
      }
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_valid_012) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_012.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json"
              }
            }
          }
        }
      }
    },
    "include": "./read_valid_001.json",
    "html": {
      "name": "Sourcemeta",
      "description": "The next-generation JSON Schema platform"
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_valid_013) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_013.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "url": "http://localhost:8000",
    "html": {
      "name": "Title",
      "description": "Description"
    },
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json",
                "baseUri": "http://localhost:8000"
              }
            }
          }
        }
      },
      "example": {
        "title": "Test",
        "baseUri": "https://example.com/extension",
        "path": "STUB_DIRECTORY/schemas/example/extension",
        "x-sourcemeta-one:path": "STUB_DIRECTORY/read_valid_013.json",
        "defaultDialect": "http://json-schema.org/draft-07/schema#",
        "resolve": {
          "https://other.com/single.json": "/foo.json"
        },
        "include": "./read_partial_001.json"
      }
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_valid_014) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_014.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "url": "http://localhost:8000",
    "html": {
      "name": "Title",
      "description": "Description"
    },
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json",
                "baseUri": "http://localhost:8000"
              }
            }
          }
        }
      },
      "example": {
        "title": "Test",
        "baseUri": "https://example.com/extension",
        "path": "STUB_DIRECTORY/schemas/example/extension",
        "x-sourcemeta-one:path": "STUB_DIRECTORY/read_valid_014.json",
        "defaultDialect": "http://json-schema.org/draft-07/schema#",
        "resolve": {
          "https://other.com/single.json": "/foo.json"
        },
        "contents": {
          "foo": {
            "include": "./read_partial_001.json"
          }
        }
      }
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_valid_015) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_015.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};

  std::string text{R"JSON({
    "url": "http://localhost:8000",
    "html": {
      "name": "Title",
      "description": "Description"
    },
    "contents": {
      "self": {
        "title": "Self",
        "description": "The schemas that define the current version of this instance",
        "email": "hello@sourcemeta.com",
        "github": "sourcemeta/one",
        "website": "https://www.sourcemeta.com",
        "contents": {
          "v1": {
            "contents": {
              "schemas": {
                "path": "COLLECTIONS_DIRECTORY/self/v1/schemas",
                "x-sourcemeta-one:path": "COLLECTIONS_DIRECTORY/self/v1/jsonschema.json",
                "baseUri": "http://localhost:8000"
              }
            }
          }
        }
      },
      "example": {
        "path": "STUB_DIRECTORY/schemas/example/extension",
        "x-sourcemeta-one:path": "STUB_DIRECTORY/read_valid_015.json",
        "baseUri": "http://localhost:8000"
      }
    }
  })JSON"};

  replace_all(text, "STUB_DIRECTORY", STUB_DIRECTORY);
  replace_all(text, "COLLECTIONS_DIRECTORY", COLLECTIONS_DIRECTORY);
  const auto expected{sourcemeta::core::parse_json(text)};
  EXPECT_EQ(raw_configuration, expected);
}

TEST(Configuration_read, read_invalid_001) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_invalid_001.json"};

  try {
    sourcemeta::one::Configuration::read(configuration_path,
                                         COLLECTIONS_DIRECTORY);
    FAIL();
  } catch (const sourcemeta::one::ConfigurationReadError &error) {
    EXPECT_EQ(error.from(), configuration_path);
    EXPECT_EQ(error.target(),
              std::filesystem::path{STUB_DIRECTORY} / "missing.json");
    EXPECT_EQ(sourcemeta::core::to_string(error.location()),
              "/contents/example/include");
    SUCCEED();
  } catch (...) {
    FAIL();
  }
}

TEST(Configuration_read, read_valid_016_diamond_extends) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_016.json"};
  const auto result{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};
  EXPECT_TRUE(result.is_object());
  EXPECT_TRUE(result.defines("contents"));
  EXPECT_TRUE(result.at("contents").defines("shared"));
  EXPECT_TRUE(result.at("contents").defines("from_b"));
  EXPECT_TRUE(result.at("contents").defines("from_c"));
}

TEST(Configuration_read, read_invalid_004_circular_extends) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_invalid_004.json"};

  try {
    sourcemeta::one::Configuration::read(configuration_path,
                                         COLLECTIONS_DIRECTORY);
    FAIL();
  } catch (const sourcemeta::one::ConfigurationCyclicReferenceError &error) {
    EXPECT_EQ(error.target(), std::filesystem::weakly_canonical(
                                  std::filesystem::path{STUB_DIRECTORY} /
                                  "read_invalid_004.json"));
    EXPECT_EQ(sourcemeta::core::to_string(error.location()),
              "/extends/extends");
  } catch (...) {
    FAIL();
  }
}

TEST(Configuration_read, read_invalid_006_circular_include) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_invalid_006.json"};

  try {
    sourcemeta::one::Configuration::read(configuration_path,
                                         COLLECTIONS_DIRECTORY);
    FAIL();
  } catch (const sourcemeta::one::ConfigurationCyclicReferenceError &error) {
    EXPECT_EQ(error.target(), std::filesystem::weakly_canonical(
                                  std::filesystem::path{STUB_DIRECTORY} /
                                  "read_invalid_006.json"));
  } catch (...) {
    FAIL();
  }
}

TEST(Configuration_read, read_configuration_files_no_extends_no_includes) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_001.json"};
  std::unordered_set<std::string> configuration_files;
  sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY, configuration_files);

  EXPECT_EQ(configuration_files.size(), 3);
  EXPECT_TRUE(configuration_files.contains(
      std::filesystem::weakly_canonical(configuration_path).native()));
  EXPECT_TRUE(configuration_files.contains(
      std::filesystem::weakly_canonical(
          std::filesystem::path{COLLECTIONS_DIRECTORY} / "self" / "v1" /
          "one.json")
          .native()));
  EXPECT_TRUE(configuration_files.contains(
      std::filesystem::weakly_canonical(
          std::filesystem::path{COLLECTIONS_DIRECTORY} / "self" / "v1" /
          "jsonschema.json")
          .native()));
}

TEST(Configuration_read, read_configuration_files_with_extends) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_016.json"};
  std::unordered_set<std::string> configuration_files;
  sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY, configuration_files);

  EXPECT_EQ(configuration_files.size(), 6);
  EXPECT_TRUE(configuration_files.contains(
      std::filesystem::weakly_canonical(configuration_path).native()));
  EXPECT_TRUE(configuration_files.contains(
      std::filesystem::weakly_canonical(
          std::filesystem::path{COLLECTIONS_DIRECTORY} / "self" / "v1" /
          "one.json")
          .native()));
  EXPECT_TRUE(configuration_files.contains(
      std::filesystem::weakly_canonical(
          std::filesystem::path{COLLECTIONS_DIRECTORY} / "self" / "v1" /
          "jsonschema.json")
          .native()));
  EXPECT_TRUE(configuration_files.contains(
      std::filesystem::weakly_canonical(std::filesystem::path{STUB_DIRECTORY} /
                                        "read_valid_016_b.json")
          .native()));
  EXPECT_TRUE(configuration_files.contains(
      std::filesystem::weakly_canonical(std::filesystem::path{STUB_DIRECTORY} /
                                        "read_valid_016_c.json")
          .native()));
  EXPECT_TRUE(configuration_files.contains(
      std::filesystem::weakly_canonical(std::filesystem::path{STUB_DIRECTORY} /
                                        "read_valid_016_d.json")
          .native()));
}

TEST(Configuration_read, read_configuration_files_with_include_chain) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "read_valid_002.json"};
  std::unordered_set<std::string> configuration_files;
  sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY, configuration_files);

  EXPECT_EQ(configuration_files.size(), 5);
  EXPECT_TRUE(configuration_files.contains(
      std::filesystem::weakly_canonical(configuration_path).native()));
  EXPECT_TRUE(configuration_files.contains(
      std::filesystem::weakly_canonical(std::filesystem::path{STUB_DIRECTORY} /
                                        "read_partial_001.json")
          .native()));
  EXPECT_TRUE(configuration_files.contains(
      std::filesystem::weakly_canonical(std::filesystem::path{STUB_DIRECTORY} /
                                        "folder" / "jsonschema.json")
          .native()));
  EXPECT_TRUE(configuration_files.contains(
      std::filesystem::weakly_canonical(
          std::filesystem::path{COLLECTIONS_DIRECTORY} / "self" / "v1" /
          "one.json")
          .native()));
  EXPECT_TRUE(configuration_files.contains(
      std::filesystem::weakly_canonical(
          std::filesystem::path{COLLECTIONS_DIRECTORY} / "self" / "v1" /
          "jsonschema.json")
          .native()));
}
