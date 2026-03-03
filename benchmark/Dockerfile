FROM one
COPY benchmark/index.sh /benchmark/index.sh
RUN /benchmark/index.sh /usr/bin/sourcemeta-one-index > /benchmark.json
ENTRYPOINT [ "cat", "/benchmark.json" ]
