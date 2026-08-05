#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/json.h>

namespace sourcemeta::one {

// A provider answering twice about one person is two halves of one account,
// but only one of them is signed. The address pair is carved out of the merge
// because `email_verified` speaks for the address delivered with it, so the
// pair is only ever taken from an answer that carried the address, and an
// assertion left on its own is dropped whichever answer it came from
auto Authentication::combine_claims(const sourcemeta::core::JSON &token,
                                    const sourcemeta::core::JSON &extra)
    -> sourcemeta::core::JSON {
  if (!token.is_object()) {
    return extra.is_object() ? extra : token;
  }

  // Nothing to combine leaves what the token said untouched, since an
  // assertion it carried alone can vouch for no address but its own
  if (!extra.is_object()) {
    return token;
  }

  auto result{token};
  const auto token_has_address{token.defines("email")};
  const auto extra_has_address{extra.defines("email")};
  if (!token_has_address) {
    result.erase("email_verified");
  }

  for (const auto &claim : extra.as_object()) {
    const auto address_pair{claim.first == "email" ||
                            claim.first == "email_verified"};
    // The answer that carried the address carries the assertion about it, so
    // neither half is taken from an answer holding only one of them
    if (address_pair && (token_has_address || !extra_has_address)) {
      continue;
    }

    if (!result.defines(claim.first)) {
      result.assign(claim.first, claim.second);
    }
  }

  return result;
}

} // namespace sourcemeta::one
