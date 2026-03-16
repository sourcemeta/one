window.BENCHMARK_DATA = {
  "lastUpdate": 1773673239716,
  "repoUrl": "https://github.com/sourcemeta/one",
  "entries": {
    "Benchmark Index (enterprise)": [
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
        "date": 1772546364343,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add One Schema",
            "value": 48,
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
        "date": 1772553185372,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 50,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 744,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 7767,
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
        "date": 1772558173472,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 49,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 1109,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 10474,
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
        "date": 1772561102347,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 53,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 1194,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 11230,
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
        "date": 1772562732342,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 52,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 926,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 9904,
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
        "date": 1772566662735,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 50,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 1027,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 10065,
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
          "id": "2cbd5ce49d7e9de3e7385b9080044a37c311bc0d",
          "message": "Upgrade Core and Blaze to the latest versions (#699)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-03T20:05:05-04:00",
          "tree_id": "d766e1a0192960c4d49901ccd08966e3822eaf44",
          "url": "https://github.com/sourcemeta/one/commit/2cbd5ce49d7e9de3e7385b9080044a37c311bc0d"
        },
        "date": 1772583411365,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 53,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 1110,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 10658,
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
          "id": "ef1287c746e7e3701273456be89e09d937073298",
          "message": "Use `--time` when building `public` on CI (#700)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-04T09:05:25-04:00",
          "tree_id": "3984558e11e1a3389f753bfbcfa7b52bbea4da65",
          "url": "https://github.com/sourcemeta/one/commit/ef1287c746e7e3701273456be89e09d937073298"
        },
        "date": 1772630225363,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 51,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 476,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 4757,
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
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "distinct": true,
          "id": "585c7739fd3b5c0229f76ab99a796f2bd0160215",
          "message": "v4.5.0\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-04T09:18:32-04:00",
          "tree_id": "44dc203acdde49e66582f5bb77d053e60496f327",
          "url": "https://github.com/sourcemeta/one/commit/585c7739fd3b5c0229f76ab99a796f2bd0160215"
        },
        "date": 1772631017359,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 45,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 474,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 4685,
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
          "id": "f68e4e26dab5e0ec72674b41cf6f3d474fa05edf",
          "message": "Optimise I/O during indexing (#702)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-04T09:38:59-04:00",
          "tree_id": "26658002ae93d428f9ed84c3dae3436ef4601bb6",
          "url": "https://github.com/sourcemeta/one/commit/f68e4e26dab5e0ec72674b41cf6f3d474fa05edf"
        },
        "date": 1772632217355,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 43,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 422,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 4071,
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
          "id": "289973196d21591b804826a96ad05b3c39af6e7e",
          "message": "Perform less last write time I/O checks during indexing (#703)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-04T12:28:33-04:00",
          "tree_id": "10ab1ef9fc56464e49329660472718e065c9e8e6",
          "url": "https://github.com/sourcemeta/one/commit/289973196d21591b804826a96ad05b3c39af6e7e"
        },
        "date": 1772642461387,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 50,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 415,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 4460,
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
            "name": "Juan Cruz Viotti",
            "username": "jviotti",
            "email": "jv@jviotti.com"
          },
          "id": "8a21e62d919bd462e5501fe3b1f3903823407a86",
          "message": "v4.6.0\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-04T19:16:43Z",
          "url": "https://github.com/sourcemeta/one/commit/8a21e62d919bd462e5501fe3b1f3903823407a86"
        },
        "date": 1772653173826,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 55,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 454,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 4242,
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
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "distinct": true,
          "id": "4482d5261cbeb6253c1bc6e7b2316fe230e48ad7",
          "message": "v4.6.1\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-04T16:46:53-04:00",
          "tree_id": "f244bbe92cdf40ffcd33b4ee2aedc4f0cd819a6d",
          "url": "https://github.com/sourcemeta/one/commit/4482d5261cbeb6253c1bc6e7b2316fe230e48ad7"
        },
        "date": 1772657937355,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 50,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 430,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 4181,
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
            "name": "Juan Cruz Viotti",
            "username": "jviotti",
            "email": "jv@jviotti.com"
          },
          "id": "4482d5261cbeb6253c1bc6e7b2316fe230e48ad7",
          "message": "v4.6.1\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-04T20:46:53Z",
          "url": "https://github.com/sourcemeta/one/commit/4482d5261cbeb6253c1bc6e7b2316fe230e48ad7"
        },
        "date": 1772740637605,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 51,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 434,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 4215,
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
          "id": "fb7046cac1a34ffdbb02c88bee491f968b72818a",
          "message": "Update the benchmark index to add schemas with `$ref` (#709)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-06T10:30:54-04:00",
          "tree_id": "81735421df57d62775308ed23f74abd33ab18dc0",
          "url": "https://github.com/sourcemeta/one/commit/fb7046cac1a34ffdbb02c88bee491f968b72818a"
        },
        "date": 1772808222344,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 42,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 307,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 3251,
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
          "id": "49fbc0dd0843ee0cae9c672b7571e40ab48fd186",
          "message": "Improve cache hits on dependency tree index calculations (#710)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-06T11:58:05-04:00",
          "tree_id": "017387039b4e8a717eb685be8b935225500c86e9",
          "url": "https://github.com/sourcemeta/one/commit/49fbc0dd0843ee0cae9c672b7571e40ab48fd186"
        },
        "date": 1772813402387,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 42,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 275,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 2810,
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
          "id": "6d786dcfe794d923f3f62f581088b558f2182a31",
          "message": "Don't rebuild every index directory listing on schema changes (#711)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-06T12:35:19-04:00",
          "tree_id": "c5029a53c6a8a16ca3ee3c8872c67009fa565d74",
          "url": "https://github.com/sourcemeta/one/commit/6d786dcfe794d923f3f62f581088b558f2182a31"
        },
        "date": 1772815638395,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 48,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 300,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 2827,
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
          "id": "3178dc6b6967d087148d082ca092bfc34124d01a",
          "message": "Consolidate all dependencies into a single indexed file (to reduce I/O) (#704)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-06T15:22:52-04:00",
          "tree_id": "adbcd14376c4b032c595767b51d3932b09b12391",
          "url": "https://github.com/sourcemeta/one/commit/3178dc6b6967d087148d082ca092bfc34124d01a"
        },
        "date": 1772825718365,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 35,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 237,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 2558,
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
          "id": "e54ca13ab94d51558d5f8a03ca5a456c47014330",
          "message": "Greatly simplify the `src/build` module (#712)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-06T17:00:49-04:00",
          "tree_id": "817ecbd1206d528e82b89627339e4f9db18f9458",
          "url": "https://github.com/sourcemeta/one/commit/e54ca13ab94d51558d5f8a03ca5a456c47014330"
        },
        "date": 1772831533381,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 249,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 2500,
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
          "id": "2dceee4e19ae5faf9b49ec1f88222db7fd36a1a5",
          "message": "Merge `Output` into `src/build` (#713)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-09T10:27:46-04:00",
          "tree_id": "124191be86d855005fd91f6b100dff2dc4b3f680",
          "url": "https://github.com/sourcemeta/one/commit/2dceee4e19ae5faf9b49ec1f88222db7fd36a1a5"
        },
        "date": 1773067150351,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 240,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 2452,
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
          "id": "90d01f649086ccbbf8dfe183e129500640c6c0de",
          "message": "Improve directory list generation sorting (#714)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-09T10:48:50-04:00",
          "tree_id": "81b92bb2d119d45a150946100a9a08df0d9e9bdb",
          "url": "https://github.com/sourcemeta/one/commit/90d01f649086ccbbf8dfe183e129500640c6c0de"
        },
        "date": 1773068434342,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 38,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 176,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 1216,
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
          "id": "d633d3ae20f7ced5ce0766de9047817863eea01c",
          "message": "Unify maps in `sourcemeta::one::Build` (#715)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-09T12:37:17-04:00",
          "tree_id": "c83c3645d75523be057b25cb316fa9090e61235e",
          "url": "https://github.com/sourcemeta/one/commit/d633d3ae20f7ced5ce0766de9047817863eea01c"
        },
        "date": 1773074948359,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 158,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 1244,
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
          "id": "c5f48432fdccafb0bc7e45f7ac7eed3569f68456",
          "message": "Speed up tracking of parent directories (#716)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-09T13:24:11-04:00",
          "tree_id": "b21fc277219c43113081f06d4039da76aedc075e",
          "url": "https://github.com/sourcemeta/one/commit/c5f48432fdccafb0bc7e45f7ac7eed3569f68456"
        },
        "date": 1773077737334,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 152,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 1126,
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
          "id": "3f4ce6d919d0b6f8455258b7539677d697f03fc2",
          "message": "Prevent unnecessary path manipulation on index key creation (#718)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-09T13:42:42-04:00",
          "tree_id": "d7a9c9277ea95b17e3dbe9ba4a0f7db34c89e909",
          "url": "https://github.com/sourcemeta/one/commit/3f4ce6d919d0b6f8455258b7539677d697f03fc2"
        },
        "date": 1773078874379,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 140,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 1098,
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
          "id": "7f6182c5b45cd9c9a7a6fc2d02667fe4128f07e0",
          "message": "Benchmark index overhead on fully cached rebuilds (#717)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-09T13:56:11-04:00",
          "tree_id": "b24d2f2e641b5c5cbab09e6c956753438fb7de57",
          "url": "https://github.com/sourcemeta/one/commit/7f6182c5b45cd9c9a7a6fc2d02667fe4128f07e0"
        },
        "date": 1773079667331,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 93,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 750,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 64,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 516,
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
          "id": "d4b424384c8684229e201465cf2b358008f49f5a",
          "message": "Avoid further I/O on the `Reviewing` index phase (#719)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-09T14:47:07-04:00",
          "tree_id": "a266bc510eeed336f084bef07404c663c352be0c",
          "url": "https://github.com/sourcemeta/one/commit/d4b424384c8684229e201465cf2b358008f49f5a"
        },
        "date": 1773082745345,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 85,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 668,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 55,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 424,
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
          "id": "d4b424384c8684229e201465cf2b358008f49f5a",
          "message": "Avoid further I/O on the `Reviewing` index phase (#719)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-09T18:47:07Z",
          "url": "https://github.com/sourcemeta/one/commit/d4b424384c8684229e201465cf2b358008f49f5a"
        },
        "date": 1773084980597,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 87,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 674,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 55,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 421,
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
          "id": "ebc7f669601684551d0784f6effa7f6be1d7cd15",
          "message": "Avoid copying dependencies in the vast majority of dispatches (#720)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-09T16:06:18-04:00",
          "tree_id": "32fcf59dccb1c8065f112d65faa5f846ac9e9d78",
          "url": "https://github.com/sourcemeta/one/commit/ebc7f669601684551d0784f6effa7f6be1d7cd15"
        },
        "date": 1773087444334,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 91,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 738,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 65,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 480,
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
          "id": "4fc6247b0de367cd8e9a4a596e09210b752638e0",
          "message": "Revamp `deps.txt` as an `mmap`ed `state.bin` (#721)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-10T09:40:11-04:00",
          "tree_id": "daf319d0b074e872b70795cbb6acc36d2e05437b",
          "url": "https://github.com/sourcemeta/one/commit/4fc6247b0de367cd8e9a4a596e09210b752638e0"
        },
        "date": 1773150676278,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 45,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 314,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 175,
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
          "id": "8337eca1c1e87e844314b0c269607e708aed989e",
          "message": "Benchmark adding and rebuilding a 10k directory of schemas (#724)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-10T12:20:13-04:00",
          "tree_id": "c626027aadd0d919a469713b9eb8a817b5a200a5",
          "url": "https://github.com/sourcemeta/one/commit/8337eca1c1e87e844314b0c269607e708aed989e"
        },
        "date": 1773161219334,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 72,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 535,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 5341,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 42,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 300,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 2966,
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
          "id": "8337eca1c1e87e844314b0c269607e708aed989e",
          "message": "Benchmark adding and rebuilding a 10k directory of schemas (#724)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-10T16:20:13Z",
          "url": "https://github.com/sourcemeta/one/commit/8337eca1c1e87e844314b0c269607e708aed989e"
        },
        "date": 1773171403744,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 518,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 5230,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 42,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 285,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 2910,
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
          "id": "8337eca1c1e87e844314b0c269607e708aed989e",
          "message": "Benchmark adding and rebuilding a 10k directory of schemas (#724)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-10T16:20:13Z",
          "url": "https://github.com/sourcemeta/one/commit/8337eca1c1e87e844314b0c269607e708aed989e"
        },
        "date": 1773257924684,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 537,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 5352,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 45,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 309,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 2992,
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
          "id": "8337eca1c1e87e844314b0c269607e708aed989e",
          "message": "Benchmark adding and rebuilding a 10k directory of schemas (#724)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-10T16:20:13Z",
          "url": "https://github.com/sourcemeta/one/commit/8337eca1c1e87e844314b0c269607e708aed989e"
        },
        "date": 1773344322679,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 72,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 538,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 5388,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 43,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 302,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 3045,
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
          "id": "d8ef7a5c8a09ae89835106e567c30133a929f341",
          "message": "Revamp the entire index build system (#725)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-13T07:13:56-04:00",
          "tree_id": "a75f5ccd41dc0588e1f890f3d53616d7878c95fc",
          "url": "https://github.com/sourcemeta/one/commit/d8ef7a5c8a09ae89835106e567c30133a929f341"
        },
        "date": 1773401276340,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 48,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 276,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 2639,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 124,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 1227,
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
          "id": "c6b3fa6cc89662f966649d9f3262c3edcb0c4967",
          "message": "Address delta index review comments (#726)\n\nSee: https://github.com/sourcemeta/one/pull/725\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-13T07:58:54-04:00",
          "tree_id": "6ae01463d0a1657b7092bc74cab1c08b86968b0c",
          "url": "https://github.com/sourcemeta/one/commit/c6b3fa6cc89662f966649d9f3262c3edcb0c4967"
        },
        "date": 1773403968333,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 59,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 380,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 4127,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 123,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 1192,
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
          "id": "a6fdafaa11e294b48eab9c15a4f5508c23affc9e",
          "message": "Address index CLI test flakiness (#728)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-13T09:22:36-04:00",
          "tree_id": "7f27d74e72869b51cf3150703f88ecd15ba3513e",
          "url": "https://github.com/sourcemeta/one/commit/a6fdafaa11e294b48eab9c15a4f5508c23affc9e"
        },
        "date": 1773408911266,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 39,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 251,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 2960,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 89,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 931,
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
          "id": "66f92842fb01d5fe304be9c998b4f356c6812541",
          "message": "Refactor delta computation to be DAG based (#727)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-13T13:25:58-04:00",
          "tree_id": "9432d0e406591f22a2e038ba7b8074a366381d49",
          "url": "https://github.com/sourcemeta/one/commit/66f92842fb01d5fe304be9c998b4f356c6812541"
        },
        "date": 1773423596336,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 83,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 621,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 6743,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 133,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 1348,
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
          "id": "22c50cdd40a7b757221d02ec6e89ed6099613d87",
          "message": "Review DAG related tests (#730)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-13T14:15:22-04:00",
          "tree_id": "59bed8be2292ef909dadd8bd17908c7e8f9cd3da",
          "url": "https://github.com/sourcemeta/one/commit/22c50cdd40a7b757221d02ec6e89ed6099613d87"
        },
        "date": 1773426592473,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 86,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 657,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 6845,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 145,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 1517,
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
          "id": "22c50cdd40a7b757221d02ec6e89ed6099613d87",
          "message": "Review DAG related tests (#730)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-13T18:15:22Z",
          "url": "https://github.com/sourcemeta/one/commit/22c50cdd40a7b757221d02ec6e89ed6099613d87"
        },
        "date": 1773430399738,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 84,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 652,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 6972,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 140,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 1411,
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
          "id": "ebaf97eec20e133f0918b4ae9e2a4d348a92b17b",
          "message": "Speed up the delta phase (#731)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-16T10:00:19-04:00",
          "tree_id": "b11520d4503c3a7a5cec6089245e84014998642f",
          "url": "https://github.com/sourcemeta/one/commit/ebaf97eec20e133f0918b4ae9e2a4d348a92b17b"
        },
        "date": 1773670516339,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 568,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 6341,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 673,
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
          "id": "d6292d08c7c30e0e8baf2c9259a70869d6f5f5fa",
          "message": "Reserve memory when parsing `state.bin` (#732)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-16T10:19:48-04:00",
          "tree_id": "cccb3548043e97cdc98fc325670700bed5f7daa4",
          "url": "https://github.com/sourcemeta/one/commit/d6292d08c7c30e0e8baf2c9259a70869d6f5f5fa"
        },
        "date": 1773671716354,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 594,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 6329,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 702,
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
          "id": "a4598b3a0ba028b44ce2a78b2724bb0ff8a73f25",
          "message": "Render schema dependencies/dependents in the explorer with JS (#733)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-16T10:45:56-04:00",
          "tree_id": "cdc1f64a566e72355839141729deefbf000734e4",
          "url": "https://github.com/sourcemeta/one/commit/a4598b3a0ba028b44ce2a78b2724bb0ff8a73f25"
        },
        "date": 1773673238344,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 50,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 318,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3509,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 662,
            "unit": "ms"
          }
        ]
      }
    ]
  }
}