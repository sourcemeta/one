ARG SOURCEMETA_ONE=one
FROM ${SOURCEMETA_ONE}

COPY public/one.json ./public/one.json
COPY vendor vendor

RUN sourcemeta ./public/one.json --profile
