#ifndef SOURCEMETA_ONE_HTML_ESCAPE_H_
#define SOURCEMETA_ONE_HTML_ESCAPE_H_

#include <string> // std::string

namespace sourcemeta::one::html {

// HTML character escaping implementation per HTML Living Standard
// See: https://html.spec.whatwg.org/multipage/parsing.html#escapingString
auto escape(std::string &text) -> void;

} // namespace sourcemeta::one::html

#endif
