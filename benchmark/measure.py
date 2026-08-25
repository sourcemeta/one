#!/usr/bin/env python3

"""Measure how long one endpoint takes to answer.

The connection is opened once and kept, so what is timed is answering a
request rather than establishing a conversation. Requests are made one after
another, which measures latency rather than throughput: what this answers is
how long one caller waits, not how many callers can be served at once.
"""

import argparse
import http.client
import json
import sys
import time


def percentile(samples, fraction):
    return samples[min(int(len(samples) * fraction), len(samples) - 1)]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--name", required=True)
    parser.add_argument("--url", required=True)
    parser.add_argument("--method", default="GET")
    parser.add_argument("--header", action="append", default=[])
    parser.add_argument("--body", default=None)
    parser.add_argument("--count", type=int, default=20000)
    parser.add_argument("--warmup", type=int, default=1000)
    options = parser.parse_args()

    _, _, rest = options.url.partition("//")
    authority, _, path = rest.partition("/")
    host, _, port = authority.partition(":")
    path = "/" + path

    headers = {}
    for header in options.header:
        key, _, value = header.partition(":")
        headers[key.strip()] = value.strip()

    body = options.body.encode() if options.body else None
    connection = http.client.HTTPConnection(host, int(port or 80))
    connection.connect()

    def once():
        connection.request(options.method, path, body=body, headers=headers)
        response = connection.getresponse()
        response.read()
        return response.status

    # Answering the first time builds what answering afterwards reuses, so
    # what is measured is a warm instance rather than a cold one
    for _ in range(options.warmup):
        status = once()
        if status >= 400:
            sys.exit(f"{options.name}: warm up got HTTP {status}")

    samples = []
    for _ in range(options.count):
        start = time.perf_counter_ns()
        status = once()
        samples.append((time.perf_counter_ns() - start) / 1000.0)
        # A run that was refused measured something other than what it set out
        # to, so it stops rather than reporting a number nobody can trust
        if status >= 400:
            sys.exit(f"{options.name}: got HTTP {status}")

    samples.sort()
    json.dump(
        [
            {
                "name": f"{options.name} (p50)",
                "unit": "us",
                "value": round(percentile(samples, 0.50)),
            },
            {
                "name": f"{options.name} (p99)",
                "unit": "us",
                "value": round(percentile(samples, 0.99)),
            },
        ],
        sys.stdout,
        indent=2,
    )
    sys.stdout.write("\n")


if __name__ == "__main__":
    main()
