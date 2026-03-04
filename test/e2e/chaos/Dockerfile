FROM one
COPY one.json .
COPY schemas schemas
COPY corrupt.sh /usr/local/bin/corrupt
RUN sourcemeta one.json --profile
RUN corrupt "$SOURCEMETA_ONE_OUTPUT"
RUN set -e && test -d "$SOURCEMETA_ONE_WORKDIR" && \
  test -z "$(ls -A "$SOURCEMETA_ONE_WORKDIR")"
RUN rm -rf "$SOURCEMETA_ONE_WORKDIR"
