FROM one
ARG SOURCEMETA_ONE_SANDBOX_CONFIGURATION=html
ARG SOURCEMETA_ONE_SANDBOX_EDITION=community

COPY one-html-community.json .
COPY one-html-enterprise.json .
COPY one-headless-community.json .
COPY one-headless-enterprise.json .
COPY one-empty-community.json .
COPY one-empty-enterprise.json .

COPY schemas schemas
COPY rules rules

RUN sourcemeta \
  one-${SOURCEMETA_ONE_SANDBOX_CONFIGURATION}-${SOURCEMETA_ONE_SANDBOX_EDITION}.json \
  --profile

# Check that by the end of the indexing, the workdir directory is empty
RUN test -d "$SOURCEMETA_ONE_WORKDIR" -a \
  "$(find "$SOURCEMETA_ONE_WORKDIR" -mindepth 1 -maxdepth 1 | wc -l)" -eq 0

RUN rm -rf "$SOURCEMETA_ONE_WORKDIR"
