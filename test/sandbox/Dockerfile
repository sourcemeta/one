FROM one
ARG SOURCEMETA_ONE_SANDBOX_CONFIGURATION=html

COPY one-html.json .
COPY one-headless.json .
COPY one-empty.json .

COPY schemas schemas

RUN sourcemeta one-${SOURCEMETA_ONE_SANDBOX_CONFIGURATION}.json --profile

# Check that by the end of the indexing, the workdir directory is empty
RUN test -d "$SOURCEMETA_ONE_WORKDIR" -a \
  "$(find "$SOURCEMETA_ONE_WORKDIR" -mindepth 1 -maxdepth 1 | wc -l)" -eq 0

# For basic testing purposes
COPY manifest-html.txt .
COPY manifest-headless.txt .
COPY manifest-empty.txt .

COPY manifest-check.sh .
RUN ./manifest-check.sh "$SOURCEMETA_ONE_OUTPUT" \
  ./manifest-${SOURCEMETA_ONE_SANDBOX_CONFIGURATION}.txt
RUN rm -rf "$SOURCEMETA_ONE_WORKDIR"
