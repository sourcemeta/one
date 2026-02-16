file(SHA256 "${INPUT}" CHECKSUM)
file(WRITE "${OUTPUT}"
  "#ifndef ${DEFINE}_H_\n"
  "#define ${DEFINE}_H_\n"
  "#include <string_view>\n"
  "constexpr std::string_view ${DEFINE}{\"${CHECKSUM}\"};\n"
  "#endif\n")
