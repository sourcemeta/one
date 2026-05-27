#!/usr/bin/env python3

import re
import sys

LINE = re.compile(r"^([0-9A-Fa-f]+)(?:\.\.([0-9A-Fa-f]+))?\s*;\s*(\S+)")
MISSING_PREFIX = re.compile(r"^#\s*@missing:\s*")

TOTAL_CODEPOINTS = 0x110000
PAGE_SHIFT = 10
PAGE_SIZE = 1 << PAGE_SHIFT
NUM_PAGES = TOTAL_CODEPOINTS // PAGE_SIZE

# Per-property canonical order. Position in this list defines the integer
# value of the matching C++ enum entry. PropertyValueAliases.txt supplies
# the short/long alias mappings at codegen time, so we only need to
# declare one form per value here.

JOINING_TYPE_ORDER = ["U", "T", "L", "R", "D", "C"]

BIDI_CLASS_ORDER = [
    "L", "R", "AL", "EN", "ES", "ET", "AN", "CS", "NSM", "BN",
    "B", "S", "WS", "ON", "LRE", "LRO", "RLE", "RLO", "PDF",
    "LRI", "RLI", "FSI", "PDI",
]

UNICODE_SCRIPT_ORDER = [
    "Adlam", "Ahom", "Anatolian_Hieroglyphs", "Arabic", "Armenian",
    "Avestan", "Balinese", "Bamum", "Bassa_Vah", "Batak", "Bengali",
    "Beria_Erfe", "Bhaiksuki", "Bopomofo", "Brahmi", "Braille",
    "Buginese", "Buhid", "Canadian_Aboriginal", "Carian",
    "Caucasian_Albanian", "Chakma", "Cham", "Cherokee", "Chorasmian",
    "Common", "Coptic", "Cuneiform", "Cypriot", "Cypro_Minoan",
    "Cyrillic", "Deseret", "Devanagari", "Dives_Akuru", "Dogra",
    "Duployan", "Egyptian_Hieroglyphs", "Elbasan", "Elymaic",
    "Ethiopic", "Garay", "Georgian", "Glagolitic", "Gothic", "Grantha",
    "Greek", "Gujarati", "Gunjala_Gondi", "Gurmukhi", "Gurung_Khema",
    "Han", "Hangul", "Hanifi_Rohingya", "Hanunoo", "Hatran", "Hebrew",
    "Hiragana", "Imperial_Aramaic", "Inherited", "Inscriptional_Pahlavi",
    "Inscriptional_Parthian", "Javanese", "Kaithi", "Kannada", "Katakana",
    "Kawi", "Kayah_Li", "Kharoshthi", "Khitan_Small_Script", "Khmer",
    "Khojki", "Khudawadi", "Kirat_Rai", "Lao", "Latin", "Lepcha", "Limbu",
    "Linear_A", "Linear_B", "Lisu", "Lycian", "Lydian", "Mahajani",
    "Makasar", "Malayalam", "Mandaic", "Manichaean", "Marchen",
    "Masaram_Gondi", "Medefaidrin", "Meetei_Mayek", "Mende_Kikakui",
    "Meroitic_Cursive", "Meroitic_Hieroglyphs", "Miao", "Modi",
    "Mongolian", "Mro", "Multani", "Myanmar", "Nabataean", "Nag_Mundari",
    "Nandinagari", "New_Tai_Lue", "Newa", "Nko", "Nushu",
    "Nyiakeng_Puachue_Hmong", "Ogham", "Ol_Chiki", "Ol_Onal",
    "Old_Hungarian", "Old_Italic", "Old_North_Arabian", "Old_Permic",
    "Old_Persian", "Old_Sogdian", "Old_South_Arabian", "Old_Turkic",
    "Old_Uyghur", "Oriya", "Osage", "Osmanya", "Pahawh_Hmong",
    "Palmyrene", "Pau_Cin_Hau", "Phags_Pa", "Phoenician",
    "Psalter_Pahlavi", "Rejang", "Runic", "Samaritan", "Saurashtra",
    "Sharada", "Shavian", "Siddham", "Sidetic", "SignWriting", "Sinhala",
    "Sogdian", "Sora_Sompeng", "Soyombo", "Sundanese", "Sunuwar",
    "Syloti_Nagri", "Syriac", "Tagalog", "Tagbanwa", "Tai_Le", "Tai_Tham",
    "Tai_Viet", "Tai_Yo", "Takri", "Tamil", "Tangsa", "Tangut", "Telugu",
    "Thaana", "Thai", "Tibetan", "Tifinagh", "Tirhuta", "Todhri",
    "Tolong_Siki", "Toto", "Tulu_Tigalari", "Ugaritic", "Unknown", "Vai",
    "Vithkuqi", "Wancho", "Warang_Citi", "Yezidi", "Yi", "Zanabazar_Square",
    "Katakana_Or_Hiragana",
]


def parse_alias_lines(aliases_path, property_short):
    rows = []
    with open(aliases_path) as source:
        for line in source:
            stripped = line.split("#", 1)[0].strip()
            if not stripped:
                continue
            parts = [part.strip() for part in stripped.split(";")]
            if parts[0] == property_short:
                rows.append([field for field in parts[1:] if field])
    return rows


