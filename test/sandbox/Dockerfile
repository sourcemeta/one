FROM one
ARG SOURCEMETA_ONE_SANDBOX_CONFIGURATION=html
ARG SOURCEMETA_ONE_SANDBOX_EDITION=community

COPY one-html-community.json .
COPY one-html-enterprise.json .
COPY one-headless-community.json .
COPY one-headless-enterprise.json .
COPY one-empty-community.json .
COPY one-empty-enterprise.json .
COPY one-chaos-community.json .
COPY one-chaos-enterprise.json .

# As the `sourcemeta` command wipes the working directory
COPY postindex.sh /usr/local/bin/sourcemeta-one-postindex

COPY schemas schemas
COPY rules rules

RUN sourcemeta \
  one-${SOURCEMETA_ONE_SANDBOX_CONFIGURATION}-${SOURCEMETA_ONE_SANDBOX_EDITION}.json \
  --profile

RUN sourcemeta-one-postindex \
  "${SOURCEMETA_ONE_SANDBOX_CONFIGURATION}" \
  "${SOURCEMETA_ONE_SANDBOX_EDITION}" \
  "${SOURCEMETA_ONE_OUTPUT}"

# Check that by the end of the indexing, the workdir directory is empty
RUN set -e && test -d "$SOURCEMETA_ONE_WORKDIR" && \
  test -z "$(ls -A "$SOURCEMETA_ONE_WORKDIR")"

RUN rm -rf "$SOURCEMETA_ONE_WORKDIR"
