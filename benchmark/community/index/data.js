window.BENCHMARK_DATA = {
  "lastUpdate": 1773845570215,
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
          "id": "17bb92c659294d288883dd0ab238a5b9776c477c",
          "message": "Speed up the detection and resolution indexing phases by ~3x (#697)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-03T15:42:28-04:00",
          "tree_id": "8d28b206437286e488acd0509a1608c866ed42ff",
          "url": "https://github.com/sourcemeta/one/commit/17bb92c659294d288883dd0ab238a5b9776c477c"
        },
        "date": 1772567618353,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 44,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 921,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 9290,
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
        "date": 1772583391362,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 44,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 1000,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 9431,
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
        "date": 1772630178354,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 41,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 455,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 4594,
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
        "date": 1772631007354,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 54,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 473,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 4564,
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
        "date": 1772632190410,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 40,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 382,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 3882,
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
        "date": 1772642393346,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 51,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 438,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 4472,
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
        "date": 1772653115814,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 43,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 405,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 4108,
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
        "date": 1772657904360,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 44,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 394,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 4099,
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
        "date": 1772740458852,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 44,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 423,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 4141,
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
        "date": 1772808127357,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 43,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 426,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 4260,
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
        "date": 1772813405397,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 45,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 324,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 3007,
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
        "date": 1772815579347,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 42,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 280,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 2852,
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
        "date": 1772825683354,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 35,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 252,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 2579,
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
        "date": 1772831494339,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 231,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 2501,
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
        "date": 1773067123370,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 230,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 2894,
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
        "date": 1773068395341,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 152,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 1281,
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
        "date": 1773074937370,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 164,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 1235,
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
        "date": 1773077715370,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 140,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 1199,
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
        "date": 1773078825379,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 160,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 994,
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
        "date": 1773079647344,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 92,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 745,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 62,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 507,
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
        "date": 1773082623284,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 52,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 397,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 250,
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
        "date": 1773084826664,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 56,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 393,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 251,
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
        "date": 1773087414326,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 636,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 50,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 400,
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
        "date": 1773150618293,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 44,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 315,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 179,
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
        "date": 1773160317493,
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
            "value": 548,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 5667,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 41,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 315,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 3046,
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
        "date": 1773171327855,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 71,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 547,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 5871,
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
            "value": 308,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 3030,
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
        "date": 1773257953570,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 69,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 522,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 5437,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 40,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 290,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 2922,
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
        "date": 1773344250612,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 67,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 519,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 5377,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 40,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 290,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 2924,
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
        "date": 1773401196337,
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
            "value": 281,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 2967,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 123,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 1457,
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
        "date": 1773403909357,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 53,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 380,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 4351,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 120,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 1292,
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
        "date": 1773408935347,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 55,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 385,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 4234,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 114,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 1197,
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
        "date": 1773423610337,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 84,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 658,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 6944,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 143,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 1363,
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
        "date": 1773426486283,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 67,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 544,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 5708,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 116,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 1198,
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
        "date": 1773430344802,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 82,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 651,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 6834,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 140,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 1396,
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
          "id": "95859464b67955768c818daa4792a31b52b55afb",
          "message": "Upgrade Core to `aa009dbb8a7340b89d0af24110913bbe3bb5df92` (#729)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-16T09:45:58-04:00",
          "tree_id": "354f7aa18af77a5c13219fce857e8e38b8c1e7c6",
          "url": "https://github.com/sourcemeta/one/commit/95859464b67955768c818daa4792a31b52b55afb"
        },
        "date": 1773669620343,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 81,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 644,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 7261,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 130,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 1350,
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
        "date": 1773670470348,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 579,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 6182,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 672,
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
        "date": 1773671620354,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 576,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 6380,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 674,
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
        "date": 1773673192347,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 51,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 314,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3427,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 648,
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
        "date": 1773677209333,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 51,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 323,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3538,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 48,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 396,
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
        "date": 1773678532347,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 52,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 325,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3633,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 52,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 449,
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
        "date": 1773690303591,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
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
            "value": 3266,
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
            "value": 28,
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
        "date": 1773691357344,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 47,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 307,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3414,
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
            "value": 30,
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
          "id": "b64474819b9e61f74e54829d11812e4611daf59c",
          "message": "Build dependents directly out of the build state (#742)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-17T09:52:46-04:00",
          "tree_id": "33cc99d1e6d9c1178f6082341a59c06fd9ac009f",
          "url": "https://github.com/sourcemeta/one/commit/b64474819b9e61f74e54829d11812e4611daf59c"
        },
        "date": 1773756412352,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 44,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 294,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3875,
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
            "value": 28,
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
        "date": 1773776565620,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 42,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 282,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3277,
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
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 208,
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
        "date": 1773782855328,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 42,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 283,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 3367,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 42,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 248,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 2702,
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
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 227,
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
        "date": 1773842732544,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 39,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 208,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 2012,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 44,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 225,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 2307,
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
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 259,
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
        "date": 1773845569466,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 43,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 195,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1955,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
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
            "value": 2168,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 10,
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
            "value": 240,
            "unit": "ms"
          }
        ]
      }
    ]
  }
}