def build_combining_mark_value_map(aliases_path):
    """Build {form: int} from PropertyValueAliases.txt mapping each
    General_Category alias to 1 if it is a combining mark (Mn, Mc, Me,
    or the supergroup M / Mark / Combining_Mark) and to 0 otherwise."""
    combining = {"M", "Mn", "Mc", "Me"}
    result = {}
    for row in parse_alias_lines(aliases_path, "gc"):
        value = 1 if any(field in combining for field in row) else 0
        for field in row:
            result[field] = value
    return result


def build_value_map(aliases_path, property_short, canonical_order=None):
    """Build {form: int} for a property. With canonical_order, each row's
    integer is its canonical's position in that list; without, the row's
    first field is read as the integer directly (used for ccc)."""
    canonical_to_int = (
        {name: index for index, name in enumerate(canonical_order)}
        if canonical_order is not None
        else None
    )
    result = {}
    unmatched = []
    for row in parse_alias_lines(aliases_path, property_short):
        if canonical_to_int is None:
            value = int(row[0])
        else:
            value = next(
                (canonical_to_int[field] for field in row if field in canonical_to_int),
                None,
            )
            if value is None:
                unmatched.append(row)
                continue
        for field in row:
            result[field] = value
    if unmatched:
        raise ValueError(
            f"{aliases_path}: property {property_short!r} has values not "
            f"declared in canonical order: {unmatched}"
        )
    return result


def parse_file(path, value_map):
    """Read a UCD file and return a list of (first, last, value) entries
    with @missing defaults first and data ranges second, so callers can
    apply them in order regardless of where @missing appears in the file."""
    missing = []
    data = []
    with open(path) as source:
        for line_number, line in enumerate(source, start=1):
            stripped = line.strip()
            if not stripped:
                continue
            target = data
            if stripped.startswith("#"):
                prefix = MISSING_PREFIX.match(stripped)
                if not prefix:
                    continue
                stripped = stripped[prefix.end():]
                target = missing
            match = LINE.match(stripped)
            if not match:
                raise ValueError(
                    f"{path}:{line_number}: unparseable line: {stripped!r}"
                )
            first = int(match.group(1), 16)
            last = int(match.group(2), 16) if match.group(2) else first
            raw_value = match.group(3)
            try:
                value = value_map[raw_value]
            except KeyError as error:
                raise ValueError(
                    f"{path}:{line_number}: invalid value {raw_value!r}: {error}"
                ) from error
            target.append((first, last, value))
    return missing + data


def build_pages(entries):
    values = [0] * TOTAL_CODEPOINTS
    for first, last, value in entries:
        values[first : last + 1] = [value] * (last - first + 1)
    page_to_id = {}
    unique_pages = []
    stage1 = []
    for page_index in range(NUM_PAGES):
        start = page_index * PAGE_SIZE
        page = tuple(values[start : start + PAGE_SIZE])
        if page not in page_to_id:
            page_to_id[page] = len(unique_pages)
            unique_pages.append(page)
        stage1.append(page_to_id[page])
    return stage1, unique_pages


def emit_row(output, items):
    for offset in range(0, len(items), 16):
        chunk = items[offset : offset + 16]
        output.write("    " + ", ".join(str(value) for value in chunk) + ",\n")


def emit_property(output, prefix, stage1, unique_pages):
    output.write(
        f"constexpr std::uint16_t {prefix}_STAGE1[{len(stage1)}] = {{\n"
    )
    emit_row(output, stage1)
    output.write("};\n\n")
    stage2_size = len(unique_pages) * PAGE_SIZE
    output.write(
        f"constexpr std::uint8_t {prefix}_STAGE2[{stage2_size}] = {{\n"
    )
    for page in unique_pages:
        emit_row(output, list(page))
    output.write("};\n\n")


def main():
    if len(sys.argv) != 8:
        print(
            f"Usage: {sys.argv[0]} "
            "<output.h> "
            "<PropertyValueAliases.txt> "
            "<DerivedCombiningClass.txt> "
            "<DerivedJoiningType.txt> "
            "<DerivedBidiClass.txt> "
            "<Scripts.txt> "
            "<DerivedGeneralCategory.txt>",
            file=sys.stderr,
        )
        sys.exit(1)

    output_path = sys.argv[1]
    aliases_path = sys.argv[2]

    properties = [
        ("COMBINING_CLASS", sys.argv[3],
         build_value_map(aliases_path, "ccc")),
        ("JOINING_TYPE", sys.argv[4],
         build_value_map(aliases_path, "jt", JOINING_TYPE_ORDER)),
        ("BIDI_CLASS", sys.argv[5],
         build_value_map(aliases_path, "bc", BIDI_CLASS_ORDER)),
        ("UNICODE_SCRIPT", sys.argv[6],
         build_value_map(aliases_path, "sc", UNICODE_SCRIPT_ORDER)),
        ("IS_COMBINING_MARK", sys.argv[7],
         build_combining_mark_value_map(aliases_path)),
    ]

    with open(output_path, "w") as output:
        output.write("#include <cstdint>\n\n")
        output.write("namespace {\n\n")
        for prefix, input_path, value_map in properties:
            stage1, pages = build_pages(parse_file(input_path, value_map))
            emit_property(output, prefix, stage1, pages)
        output.write("} // namespace\n")


if __name__ == "__main__":
    main()
