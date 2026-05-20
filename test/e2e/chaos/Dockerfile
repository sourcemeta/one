FROM one
COPY one.json .
COPY schemas schemas
COPY post-index.sh /usr/local/bin/post-index
RUN sourcemeta one.json --profile
RUN post-index "$SOURCEMETA_ONE_OUTPUT"
RUN set -e && test -d "$SOURCEMETA_ONE_WORKDIR" && \
  test -z "$(ls -A "$SOURCEMETA_ONE_WORKDIR")"
RUN rm -rf "$SOURCEMETA_ONE_WORKDIR"
