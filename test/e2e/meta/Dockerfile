FROM one AS tests
WORKDIR /test
COPY schemas schemas
COPY test test
RUN jsonschema test ./test

FROM one
COPY one.json .
# Copying from the previous stage guarantees the schema tests run
# as part of this build
COPY --from=tests /test/schemas schemas
RUN sourcemeta one.json --profile
RUN set -e && test -d "$SOURCEMETA_ONE_WORKDIR" && \
  test -z "$(ls -A "$SOURCEMETA_ONE_WORKDIR")"
RUN rm -rf "$SOURCEMETA_ONE_WORKDIR"
