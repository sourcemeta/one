ARG SOURCEMETA_ONE=one
FROM ${SOURCEMETA_ONE}

COPY enterprise/e2e/public/one.json ./enterprise/e2e/public/one.json
COPY vendor vendor

RUN sourcemeta ./enterprise/e2e/public/one.json --profile --time
