#!/usr/bin/env python3

"""Measure how long endpoints take to answer.

A suite declares what it wants measured by importing `run` from here and
naming the requests. Where the suite lives and where the instance is listening
arrive as arguments, so a declaration says what to measure and nothing about
where it is being measured from. Headers a suite cannot write down, such as a
credential it has to go and get, are named as a function to call instead.

The connection is opened once and kept, so what is timed is answering a
request rather than establishing a conversation. Requests are made one after
another, which measures latency rather than throughput: what this answers is
how long one caller waits, not how many callers can be served at once.
"""

import http.client
import json
import sys
import time

WARMUP = 1000
COUNT = 20000


def _percentile(samples, fraction):
    return samples[min(int(len(samples) * fraction), len(samples) - 1)]


def measure(suite, base, name, path, method="GET", headers=None, body=None):
    _, _, rest = base.partition("//")
    authority, _, prefix = rest.partition("/")
    host, _, port = authority.partition(":")
    target = "/" + prefix + path if prefix else path

    # A credential that has to be asked for is asked for here rather than when
    # the run began, as what it grants may not outlast the measurements queued
    # ahead of this one
    headers = headers() if callable(headers) else headers or {}
    payload = body.encode() if body else None
    connection = http.client.HTTPConnection(host, int(port or 80))
    connection.connect()

    def once():
        connection.request(method, target, body=payload, headers=headers)
        response = connection.getresponse()
        response.read()
        return response.status

    # Answering the first time builds what answering afterwards reuses, so
    # what is measured is a warm instance rather than a cold one
    for _ in range(WARMUP):
        status = once()
        if status >= 400:
            sys.exit(f"{suite}: {name}: warm up got HTTP {status}")

    samples = []
    for _ in range(COUNT):
        start = time.perf_counter_ns()
        status = once()
        samples.append((time.perf_counter_ns() - start) / 1000.0)
        # A run that was refused measured something other than what it set out
        # to, so it stops rather than reporting a number nobody can trust
        if status >= 400:
            sys.exit(f"{suite}: {name}: got HTTP {status}")

    connection.close()
    samples.sort()
    return [
        {
            "name": f"{suite}: {name} ({label})",
            "unit": "us",
            "value": round(_percentile(samples, fraction)),
        }
        for label, fraction in (("p50", 0.50), ("p99", 0.99))
    ]


def run(measurements):
    if len(sys.argv) != 3:
        sys.exit(f"Usage: {sys.argv[0]} <suite> <base-url>")

    suite, base = sys.argv[1], sys.argv[2]
    entries = []
    for measurement in measurements:
        entries.extend(measure(suite, base, **measurement))

    json.dump(entries, sys.stdout, indent=2)
    sys.stdout.write("\n")
