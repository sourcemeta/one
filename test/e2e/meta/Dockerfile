FROM one
RUN apt-get --yes update && apt-get install --yes --no-install-recommends \
  nodejs npm && apt-get clean && rm -rf /var/lib/apt/lists/*
RUN npm install --global @sourcemeta/jsonschema
COPY one.json .
COPY schemas schemas
COPY test test
RUN jsonschema test ./test
RUN sourcemeta one.json --profile
RUN set -e && test -d "$SOURCEMETA_ONE_WORKDIR" && \
  test -z "$(ls -A "$SOURCEMETA_ONE_WORKDIR")"
RUN rm -rf "$SOURCEMETA_ONE_WORKDIR"
