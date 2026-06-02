FROM one
RUN apt-get --yes update && apt-get install --yes --no-install-recommends jq \
  && apt-get clean && rm -rf /var/lib/apt/lists/*
COPY benchmark/index.sh /benchmark/index.sh
COPY benchmark/index-add-update-rebuild.sh /benchmark/index-add-update-rebuild.sh
COPY benchmark/index-n.sh /benchmark/index-n.sh
COPY benchmark/index-custom-meta-schema.sh /benchmark/index-custom-meta-schema.sh
RUN /benchmark/index.sh /usr/bin/sourcemeta-one-index > /benchmark.json
ENTRYPOINT [ "cat", "/benchmark.json" ]
