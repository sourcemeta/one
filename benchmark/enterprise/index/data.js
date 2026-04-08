window.BENCHMARK_DATA = {
  "lastUpdate": 1775675542006,
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
          "id": "63eae35d437e010b38d5632fa5a46b5916c2985a",
          "message": "Manage `state.bin` purely using `mmap` (#735)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-16T11:52:47-04:00",
          "tree_id": "4fd823fcbabbd32103258d9c4d055e6d24e316db",
          "url": "https://github.com/sourcemeta/one/commit/63eae35d437e010b38d5632fa5a46b5916c2985a"
        },
        "date": 1773677239327,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 55,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 326,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3797,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 50,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 391,
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
          "id": "f8f041e3409c99a4eb522caaf0bdaabf0c1fc094",
          "message": "Increase robustness of `state.bin` in-memory manipulations (#737)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-16T12:14:20-04:00",
          "tree_id": "f7c6986e5e3a6f3fe061b4e91c4cc8e89fc866d0",
          "url": "https://github.com/sourcemeta/one/commit/f8f041e3409c99a4eb522caaf0bdaabf0c1fc094"
        },
        "date": 1773678569325,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 51,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 327,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3566,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 54,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 440,
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
          "id": "3bc1e5925940a476b759278bb4b482a8c979a61b",
          "message": "Revise TODO comments on `index.cc` (#739)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-16T14:58:31-04:00",
          "tree_id": "bdc4b1fcd1172fc4dc821e5df426013278c49280",
          "url": "https://github.com/sourcemeta/one/commit/3bc1e5925940a476b759278bb4b482a8c979a61b"
        },
        "date": 1773688423344,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 50,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 334,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3416,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 218,
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
          "id": "9a4b0e6d0db5d6bc711dbe40f11c1e8bc162d9ef",
          "message": "Simplify delta calculation interface (#740)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-16T15:13:38-04:00",
          "tree_id": "6e047c00545c35dfaf39829dc6bf180affa32c94",
          "url": "https://github.com/sourcemeta/one/commit/9a4b0e6d0db5d6bc711dbe40f11c1e8bc162d9ef"
        },
        "date": 1773689303327,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 48,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 323,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3488,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 210,
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
          "id": "9a4b0e6d0db5d6bc711dbe40f11c1e8bc162d9ef",
          "message": "Simplify delta calculation interface (#740)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-16T19:13:38Z",
          "url": "https://github.com/sourcemeta/one/commit/9a4b0e6d0db5d6bc711dbe40f11c1e8bc162d9ef"
        },
        "date": 1773690371812,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 48,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 302,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3220,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 211,
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
          "id": "6ff69542fe2208ac629d26746f69ce05d07d5dee",
          "message": "Extend index handlers with state and skipping ability (#741)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-16T15:48:47-04:00",
          "tree_id": "faa4dd4493f820f2b25019ee429e2b019573538d",
          "url": "https://github.com/sourcemeta/one/commit/6ff69542fe2208ac629d26746f69ce05d07d5dee"
        },
        "date": 1773691408341,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 50,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 327,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3594,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 212,
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
          "id": "b64474819b9e61f74e54829d11812e4611daf59c",
          "message": "Build dependents directly out of the build state (#742)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-17T09:52:46-04:00",
          "tree_id": "33cc99d1e6d9c1178f6082341a59c06fd9ac009f",
          "url": "https://github.com/sourcemeta/one/commit/b64474819b9e61f74e54829d11812e4611daf59c"
        },
        "date": 1773756499356,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 43,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 285,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 4400,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 215,
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
          "id": "b64474819b9e61f74e54829d11812e4611daf59c",
          "message": "Build dependents directly out of the build state (#742)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-17T13:52:46Z",
          "url": "https://github.com/sourcemeta/one/commit/b64474819b9e61f74e54829d11812e4611daf59c"
        },
        "date": 1773776771652,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 45,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 290,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3657,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 218,
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
          "id": "d0b7eb8b98cedcfabdf6462445f7f693256ce80e",
          "message": "Set the cache path when populating the resolver with cache hits (#746)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-17T17:13:28-04:00",
          "tree_id": "c9da661b0bf72f52ac210292cc8f8bf50328865f",
          "url": "https://github.com/sourcemeta/one/commit/d0b7eb8b98cedcfabdf6462445f7f693256ce80e"
        },
        "date": 1773784617369,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 47,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 323,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3986,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 47,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 285,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 3082,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 256,
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
          "id": "63171c95ea5e9fd466012bfb88ef80ef8234c182",
          "message": "Two phase deltas during indexing (#747)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-18T09:49:34-04:00",
          "tree_id": "35194f4b0b8bc7fdc2622c6d33328911168c1825",
          "url": "https://github.com/sourcemeta/one/commit/63171c95ea5e9fd466012bfb88ef80ef8234c182"
        },
        "date": 1773842635284,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 136,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1406,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 157,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1760,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 162,
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
          "id": "943da3c35972bd5f0d94b4cd9b5c8ee4be73baaf",
          "message": "Fix potential inconsistencies when removing schemas with references (#748)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-18T10:38:13-04:00",
          "tree_id": "22666c64710917b0fc6bed8c13d73fba0ddcc477",
          "url": "https://github.com/sourcemeta/one/commit/943da3c35972bd5f0d94b4cd9b5c8ee4be73baaf"
        },
        "date": 1773845604353,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 38,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 182,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1905,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 38,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 196,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 2149,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 228,
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
          "id": "af096f091f8a2dd2b8ca75f676b72039f338f20b",
          "message": "Improve index benchmark details (#749)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-18T13:21:04-04:00",
          "tree_id": "4dac22788ca1801c016c4f5b1546777529bfa275",
          "url": "https://github.com/sourcemeta/one/commit/af096f091f8a2dd2b8ca75f676b72039f338f20b"
        },
        "date": 1773855359355,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 39,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 191,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1922,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 39,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 206,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 2143,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 235,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 152,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1724,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 55140,
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
          "id": "9a6fa19931cf712aed8903a4f787c4d0f10a3316",
          "message": "Improve from-scratch indexing performance (#750)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-18T14:36:54-04:00",
          "tree_id": "f56ead69c63116de1198b34526f401f75bc3a63f",
          "url": "https://github.com/sourcemeta/one/commit/9a6fa19931cf712aed8903a4f787c4d0f10a3316"
        },
        "date": 1773859901345,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 39,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 198,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 2095,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 41,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 207,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 2017,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 239,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 152,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1378,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 16273,
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
          "id": "9a6fa19931cf712aed8903a4f787c4d0f10a3316",
          "message": "Improve from-scratch indexing performance (#750)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-18T18:36:54Z",
          "url": "https://github.com/sourcemeta/one/commit/9a6fa19931cf712aed8903a4f787c4d0f10a3316"
        },
        "date": 1773863057621,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 40,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 201,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 2022,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 42,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 198,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 2055,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 245,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 156,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1419,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 16567,
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
          "id": "d19337db02b664657754a69f27536da8c10d50c6",
          "message": "Migrate `.metapack` headers into a binary format (#751)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-18T16:03:49-04:00",
          "tree_id": "de12bed5201eb2875cc8edee0391c8d5476cc0e3",
          "url": "https://github.com/sourcemeta/one/commit/d19337db02b664657754a69f27536da8c10d50c6"
        },
        "date": 1773865038283,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 128,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1376,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 124,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1358,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 165,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 106,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 947,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12229,
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
          "id": "6d683a9d806a908fb2ac79b3cd5857385a5a4f9f",
          "message": "Improve safety guards on metapack header parsing (#752)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-18T17:01:10-04:00",
          "tree_id": "3e9e00c0d61cf31f3bcd9cbe5e01764f5ef251c4",
          "url": "https://github.com/sourcemeta/one/commit/6d683a9d806a908fb2ac79b3cd5857385a5a4f9f"
        },
        "date": 1773868577341,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 36,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 162,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1661,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 35,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 160,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1610,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 237,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 146,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1175,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 15663,
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
          "id": "486f3d545615882407288a9455931dcdfd574755",
          "message": "Compute search index out of the directory metadata (#753)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-19T09:22:42-04:00",
          "tree_id": "356841d8a7f4d31f6c13a8c3911a8e513e7671bb",
          "url": "https://github.com/sourcemeta/one/commit/486f3d545615882407288a9455931dcdfd574755"
        },
        "date": 1773927424315,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 133,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1430,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 129,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1395,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 171,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 105,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 983,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12696,
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
          "id": "2d928686d5e240495774957cd9ad72a53a6a8417",
          "message": "Speed up directory indexing and revise directory ordering (#754)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-19T12:05:00-04:00",
          "tree_id": "fd4d95b9babdb5210132cc246c17bb44a138e1d3",
          "url": "https://github.com/sourcemeta/one/commit/2d928686d5e240495774957cd9ad72a53a6a8417"
        },
        "date": 1773937167335,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 155,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1546,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 149,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1512,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 228,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 137,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1225,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 15845,
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
          "id": "1b564cd854af07defb5171fd7960fb73d026cd29",
          "message": "v5.0.0\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-19T14:12:25-04:00",
          "tree_id": "52b5e949fc0408ca81899e9a7e755144423c9d4b",
          "url": "https://github.com/sourcemeta/one/commit/1b564cd854af07defb5171fd7960fb73d026cd29"
        },
        "date": 1773945258349,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 36,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 174,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1764,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 36,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 165,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1709,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 240,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 138,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1185,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 15430,
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
          "id": "1b564cd854af07defb5171fd7960fb73d026cd29",
          "message": "v5.0.0\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-19T18:12:25Z",
          "url": "https://github.com/sourcemeta/one/commit/1b564cd854af07defb5171fd7960fb73d026cd29"
        },
        "date": 1773949312606,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 156,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1676,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 150,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1811,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 230,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 136,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1138,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14732,
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
          "id": "209c2fab4bf6c0d705ef50de30bf09ba875aaf82",
          "message": "v5.0.1\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-19T16:39:39-04:00",
          "tree_id": "5fb524c39267c1ad22cadb169dc23494a23d2f45",
          "url": "https://github.com/sourcemeta/one/commit/209c2fab4bf6c0d705ef50de30bf09ba875aaf82"
        },
        "date": 1773953645301,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 85,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 731,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 84,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 690,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 134,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 95,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 864,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12437,
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
          "id": "a4efbe07de102ead5da846aea8ce6852341769b7",
          "message": "Add a documentation warning about directories with >10k entries (#766)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-20T13:07:16-04:00",
          "tree_id": "414b7694f00f35e0a6df4b65739a3d822fb826f3",
          "url": "https://github.com/sourcemeta/one/commit/a4efbe07de102ead5da846aea8ce6852341769b7"
        },
        "date": 1774027546330,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 109,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 872,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 103,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 825,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 189,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 126,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1942,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 24372,
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
          "id": "50e3005126f012c29bd4081d1179d50586b468dd",
          "message": "Replace ZLIB with `libdeflate` on `src/gzip` (#765)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-20T13:28:13-04:00",
          "tree_id": "d8c005dd28b8ba57974fdcec7ad0b6ed8425a58b",
          "url": "https://github.com/sourcemeta/one/commit/50e3005126f012c29bd4081d1179d50586b468dd"
        },
        "date": 1774028573337,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 102,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 922,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 101,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 837,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 189,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 121,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1073,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14167,
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
          "id": "3fb41b64fc287265aaf8b91711c0b7abf05981c0",
          "message": "Attempt to patch the directory list when possible (#763)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-20T13:44:47-04:00",
          "tree_id": "a94976bc043107199bb941df4e15306f71286ac6",
          "url": "https://github.com/sourcemeta/one/commit/3fb41b64fc287265aaf8b91711c0b7abf05981c0"
        },
        "date": 1774029786457,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 82,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 658,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 82,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 626,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 187,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 140,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1031,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13515,
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
          "id": "3fb41b64fc287265aaf8b91711c0b7abf05981c0",
          "message": "Attempt to patch the directory list when possible (#763)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-20T17:44:47Z",
          "url": "https://github.com/sourcemeta/one/commit/3fb41b64fc287265aaf8b91711c0b7abf05981c0"
        },
        "date": 1774036060589,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 84,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 700,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 83,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 620,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 193,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 127,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1069,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13933,
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
          "id": "780a4bc8be4ee5775109066bc750b320d50a563b",
          "message": "Speed up delta calculation on schema insert/update (#767)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-20T15:50:48-04:00",
          "tree_id": "5bed94200eb938e7c46f17ce087f1e1d07d6b5c6",
          "url": "https://github.com/sourcemeta/one/commit/780a4bc8be4ee5775109066bc750b320d50a563b"
        },
        "date": 1774037138343,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 677,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 629,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 213,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 148,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1045,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14152,
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
          "id": "470fdc43c2396fe06d2f64c69a0c3dd45acf6253",
          "message": "Remove unused handler Bypassing mechanisms (#770)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-20T16:44:40-04:00",
          "tree_id": "531469987e6e4354c4cd6d05ab3c02afe95ad7e4",
          "url": "https://github.com/sourcemeta/one/commit/470fdc43c2396fe06d2f64c69a0c3dd45acf6253"
        },
        "date": 1774040389336,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 599,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 81,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 612,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 202,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 126,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1007,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13601,
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
          "id": "57b51fd5cdb4a637d1eaaf61500082c35d339cb8",
          "message": "Implement `--maximum-direct-directory-entries` with a sane default (1000) (#768)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-20T17:23:17-04:00",
          "tree_id": "3038eae06625cfa5e190c9d0ceda6bb755028472",
          "url": "https://github.com/sourcemeta/one/commit/57b51fd5cdb4a637d1eaaf61500082c35d339cb8"
        },
        "date": 1774042664324,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 618,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 602,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 197,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 128,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1103,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13992,
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
          "id": "0a8a48cd21215325b61079301dcfbffebb634e7e",
          "message": "v5.0.2\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-21T10:57:53-04:00",
          "tree_id": "76274b7770bc2b08daefac7ad5f9fcd57ed5767b",
          "url": "https://github.com/sourcemeta/one/commit/0a8a48cd21215325b61079301dcfbffebb634e7e"
        },
        "date": 1774105907281,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 57,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 462,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 61,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 472,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 131,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 86,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 798,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 11305,
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
          "id": "3d8fcfa6bcf5485d72aee882661e6e82fef5e1e4",
          "message": "Upgrade uWebSockets to v20.76.0 (#771)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-23T09:39:07-04:00",
          "tree_id": "08508e299daf49e7142068230158f5672488809f",
          "url": "https://github.com/sourcemeta/one/commit/3d8fcfa6bcf5485d72aee882661e6e82fef5e1e4"
        },
        "date": 1774274405351,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 651,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 612,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 199,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 128,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1125,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14736,
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
          "id": "3ee82fc56f7149e7bafcf2f1d62d1da9693e5054",
          "message": "Expose the recursive count of schemas in the directory API (#772)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-23T10:43:54-04:00",
          "tree_id": "5df04f301eb456ceb99f35071d8b39132a79611e",
          "url": "https://github.com/sourcemeta/one/commit/3ee82fc56f7149e7bafcf2f1d62d1da9693e5054"
        },
        "date": 1774277887318,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 604,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 596,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 195,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 118,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1091,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14748,
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
          "id": "7a1e3b91576d18bb4107679d176655fa61de1a06",
          "message": "v5.1.0\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-23T11:38:43-04:00",
          "tree_id": "366e59ef8d16a15c329ed3543aeefef4276d91b6",
          "url": "https://github.com/sourcemeta/one/commit/7a1e3b91576d18bb4107679d176655fa61de1a06"
        },
        "date": 1774281215333,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 613,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 81,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 638,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 203,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 149,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1087,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14562,
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
          "id": "7a1e3b91576d18bb4107679d176655fa61de1a06",
          "message": "v5.1.0\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-23T15:38:43Z",
          "url": "https://github.com/sourcemeta/one/commit/7a1e3b91576d18bb4107679d176655fa61de1a06"
        },
        "date": 1774294849533,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 792,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 603,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 200,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 122,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1132,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14050,
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
          "id": "7a1e3b91576d18bb4107679d176655fa61de1a06",
          "message": "v5.1.0\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-23T15:38:43Z",
          "url": "https://github.com/sourcemeta/one/commit/7a1e3b91576d18bb4107679d176655fa61de1a06"
        },
        "date": 1774381571573,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 628,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 81,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 636,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 209,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 122,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1057,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14650,
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
          "id": "5f7a697fb63cbf1b961446ffa487acec88240272",
          "message": "Fix various potential crashes in the indexer resolver (#774)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-25T09:53:32-04:00",
          "tree_id": "53c490e151bd54e625bec4495291b18b1e1daaa9",
          "url": "https://github.com/sourcemeta/one/commit/5f7a697fb63cbf1b961446ffa487acec88240272"
        },
        "date": 1774447792415,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 619,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 635,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 208,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 123,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1104,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 15256,
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
          "id": "fc61f15d7d22ca295fcfd9e7b1a0458681df82b8",
          "message": "Fix infinite cycles in configuration includes/extends (#775)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-25T10:32:32-04:00",
          "tree_id": "e0a6e9e93e79ca81cfe6bf907c8861eb63dbfa32",
          "url": "https://github.com/sourcemeta/one/commit/fc61f15d7d22ca295fcfd9e7b1a0458681df82b8"
        },
        "date": 1774450029396,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 600,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 624,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 199,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 124,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1078,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14242,
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
          "id": "dba66cfdabc77b97c5145e2fe4af82b4655f61b1",
          "message": "Upgrade Core to `b97cf5b157022765ae54380bbb0b65b8b2d1792f` (#776)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-25T10:56:42-04:00",
          "tree_id": "79e09236a992e61185954a63a6a6d0f1fcab00ae",
          "url": "https://github.com/sourcemeta/one/commit/dba66cfdabc77b97c5145e2fe4af82b4655f61b1"
        },
        "date": 1774451505337,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 67,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 540,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 69,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 681,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 178,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 118,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 982,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13247,
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
          "id": "a52e00f483200ca7e4de5b0ba393ec956cd4d044",
          "message": "Fuzz the indexer CLI and fix circular identifier/dialect URIs crash (#773)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-25T11:56:44-04:00",
          "tree_id": "d44a9deec2076b95c90789f1e188a0095639cda0",
          "url": "https://github.com/sourcemeta/one/commit/a52e00f483200ca7e4de5b0ba393ec956cd4d044"
        },
        "date": 1774456108391,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 92,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 779,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 93,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 775,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 262,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 135,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1301,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 16504,
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
          "id": "d451c3c517292ff0a2d5f0caeeb7708c02c9fce5",
          "message": "v5.1.1\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-25T14:36:02-04:00",
          "tree_id": "88b6184b74f61c8172b744e78219ea7f74f71fa9",
          "url": "https://github.com/sourcemeta/one/commit/d451c3c517292ff0a2d5f0caeeb7708c02c9fce5"
        },
        "date": 1774464729360,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 746,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 629,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 203,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 127,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1069,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14337,
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
          "id": "947760f152d88b7de5a9d22ca9caafabe97b15f2",
          "message": "Flaky test release hot fix\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-25T14:56:50-04:00",
          "tree_id": "47d9e7b73115f34a131621883a983258b00bec54",
          "url": "https://github.com/sourcemeta/one/commit/947760f152d88b7de5a9d22ca9caafabe97b15f2"
        },
        "date": 1774465942366,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 793,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 608,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 198,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 124,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1048,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14118,
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
          "id": "1275841c919f500764c70739482568a7068a8ed3",
          "message": "Automatically ignore configuration files in collection paths (#779)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-25T15:33:28-04:00",
          "tree_id": "96c4f6aa38c5a5f3d1970bccdcb2b6e48e641bc4",
          "url": "https://github.com/sourcemeta/one/commit/1275841c919f500764c70739482568a7068a8ed3"
        },
        "date": 1774468169337,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 85,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 720,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 85,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 734,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 38,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 305,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 124,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1094,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14706,
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
          "id": "dd5d96493c207076282378182f917e69b67ee986",
          "message": "Refactor the current search functionality into a search module (#781)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-25T16:08:38-04:00",
          "tree_id": "b07abb81f55aa7eef538323da8e3943ab3903bef",
          "url": "https://github.com/sourcemeta/one/commit/dd5d96493c207076282378182f917e69b67ee986"
        },
        "date": 1774470247333,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 86,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 867,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 87,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 707,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 38,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 303,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 131,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1120,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13941,
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
          "id": "00a07675e06b780624d7dd96be763a9767802355",
          "message": "Consider health scores in the search API (#783)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-25T16:27:18-04:00",
          "tree_id": "d8db7ef145dcc342e881a42cfffd8feb4c29b2bd",
          "url": "https://github.com/sourcemeta/one/commit/00a07675e06b780624d7dd96be763a9767802355"
        },
        "date": 1774471368340,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 84,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 805,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 87,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 705,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 39,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 300,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 124,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1065,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14304,
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
          "id": "aa4b1dd06e137097bbee0f6465592083760a222f",
          "message": "Turn the search index into a binary file (#784)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-26T08:50:59-04:00",
          "tree_id": "a4b7acc952a85efdc1b7c97d99bd5fd8ee2ddecc",
          "url": "https://github.com/sourcemeta/one/commit/aa4b1dd06e137097bbee0f6465592083760a222f"
        },
        "date": 1774530370318,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 612,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 643,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 263,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 114,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 999,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13674,
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
          "id": "7a5588799c0e4895e7718b4d6b612d1371e57015",
          "message": "Move the search limit constant to the server (#785)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-26T10:44:18-04:00",
          "tree_id": "87be0d2a79d21034031a27b9964c577e2fc0e358",
          "url": "https://github.com/sourcemeta/one/commit/7a5588799c0e4895e7718b4d6b612d1371e57015"
        },
        "date": 1774537219346,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 656,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 83,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 668,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 36,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 277,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 118,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1063,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13766,
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
          "id": "162be0919b606e1b58c546fa17d456fb275d6243",
          "message": "Internally support search scopes (#786)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-26T11:37:25-04:00",
          "tree_id": "62dd3238c9462fb76a798a4db5313a04b40578db",
          "url": "https://github.com/sourcemeta/one/commit/162be0919b606e1b58c546fa17d456fb275d6243"
        },
        "date": 1774540431313,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 648,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 757,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 35,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 280,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 164,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1046,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13248,
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
          "id": "99ae1394906849c812a9611ce31acaf5695306dc",
          "message": "Upgrade all Sourcemeta dependencies (#787)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-26T14:18:03-04:00",
          "tree_id": "27bf12751bb79446f5ec9949375b21daa85c6486",
          "url": "https://github.com/sourcemeta/one/commit/99ae1394906849c812a9611ce31acaf5695306dc"
        },
        "date": 1774550024342,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 86,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 704,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 94,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 707,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 40,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 302,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 132,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1056,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14224,
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
          "id": "e9f466cb05c05c70e89b0087899b22690cad5e89",
          "message": "Test that `$vocabulary` on pre 2019-09 is ignored (#788)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-26T15:14:01-04:00",
          "tree_id": "4d53db58dea09196afd2b17b36cd355dbb3c88ba",
          "url": "https://github.com/sourcemeta/one/commit/e9f466cb05c05c70e89b0087899b22690cad5e89"
        },
        "date": 1774553376337,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 624,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 647,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 264,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 115,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 952,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14023,
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
          "id": "ff84978e48cc8b22536eb37f263758aab32c7153",
          "message": "Test meta-schema validation of invalid `$vocabulary` (#789)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-26T15:52:57-04:00",
          "tree_id": "00c98af7b5afc8d535b753f4d2326720407c476b",
          "url": "https://github.com/sourcemeta/one/commit/ff84978e48cc8b22536eb37f263758aab32c7153"
        },
        "date": 1774555749339,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 84,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 829,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 86,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 705,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 38,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 299,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 122,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1039,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14439,
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
          "id": "27a390b3717717b9b66504767e64117ebc004de2",
          "message": "Use the registry resolver when compiling meta-schemas during index (#790)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-26T16:36:11-04:00",
          "tree_id": "8191dccdc07aa526f60a726a3529d4db69b33cd6",
          "url": "https://github.com/sourcemeta/one/commit/27a390b3717717b9b66504767e64117ebc004de2"
        },
        "date": 1774558322351,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 648,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 652,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 35,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 270,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 118,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1015,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13204,
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
          "id": "82795289999ecf766cdabac969c331bb79adb653",
          "message": "Handle `SchemaVocabularyError` with file paths during indexing (#791)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-27T09:00:04-04:00",
          "tree_id": "91259a6e0178172b9bd6b0ec211543b1df1030a8",
          "url": "https://github.com/sourcemeta/one/commit/82795289999ecf766cdabac969c331bb79adb653"
        },
        "date": 1774617305320,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 722,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 677,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 288,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 114,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1135,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13671,
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
          "id": "5c7543d5c62231b09ca72f9107de22af05b43920",
          "message": "Reject meta-schemas with unknown required vocabularies even if not used (#782)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-27T10:16:31-04:00",
          "tree_id": "fd48231d8e3547a178b4d5018ff84fd0d01715a7",
          "url": "https://github.com/sourcemeta/one/commit/5c7543d5c62231b09ca72f9107de22af05b43920"
        },
        "date": 1774621896330,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 86,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 754,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 87,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 701,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 40,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 317,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 118,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1050,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13574,
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
          "id": "68468d49c1094ab488a0a51352920b437b0d7041",
          "message": "v5.2.0\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-27T12:39:50-04:00",
          "tree_id": "753ea31b81a54d70dcf5b028a6b4cb8905ebd533",
          "url": "https://github.com/sourcemeta/one/commit/68468d49c1094ab488a0a51352920b437b0d7041"
        },
        "date": 1774630488281,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 60,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 742,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 60,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 495,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 173,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 123,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 754,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 11479,
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
          "id": "68468d49c1094ab488a0a51352920b437b0d7041",
          "message": "v5.2.0\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-27T16:39:50Z",
          "url": "https://github.com/sourcemeta/one/commit/68468d49c1094ab488a0a51352920b437b0d7041"
        },
        "date": 1774640712544,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 86,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 614,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 84,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 638,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 37,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 265,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 116,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1060,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13280,
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
          "id": "399ded576b1f31624e0eb3a720eaa7187a9d0006",
          "message": "Document how the `@self/v1` extend is what pulls the API (#793)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-27T16:36:02-04:00",
          "tree_id": "8c011839cb2791f5f67c0b78c4795eb5428e6aab",
          "url": "https://github.com/sourcemeta/one/commit/399ded576b1f31624e0eb3a720eaa7187a9d0006"
        },
        "date": 1774644684341,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 628,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 638,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 265,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 115,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1016,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14830,
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
          "id": "399ded576b1f31624e0eb3a720eaa7187a9d0006",
          "message": "Document how the `@self/v1` extend is what pulls the API (#793)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-27T20:36:02Z",
          "url": "https://github.com/sourcemeta/one/commit/399ded576b1f31624e0eb3a720eaa7187a9d0006"
        },
        "date": 1774901508603,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 635,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 82,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 663,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 36,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 271,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 119,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1011,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13611,
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
          "id": "a5ffb13765a3eecfba074de52be0c7f759c55029",
          "message": "Update `locations` API to return JSON Pointers as arrays of tokens (#794)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-30T18:17:12-04:00",
          "tree_id": "65f23aecb3c87c23c2f8bace0d3f2fe2585b4ca8",
          "url": "https://github.com/sourcemeta/one/commit/a5ffb13765a3eecfba074de52be0c7f759c55029"
        },
        "date": 1774909990443,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 612,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 614,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 251,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 118,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1190,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13207,
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
          "id": "f7a3cc911d38413237339af51989665262a0b4fc",
          "message": "Extract HTTP server wrappers to their own module (#795)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-31T09:59:06-04:00",
          "tree_id": "d11a77a3cc7f21ff5b980ad465be6a7dba67a284",
          "url": "https://github.com/sourcemeta/one/commit/f7a3cc911d38413237339af51989665262a0b4fc"
        },
        "date": 1774968229380,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 613,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 82,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 819,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 285,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 110,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1042,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12805,
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
          "id": "f72ba1abc283def0b0103b9814f5977904fc493e",
          "message": "Extract server actions into its own module (#796)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-31T12:09:16-04:00",
          "tree_id": "12edca3251c9cf6572b7d9ca31c5740cca7a67ab",
          "url": "https://github.com/sourcemeta/one/commit/f72ba1abc283def0b0103b9814f5977904fc493e"
        },
        "date": 1774974314324,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 886,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 901,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 37,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 292,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 112,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1075,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13006,
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
          "id": "7104ae31a5b8ed64d0ba8b67adf7c36ee2c974b8",
          "message": "Support passing router arguments to the server action handlers (#797)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-31T14:22:37-04:00",
          "tree_id": "e57c5a16d1fbb9dbe9bbd951bb6474277c0171f2",
          "url": "https://github.com/sourcemeta/one/commit/7104ae31a5b8ed64d0ba8b67adf7c36ee2c974b8"
        },
        "date": 1774982285315,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 601,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 641,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 246,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 111,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 958,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13223,
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
          "id": "7104ae31a5b8ed64d0ba8b67adf7c36ee2c974b8",
          "message": "Support passing router arguments to the server action handlers (#797)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-31T18:22:37Z",
          "url": "https://github.com/sourcemeta/one/commit/7104ae31a5b8ed64d0ba8b67adf7c36ee2c974b8"
        },
        "date": 1774986433593,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 802,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 661,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 270,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 117,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1079,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14259,
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
          "id": "1f7aa2a8b2e554025e7f12f0b4a7929923366be6",
          "message": "Better encapsulate action definitions in `src/actions` (#798)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-31T18:06:35-04:00",
          "tree_id": "8f583dd80ef76e9878df3a79351e1ca26f8ed89f",
          "url": "https://github.com/sourcemeta/one/commit/1f7aa2a8b2e554025e7f12f0b4a7929923366be6"
        },
        "date": 1774995769323,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 844,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 830,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 35,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 291,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 112,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1101,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13052,
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
          "id": "5475912f72175daabdcb8f0f81cdd97c4fec2f96",
          "message": "Turn action definitions into classes (#799)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-01T10:34:45-04:00",
          "tree_id": "b06e8907cc451c0727129937cc6ee3033a10ce78",
          "url": "https://github.com/sourcemeta/one/commit/5475912f72175daabdcb8f0f81cdd97c4fec2f96"
        },
        "date": 1775055261326,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1036,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 82,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 692,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 294,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 127,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 988,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13364,
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
          "id": "6eb3d73dab5592baf75bbbcb1eb7f25a58ef089d",
          "message": "Cleanup action argument dispatching (#800)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-01T19:16:21Z",
          "url": "https://github.com/sourcemeta/one/commit/6eb3d73dab5592baf75bbbcb1eb7f25a58ef089d"
        },
        "date": 1775073273635,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 604,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 72,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 616,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 252,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 118,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 987,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12755,
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
          "id": "6fed5aeee8e2274af8db7bc70c8cf82d9ca80813",
          "message": "Take all action arguments through the router (#801)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-01T16:04:02-04:00",
          "tree_id": "38b302ab1bd1b83a5c9fcb2af462ce48d37cd6e8",
          "url": "https://github.com/sourcemeta/one/commit/6fed5aeee8e2274af8db7bc70c8cf82d9ca80813"
        },
        "date": 1775074782315,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 881,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 941,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 253,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 112,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1075,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12657,
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
          "id": "e45ee90fc7b1e542a49df2f472b24242324b2372",
          "message": "Upgrade all Sourcemeta dependencies (#802)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-01T18:00:54-04:00",
          "tree_id": "29e607b7f74f17952231b935d0992a75940bff11",
          "url": "https://github.com/sourcemeta/one/commit/e45ee90fc7b1e542a49df2f472b24242324b2372"
        },
        "date": 1775081798433,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 70,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 583,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 605,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 245,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 109,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 960,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13200,
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
          "id": "1d6a37bcc5cf77d315a4ae38c9023a5a41f56575",
          "message": "Extensively test base URLs with path at the indexer level (#805)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-02T12:59:16-04:00",
          "tree_id": "1800bc27178ee61ec8026fb2519c69e300575cbd",
          "url": "https://github.com/sourcemeta/one/commit/1d6a37bcc5cf77d315a4ae38c9023a5a41f56575"
        },
        "date": 1775151319343,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 72,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 753,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 620,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 276,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 128,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1082,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14264,
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
          "id": "aca182e4648e3bd12c30c0333384e1f4b2408d9f",
          "message": "Revise various documentation inconsistencies and clarification (#804)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-02T13:43:16-04:00",
          "tree_id": "d86f90fe2bf4519143148ea38461dd3cdea87960",
          "url": "https://github.com/sourcemeta/one/commit/aca182e4648e3bd12c30c0333384e1f4b2408d9f"
        },
        "date": 1775152749337,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 615,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 604,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 248,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 143,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1137,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12405,
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
          "id": "3f2bc046198e6ef79a1e12d716719fbed1d099b0",
          "message": "Upgrade Blaze to `83e2458856b934aa7995c44da36ab09152bdfc8d` (#806)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-02T14:03:34-04:00",
          "tree_id": "135eb3e0632e32436a5d89f80c1ade320e84104d",
          "url": "https://github.com/sourcemeta/one/commit/3f2bc046198e6ef79a1e12d716719fbed1d099b0"
        },
        "date": 1775153958340,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 606,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 610,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 252,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 110,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1017,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14123,
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
          "id": "b4c34b3bf422c9c74b47a0d2f118250cde7cd905",
          "message": "Fix broken `/self/v1` extend with a base URL with path (#808)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-02T19:11:36Z",
          "url": "https://github.com/sourcemeta/one/commit/b4c34b3bf422c9c74b47a0d2f118250cde7cd905"
        },
        "date": 1775159332605,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 72,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 620,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 636,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 258,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 108,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1004,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13462,
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
          "id": "d61c5d6faf3f26f66cd293f6fab5f3c161917fbf",
          "message": "Upgrade Core to `fd7f65fab438a1bae94b6646c83b4f9168e50134` (#810)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-03T10:58:39-04:00",
          "tree_id": "cbcec3e759b35f550dd0a5f579ea9c200d9e6794",
          "url": "https://github.com/sourcemeta/one/commit/d61c5d6faf3f26f66cd293f6fab5f3c161917fbf"
        },
        "date": 1775229285343,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 709,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 638,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 276,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 118,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1084,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13778,
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
          "id": "c20a47a684c39c5e3b9512cca47f2178f146d434",
          "message": "Upgrade Core to `b6640a840bf149edca223c9129a71d64474e12fc` (#812)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-03T13:25:35-04:00",
          "tree_id": "1837f55cbb440ef794dd728a92c7085987967d7d",
          "url": "https://github.com/sourcemeta/one/commit/c20a47a684c39c5e3b9512cca47f2178f146d434"
        },
        "date": 1775238149477,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 688,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 625,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 297,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 108,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 998,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13831,
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
          "id": "1ab17eb3822fa91f4e19518c8a8bd8445d9108a5",
          "message": "Revise dynamic libraries kept in Kraft for `public` (#809)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-03T19:07:11Z",
          "url": "https://github.com/sourcemeta/one/commit/1ab17eb3822fa91f4e19518c8a8bd8445d9108a5"
        },
        "date": 1775245329653,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 906,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 836,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 258,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 114,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 910,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13258,
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
          "id": "6eda1dc316c954f8df254429933744e5a20aef8c",
          "message": "Run Hurl tests multiple times to catch flakiness (#813)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-03T15:44:23-04:00",
          "tree_id": "2700da01c6898d5b71253b1ea4af083bf0316c1e",
          "url": "https://github.com/sourcemeta/one/commit/6eda1dc316c954f8df254429933744e5a20aef8c"
        },
        "date": 1775246468343,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 920,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 689,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 255,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 117,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1044,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14263,
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
          "id": "f31a369ad6c9e8026d6640c787ec1a52c85076ef",
          "message": "Support running on base URLs with path components (#807)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-03T19:02:00-04:00",
          "tree_id": "96496ea30577a9f53e5160ae5000a36980570718",
          "url": "https://github.com/sourcemeta/one/commit/f31a369ad6c9e8026d6640c787ec1a52c85076ef"
        },
        "date": 1775258330334,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 613,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 758,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 273,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 108,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1053,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13276,
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
          "id": "f47586978c18a7b087305a442cdf4482a1777263",
          "message": "v5.3.0\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-03T19:20:03-04:00",
          "tree_id": "53f68b2f4de9fc38272a65fcb9191bebfbec5845",
          "url": "https://github.com/sourcemeta/one/commit/f47586978c18a7b087305a442cdf4482a1777263"
        },
        "date": 1775259421313,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 72,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 577,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 588,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 244,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 113,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1028,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12461,
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
          "id": "f47586978c18a7b087305a442cdf4482a1777263",
          "message": "v5.3.0\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-03T23:20:03Z",
          "url": "https://github.com/sourcemeta/one/commit/f47586978c18a7b087305a442cdf4482a1777263"
        },
        "date": 1775505642725,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 916,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 890,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 289,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 111,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 972,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13415,
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
          "id": "09f5de3f140d4ee09d027ebc337fd7d7d93a2b19",
          "message": "Kickstart a larger documentation guide (#814)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-07T13:22:03-04:00",
          "tree_id": "0a0604c7dbd1884941cab053ad0de7ac4308f35f",
          "url": "https://github.com/sourcemeta/one/commit/09f5de3f140d4ee09d027ebc337fd7d7d93a2b19"
        },
        "date": 1775583514336,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 689,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 644,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 290,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 139,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1003,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13188,
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
          "id": "102d7df4ebcc9f9ce35047a18aaf54acb1c16083",
          "message": "Add a new guide entry on incrementally adopting schema governance (#815)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-07T14:29:40-04:00",
          "tree_id": "8b2be9dc3fbc04ca31936a79482fb1bccc82b8f2",
          "url": "https://github.com/sourcemeta/one/commit/102d7df4ebcc9f9ce35047a18aaf54acb1c16083"
        },
        "date": 1775587608385,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 906,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 898,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 307,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 109,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1159,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13422,
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
          "id": "102d7df4ebcc9f9ce35047a18aaf54acb1c16083",
          "message": "Add a new guide entry on incrementally adopting schema governance (#815)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-07T18:29:40Z",
          "url": "https://github.com/sourcemeta/one/commit/102d7df4ebcc9f9ce35047a18aaf54acb1c16083"
        },
        "date": 1775591322633,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 896,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 632,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 35,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 282,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 114,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1063,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14060,
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
          "id": "0af51d1236009f0b2d43a4c4f941b7b422e5a1f7",
          "message": "Describe schema evolution and versioning in the guide (#816)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-08T09:00:41-04:00",
          "tree_id": "77a8553ea167b6c8d2109bfd6d9f1447f11f45ab",
          "url": "https://github.com/sourcemeta/one/commit/0af51d1236009f0b2d43a4c4f941b7b422e5a1f7"
        },
        "date": 1775654278363,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 688,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 627,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 253,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 111,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 996,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13859,
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
          "id": "4281d8ab67301a6c910c13ef6dfaef9bd216be1c",
          "message": "Minor documentation revisions (#818)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-08T09:41:11-04:00",
          "tree_id": "da65e34386a3fda3fb55787d87ebcbc42da93779",
          "url": "https://github.com/sourcemeta/one/commit/4281d8ab67301a6c910c13ef6dfaef9bd216be1c"
        },
        "date": 1775656773477,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 690,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 634,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 258,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 111,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1174,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13600,
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
          "id": "d716578bad6972a570691fe7055ff7517092289c",
          "message": "Upgrade Core to `c91f056db173406f413660fdee97d72fc1506fa8` (#819)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-08T14:48:59-04:00",
          "tree_id": "8134d5ecef838097271d65c73915f8030b68fa43",
          "url": "https://github.com/sourcemeta/one/commit/d716578bad6972a570691fe7055ff7517092289c"
        },
        "date": 1775675541329,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 760,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 622,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 271,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 116,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 973,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13355,
            "unit": "ms"
          }
        ]
      }
    ]
  }
}