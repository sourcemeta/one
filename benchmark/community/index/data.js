window.BENCHMARK_DATA = {
  "lastUpdate": 1772566549471,
  "repoUrl": "https://github.com/sourcemeta/one",
  "entries": {
    "Benchmark Index (community)": [
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "distinct": true,
          "id": "302db472f9e387ed57f82955b58c2c2a8de238a6",
          "message": "Run CI pipeline on `main` too (for benchmarks)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-03T09:48:13-04:00",
          "tree_id": "f855fc24841c07dca7d99ed0980bcd7daeedd997",
          "url": "https://github.com/sourcemeta/one/commit/302db472f9e387ed57f82955b58c2c2a8de238a6"
        },
        "date": 1772546339359,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add One Schema",
            "value": 49,
            "unit": "ms"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "c6157d325dc3225163af08edd90e0fb061587df0",
          "message": "Increase indexing benchmarks based on registry size (#693)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-03T11:41:40-04:00",
          "tree_id": "d4d86233da495ad98987fb117b75e6491a4c0152",
          "url": "https://github.com/sourcemeta/one/commit/c6157d325dc3225163af08edd90e0fb061587df0"
        },
        "date": 1772553182382,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 47,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 759,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 7629,
            "unit": "ms"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "af2e09ce4464907eceaac343efde4e4f8edf376f",
          "message": "Reuse a single staging directory to speed up I/O operations in the indexer (#694)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-03T13:05:14-04:00",
          "tree_id": "2131463d1f994b9002d88b9f73c9715ea6de9edb",
          "url": "https://github.com/sourcemeta/one/commit/af2e09ce4464907eceaac343efde4e4f8edf376f"
        },
        "date": 1772558174353,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 44,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 986,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 9724,
            "unit": "ms"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "e77e189cafec6f5ec2d15e59a74126e95494adde",
          "message": "Add higher level profiling to the indexing phases with `--time` (#695)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-03T13:52:44-04:00",
          "tree_id": "197ba439252103131231a31885092480d4749c17",
          "url": "https://github.com/sourcemeta/one/commit/e77e189cafec6f5ec2d15e59a74126e95494adde"
        },
        "date": 1772561023412,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 50,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 1134,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 10685,
            "unit": "ms"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "e24c5353cccf00350602ef1bb9a3df335e71280a",
          "message": "Speed up `resolver::add` during the detection phase (#696)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-03T14:19:34-04:00",
          "tree_id": "230e5706bdc13d5f9f57cc65b1ce9ec4fa17990d",
          "url": "https://github.com/sourcemeta/one/commit/e24c5353cccf00350602ef1bb9a3df335e71280a"
        },
        "date": 1772562634340,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 43,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 973,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 9459,
            "unit": "ms"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "name": "Juan Cruz Viotti",
            "username": "jviotti",
            "email": "jv@jviotti.com"
          },
          "committer": {
            "name": "GitHub",
            "username": "web-flow",
            "email": "noreply@github.com"
          },
          "id": "e24c5353cccf00350602ef1bb9a3df335e71280a",
          "message": "Speed up `resolver::add` during the detection phase (#696)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-03T18:19:34Z",
          "url": "https://github.com/sourcemeta/one/commit/e24c5353cccf00350602ef1bb9a3df335e71280a"
        },
        "date": 1772566548664,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 47,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 1328,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 12770,
            "unit": "ms"
          }
        ]
      }
    ]
  }
}