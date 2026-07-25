FROM one
COPY tls/ca.crt /usr/local/share/ca-certificates/auth-sso-authority.crt
RUN cat /usr/local/share/ca-certificates/auth-sso-authority.crt \
  >> /etc/ssl/certs/ca-certificates.crt
COPY one.json .
COPY schemas schemas
RUN sourcemeta one.json --profile
RUN set -e && test -d "$SOURCEMETA_ONE_WORKDIR" && \
  test -z "$(ls -A "$SOURCEMETA_ONE_WORKDIR")"
RUN rm -rf "$SOURCEMETA_ONE_WORKDIR"
