FROM one
COPY one.json .
COPY schemas schemas
COPY rules rules
RUN sourcemeta one.json --profile
RUN set -e && test -d "$SOURCEMETA_ONE_WORKDIR" && \
  test -z "$(ls -A "$SOURCEMETA_ONE_WORKDIR")"
RUN rm -rf "$SOURCEMETA_ONE_WORKDIR"
