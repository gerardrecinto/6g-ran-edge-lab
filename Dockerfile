FROM ubuntu:25.10 AS build

RUN apt-get update \
    && apt-get install -y --no-install-recommends build-essential make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .
RUN make

FROM ubuntu:25.10
COPY --from=build /src/build/ranedge-sim /usr/local/bin/ranedge-sim
ENTRYPOINT ["ranedge-sim"]
CMD ["--ticks", "20", "--json"]
