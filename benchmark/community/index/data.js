window.BENCHMARK_DATA = {
  "lastUpdate": 1777239808580,
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
        "date": 1773855312337,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 38,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 187,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1930,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 38,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 193,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 2251,
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
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 232,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 155,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1557,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 53339,
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
        "date": 1773859894467,
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
            "value": 211,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 2129,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 41,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 203,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 2207,
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
            "value": 200,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1355,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 16897,
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
        "date": 1773862990583,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 40,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 197,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1987,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 37,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 188,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1955,
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
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 232,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 157,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1327,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 16889,
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
        "date": 1773865028489,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 149,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1614,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 155,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1573,
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
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 229,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 132,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1204,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14927,
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
        "date": 1773868509349,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 35,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 164,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1702,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 160,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1670,
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
            "value": 244,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 154,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1235,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 15919,
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
        "date": 1773927360297,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 138,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1462,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 130,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1436,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 26,
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
            "value": 893,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12838,
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
        "date": 1773937153344,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 35,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 157,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1668,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 159,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1678,
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
            "value": 241,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 135,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1171,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 15907,
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
          "id": "5d3768131942a7cc5e172914904e066ffa1957d7",
          "message": "Upgrade Core to `dcda38db88896195eec9590cb609bcc0e906ad2d` (#757)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-19T13:48:20-04:00",
          "tree_id": "0cbc06f4eba3117fb82b7ca5c6bee074e72d3fad",
          "url": "https://github.com/sourcemeta/one/commit/5d3768131942a7cc5e172914904e066ffa1957d7"
        },
        "date": 1773943451285,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 137,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1532,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 138,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1534,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 165,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 97,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 915,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 11970,
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
        "date": 1773944991337,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 37,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 155,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1751,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 159,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1666,
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
            "value": 1274,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 15312,
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
        "date": 1773949290570,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 154,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1672,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 154,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1644,
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
            "value": 230,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 142,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1259,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 15027,
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
          "id": "eb70364556f5140fbdc615e21335c051d004dd5b",
          "message": "Upgrade Core to `3c800567cf63aea812b03d97d39adff1f9530d16` (#760)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-19T16:18:18-04:00",
          "tree_id": "0cdcc564e19ceeecfd248d1194f7190616c51ead",
          "url": "https://github.com/sourcemeta/one/commit/eb70364556f5140fbdc615e21335c051d004dd5b"
        },
        "date": 1773952313332,
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
            "value": 162,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1729,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 154,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 1720,
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
            "value": 269,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 125,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1049,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14896,
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
        "date": 1773953693331,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 36,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 111,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 947,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 114,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 908,
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
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 194,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 142,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1090,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14634,
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
        "date": 1774028585351,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 45,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 110,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 925,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 112,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 874,
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
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 197,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 146,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1144,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14462,
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
        "date": 1774029701335,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 83,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 683,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
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
            "value": 639,
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
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 190,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 117,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 996,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14248,
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
        "date": 1774035980544,
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
            "value": 86,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 686,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 82,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 637,
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
            "value": 188,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 118,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1154,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14786,
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
        "date": 1774037087368,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
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
            "value": 617,
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
            "value": 630,
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
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 193,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 114,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 973,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13959,
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
        "date": 1774040307331,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
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
            "value": 606,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 624,
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
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 194,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 117,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 981,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13637,
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
        "date": 1774042620323,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
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
            "value": 608,
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
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 619,
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
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 193,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 117,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1141,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14216,
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
        "date": 1774105931330,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 624,
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
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 645,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 202,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 131,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 972,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14132,
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
        "date": 1774274352432,
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
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 717,
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
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 200,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 127,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 965,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13701,
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
        "date": 1774278640336,
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
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 636,
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
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 198,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 135,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 991,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13872,
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
        "date": 1774281152368,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
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
            "value": 625,
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
            "value": 664,
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
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 197,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 162,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1055,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14173,
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
        "date": 1774295633722,
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
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 658,
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
            "value": 664,
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
            "value": 206,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 136,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1010,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13603,
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
        "date": 1774381484618,
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
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 653,
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
            "value": 85,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 668,
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
            "value": 204,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 118,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1047,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14322,
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
        "date": 1774447757327,
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
            "value": 68,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 563,
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
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 595,
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
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 178,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 157,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1213,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14280,
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
        "date": 1774449980342,
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
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 627,
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
            "value": 79,
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
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 194,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 118,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 986,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13707,
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
        "date": 1774451489348,
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
            "value": 650,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 644,
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
            "value": 199,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 120,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 999,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13794,
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
        "date": 1774455115349,
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
            "value": 70,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 586,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 600,
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
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 186,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 121,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1046,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14704,
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
        "date": 1774464685424,
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
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 619,
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
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 639,
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
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 194,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 118,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 957,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13337,
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
        "date": 1774465910339,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 622,
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
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 643,
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
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 194,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 147,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1175,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14868,
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
        "date": 1774468124343,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 28,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 85,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 809,
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
            "value": 87,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 743,
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
            "value": 38,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 297,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 125,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 989,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14453,
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
        "date": 1774470238348,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 26,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 87,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 904,
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
            "value": 90,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 766,
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
            "value": 38,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 311,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 150,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1004,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14287,
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
        "date": 1774471307333,
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
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 819,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 673,
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
            "value": 109,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 888,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13507,
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
        "date": 1774530343335,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 655,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
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
            "value": 661,
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
            "value": 267,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 152,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1267,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14710,
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
        "date": 1774537158333,
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
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 678,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
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
            "value": 695,
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
            "value": 270,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 120,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1071,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13530,
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
        "date": 1774540351449,
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
            "value": 86,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 729,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
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
            "value": 735,
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
            "value": 37,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 299,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 125,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1128,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13919,
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
        "date": 1774551747343,
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
            "value": 84,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 732,
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
            "value": 87,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 731,
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
            "value": 37,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 297,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 116,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1099,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13792,
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
        "date": 1774553413421,
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
            "value": 84,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 674,
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
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 673,
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
            "value": 280,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 150,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1115,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14448,
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
          "id": "e9f466cb05c05c70e89b0087899b22690cad5e89",
          "message": "Test that `$vocabulary` on pre 2019-09 is ignored (#788)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-03-26T19:14:01Z",
          "url": "https://github.com/sourcemeta/one/commit/e9f466cb05c05c70e89b0087899b22690cad5e89"
        },
        "date": 1774554727624,
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
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 666,
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
            "value": 79,
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
            "value": 263,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 112,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 906,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13518,
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
        "date": 1774555635278,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 64,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 537,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 66,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 546,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 175,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 104,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 811,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 10866,
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
        "date": 1774558256328,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
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
            "value": 660,
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
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 667,
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
            "value": 265,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 163,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1166,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14141,
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
        "date": 1774617318335,
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
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 660,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 81,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 685,
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
            "value": 270,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 148,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1145,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13967,
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
        "date": 1774621891322,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 663,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 675,
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
            "value": 268,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 111,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 910,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13445,
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
        "date": 1774630512336,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 647,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
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
            "value": 650,
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
            "value": 263,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 159,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1065,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13700,
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
        "date": 1774640787616,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 654,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
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
            "value": 666,
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
            "value": 120,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 974,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13387,
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
        "date": 1774644639328,
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
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 659,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 676,
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
            "value": 263,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 143,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1083,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14082,
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
        "date": 1774900585817,
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
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 647,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
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
            "value": 659,
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
            "value": 259,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 165,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1167,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14463,
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
        "date": 1774909965328,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 886,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 883,
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
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 329,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 121,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 968,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13191,
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
        "date": 1774966475330,
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
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 639,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
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
            "value": 653,
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
            "value": 248,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 118,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1023,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13649,
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
        "date": 1774974283335,
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
            "value": 663,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
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
            "value": 648,
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
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 249,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 112,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 962,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12921,
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
        "date": 1774982323345,
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
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 681,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 18,
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
            "value": 719,
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
            "value": 264,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 129,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1065,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13760,
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
        "date": 1774987023602,
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
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 662,
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
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 674,
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
            "value": 256,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 112,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 991,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13559,
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
        "date": 1774995736334,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 639,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 660,
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
            "value": 254,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 135,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1167,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13255,
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
        "date": 1775055196316,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
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
            "value": 638,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
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
            "value": 645,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
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
            "value": 248,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 129,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1026,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13434,
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
        "date": 1775073263681,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 29,
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
            "value": 633,
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
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 646,
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
            "value": 250,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 140,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1035,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12730,
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
        "date": 1775074691278,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 60,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 729,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 62,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 525,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 8,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 165,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 107,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 789,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 11486,
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
        "date": 1775081813337,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 627,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
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
            "value": 646,
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
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 251,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 145,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1104,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13780,
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
        "date": 1775150079320,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 643,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
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
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 247,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 125,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1077,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13066,
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
        "date": 1775152660275,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 59,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 499,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 61,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 511,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 8,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 159,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 107,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 845,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 10928,
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
        "date": 1775153946319,
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
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 627,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 676,
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
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 243,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 131,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1003,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13126,
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
        "date": 1775159425711,
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
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 611,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 72,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 645,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 256,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 127,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 963,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13531,
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
        "date": 1775229263318,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 656,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 840,
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
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 268,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 125,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1019,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13124,
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
        "date": 1775238166345,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 631,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 645,
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
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 248,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 109,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 865,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 27513,
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
        "date": 1775245065552,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 81,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 685,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 81,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 694,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
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
            "value": 279,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 132,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1041,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13229,
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
        "date": 1775246385335,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
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
            "value": 939,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 913,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 8,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 292,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 140,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1055,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13008,
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
        "date": 1775258299356,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 945,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 658,
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
            "value": 112,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 991,
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
        "date": 1775259457373,
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
            "value": 82,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 719,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 87,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 743,
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
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 285,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 122,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 913,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 15862,
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
        "date": 1775504996649,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 928,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
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
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 304,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 121,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 977,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13645,
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
        "date": 1775583500434,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
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
            "value": 868,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 884,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 259,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 130,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1092,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13101,
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
        "date": 1775588014325,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 892,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 815,
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
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 320,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 121,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 998,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12810,
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
        "date": 1775591326586,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
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
            "value": 1061,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 689,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 278,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 158,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1082,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13583,
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
        "date": 1775654189331,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 826,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 79,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 843,
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
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 288,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 121,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 976,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12911,
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
        "date": 1775656721322,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 629,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 76,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 922,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 8,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 249,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 116,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 915,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12555,
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
        "date": 1775675129409,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 72,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 615,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 72,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 700,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 248,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 115,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 966,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12294,
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
          "id": "d716578bad6972a570691fe7055ff7517092289c",
          "message": "Upgrade Core to `c91f056db173406f413660fdee97d72fc1506fa8` (#819)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-08T18:48:59Z",
          "url": "https://github.com/sourcemeta/one/commit/d716578bad6972a570691fe7055ff7517092289c"
        },
        "date": 1775678202661,
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
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 758,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
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
            "value": 727,
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
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 267,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 112,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1025,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 15022,
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
          "id": "44e3faf2f4849578b7977dbbe6f2df09d5b5c4a5",
          "message": "Introduce an `Action` virtual class (#820)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-08T16:54:49-04:00",
          "tree_id": "18bbbd59a4e1c2766951af15fc97e1637b6eb413",
          "url": "https://github.com/sourcemeta/one/commit/44e3faf2f4849578b7977dbbe6f2df09d5b5c4a5"
        },
        "date": 1775682644343,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 628,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 805,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 8,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 243,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 113,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 985,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13133,
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
          "id": "8434752528dc6254255eed44aea6fa5bbe3b17b8",
          "message": "Implement fully dynamic action dispatching (#821)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-08T17:39:58-04:00",
          "tree_id": "a49478c57e6813bc68f8de847a04d9c12fd53d3b",
          "url": "https://github.com/sourcemeta/one/commit/8434752528dc6254255eed44aea6fa5bbe3b17b8"
        },
        "date": 1775685393369,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 726,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 81,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 715,
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
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 283,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 126,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1057,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13435,
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
          "id": "9658355111d138d39a5779fe7fe7d530bfae3f3d",
          "message": "Version every internal action (#822)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-08T18:04:01-04:00",
          "tree_id": "d735113d5f76abd7286620e5b2fd65877a6508f7",
          "url": "https://github.com/sourcemeta/one/commit/9658355111d138d39a5779fe7fe7d530bfae3f3d"
        },
        "date": 1775688629328,
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
            "value": 82,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 837,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 85,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 718,
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
            "value": 36,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 288,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 123,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 986,
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
          "id": "414a71a031217860d38d838ac6aa8ebefd09a404",
          "message": "Take most schemas as route arguments (#823)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-08T18:54:09-04:00",
          "tree_id": "3381559c901f7f09eef570d8a9e9cb5547f56a68",
          "url": "https://github.com/sourcemeta/one/commit/414a71a031217860d38d838ac6aa8ebefd09a404"
        },
        "date": 1775689831322,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 663,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 929,
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
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 261,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 104,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 940,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13112,
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
          "id": "a90a634f4e2f120a0afa7aa5ec8d94eb5e25849e",
          "message": "Make use of default router arguments (#824)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-09T09:30:23-04:00",
          "tree_id": "72b1e00947dac8c6296e40d14860b647fe65c9d4",
          "url": "https://github.com/sourcemeta/one/commit/a90a634f4e2f120a0afa7aa5ec8d94eb5e25849e"
        },
        "date": 1775742429331,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 657,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 665,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 252,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 124,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 964,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13014,
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
          "id": "c088f9eaa2a56673096d7fb4b5b2d495715ee947",
          "message": "v5.3.1\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-09T10:23:20-04:00",
          "tree_id": "99a55ce3eff7e8a1e0972b12a5fd90b0f5af452b",
          "url": "https://github.com/sourcemeta/one/commit/c088f9eaa2a56673096d7fb4b5b2d495715ee947"
        },
        "date": 1775745638329,
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
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 629,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
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
            "value": 639,
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
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 249,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 161,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1023,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13219,
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
          "id": "c088f9eaa2a56673096d7fb4b5b2d495715ee947",
          "message": "v5.3.1\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-09T14:23:20Z",
          "url": "https://github.com/sourcemeta/one/commit/c088f9eaa2a56673096d7fb4b5b2d495715ee947"
        },
        "date": 1775764414630,
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
            "value": 82,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 737,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 86,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 693,
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
            "value": 34,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 284,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 104,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 844,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14493,
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
          "id": "c088f9eaa2a56673096d7fb4b5b2d495715ee947",
          "message": "v5.3.1\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-09T14:23:20Z",
          "url": "https://github.com/sourcemeta/one/commit/c088f9eaa2a56673096d7fb4b5b2d495715ee947"
        },
        "date": 1775850301694,
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
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 700,
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
            "value": 82,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 729,
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
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 267,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 140,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1106,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14627,
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
          "id": "c088f9eaa2a56673096d7fb4b5b2d495715ee947",
          "message": "v5.3.1\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-09T14:23:20Z",
          "url": "https://github.com/sourcemeta/one/commit/c088f9eaa2a56673096d7fb4b5b2d495715ee947"
        },
        "date": 1776109994556,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 730,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 801,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 282,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 123,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 969,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13011,
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
          "id": "c088f9eaa2a56673096d7fb4b5b2d495715ee947",
          "message": "v5.3.1\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-09T14:23:20Z",
          "url": "https://github.com/sourcemeta/one/commit/c088f9eaa2a56673096d7fb4b5b2d495715ee947"
        },
        "date": 1776196330560,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 671,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 86,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 830,
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
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 274,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 127,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1030,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12506,
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
          "id": "c088f9eaa2a56673096d7fb4b5b2d495715ee947",
          "message": "v5.3.1\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-09T14:23:20Z",
          "url": "https://github.com/sourcemeta/one/commit/c088f9eaa2a56673096d7fb4b5b2d495715ee947"
        },
        "date": 1776283428589,
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
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 817,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
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
            "value": 824,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 8,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 259,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 104,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 936,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13031,
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
          "id": "2b5a294f5bcce60304f5143cf9a7e1dc535276a9",
          "message": "Upgrade uWebSockets to v20.77.0 (#826)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-15T18:27:14-04:00",
          "tree_id": "7b255e6e0776117c20b5cde15923586f00a32c8e",
          "url": "https://github.com/sourcemeta/one/commit/2b5a294f5bcce60304f5143cf9a7e1dc535276a9"
        },
        "date": 1776293429312,
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
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 630,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 635,
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
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 243,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 105,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 989,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13133,
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
          "id": "4f378bb5c986d8d21b71a1688cabe654c9a48c1a",
          "message": "Describe ingesting schemas with `$id` in the getting started guide (#827)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-15T20:30:28-04:00",
          "tree_id": "a701e487038d79639ebfc07fb7f968277750b2f6",
          "url": "https://github.com/sourcemeta/one/commit/4f378bb5c986d8d21b71a1688cabe654c9a48c1a"
        },
        "date": 1776300646308,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 62,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 682,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 61,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 540,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 8,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 180,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 116,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 853,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 11232,
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
          "id": "d82ae1c8ff06ad306426e48b494898e818e5eb3a",
          "message": "Add indexing tests on schemas / collection id duplication (#828)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-15T21:02:05-04:00",
          "tree_id": "dcd694598843e75683d516dc183afa749c122666",
          "url": "https://github.com/sourcemeta/one/commit/d82ae1c8ff06ad306426e48b494898e818e5eb3a"
        },
        "date": 1776302954320,
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
            "value": 72,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 759,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 72,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 644,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 8,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 247,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 126,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1002,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13534,
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
          "id": "d82ae1c8ff06ad306426e48b494898e818e5eb3a",
          "message": "Add indexing tests on schemas / collection id duplication (#828)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-16T01:02:05Z",
          "url": "https://github.com/sourcemeta/one/commit/d82ae1c8ff06ad306426e48b494898e818e5eb3a"
        },
        "date": 1776369308617,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 1054,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 84,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 687,
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
            "value": 33,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 286,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 117,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 921,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12923,
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
          "id": "118b39b0ab062e6ff95d117d0f4810507ab79942",
          "message": "Quote Fran's Shift in `docs/guide/approach.md` (#829)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-16T19:13:22-04:00",
          "tree_id": "6bcd263521f309f27bd1b16a0bcbf8a47c4a60a3",
          "url": "https://github.com/sourcemeta/one/commit/118b39b0ab062e6ff95d117d0f4810507ab79942"
        },
        "date": 1776382182330,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
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
            "value": 858,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
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
            "value": 774,
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
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 286,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 139,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1031,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12980,
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
          "id": "7bdf6001992c3f0d1106c291a06e261b386b331b",
          "message": "LICENSE clarification\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-17T10:15:42-04:00",
          "tree_id": "8972814345b27bc32e7111bda52278f3db389932",
          "url": "https://github.com/sourcemeta/one/commit/7bdf6001992c3f0d1106c291a06e261b386b331b"
        },
        "date": 1776436352337,
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
            "value": 780,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 85,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 688,
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
            "value": 44,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 260,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 141,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1021,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13852,
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
          "id": "7bdf6001992c3f0d1106c291a06e261b386b331b",
          "message": "LICENSE clarification\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-17T14:15:42Z",
          "url": "https://github.com/sourcemeta/one/commit/7bdf6001992c3f0d1106c291a06e261b386b331b"
        },
        "date": 1776455409670,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 872,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 660,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 8,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 294,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 130,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 969,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14038,
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
          "id": "7bdf6001992c3f0d1106c291a06e261b386b331b",
          "message": "LICENSE clarification\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-17T14:15:42Z",
          "url": "https://github.com/sourcemeta/one/commit/7bdf6001992c3f0d1106c291a06e261b386b331b"
        },
        "date": 1776714650709,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 657,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 657,
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
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 250,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 113,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 982,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12929,
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
          "id": "4527eeb88b928c3872315466ad8df12b4de4ce52",
          "message": "Add more quotes to the guide (#830)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-20T16:48:15-04:00",
          "tree_id": "174c4a3d545662ebe40417e3866eddd1ead68610",
          "url": "https://github.com/sourcemeta/one/commit/4527eeb88b928c3872315466ad8df12b4de4ce52"
        },
        "date": 1776719088346,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 21,
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
            "value": 967,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 642,
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
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 242,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 165,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1205,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14218,
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
          "id": "05cbdeeb5fb16cf1b3d196d989cfb81d9f529504",
          "message": "Better document custom linter rules (#831)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-21T12:44:07-04:00",
          "tree_id": "0806ba513223ffbecae15201d9c7e937fdf99b5e",
          "url": "https://github.com/sourcemeta/one/commit/05cbdeeb5fb16cf1b3d196d989cfb81d9f529504"
        },
        "date": 1776790870336,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 980,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
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
            "value": 710,
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
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 265,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 136,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1052,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13626,
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
          "id": "05cbdeeb5fb16cf1b3d196d989cfb81d9f529504",
          "message": "Better document custom linter rules (#831)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-21T16:44:07Z",
          "url": "https://github.com/sourcemeta/one/commit/05cbdeeb5fb16cf1b3d196d989cfb81d9f529504"
        },
        "date": 1776801130741,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 833,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 910,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 260,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 145,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1008,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12766,
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
          "id": "c371a198ab10e1684adb87dd50e36b52def31604",
          "message": "Upgrade all Sourcemeta dependencies (#832)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-22T10:34:19-04:00",
          "tree_id": "949b3b7641abd5d44fdd1b8268e35a5aef83fe83",
          "url": "https://github.com/sourcemeta/one/commit/c371a198ab10e1684adb87dd50e36b52def31604"
        },
        "date": 1776869463453,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 72,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 858,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 623,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 8,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 11,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 251,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 141,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 999,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14100,
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
          "id": "68bf528bd92de0280ad980c81e525a797c548df7",
          "message": "Fix style of alerts given Markdown generation (#834)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-22T13:08:05-04:00",
          "tree_id": "c1f87dc8dd2fa6db01bc72b0510cc9a398dccaf0",
          "url": "https://github.com/sourcemeta/one/commit/68bf528bd92de0280ad980c81e525a797c548df7"
        },
        "date": 1776878815337,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 842,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 705,
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
            "value": 31,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 271,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 132,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1013,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13457,
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
          "id": "68bf528bd92de0280ad980c81e525a797c548df7",
          "message": "Fix style of alerts given Markdown generation (#834)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-22T17:08:05Z",
          "url": "https://github.com/sourcemeta/one/commit/68bf528bd92de0280ad980c81e525a797c548df7"
        },
        "date": 1776887688656,
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
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 757,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 653,
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
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 263,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 135,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1203,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13480,
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
          "id": "d1d723c739c980a744b51ac2fc760bbe3ef44eb5",
          "message": "Implement a `Configuration::is_collection_base` helper (#836)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-23T14:40:52-04:00",
          "tree_id": "4714d5d70121d8e7fa35387385de089e1a68387d",
          "url": "https://github.com/sourcemeta/one/commit/d1d723c739c980a744b51ac2fc760bbe3ef44eb5"
        },
        "date": 1776971068350,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 673,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
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
            "value": 746,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 268,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 158,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1078,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13313,
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
          "id": "d1d723c739c980a744b51ac2fc760bbe3ef44eb5",
          "message": "Implement a `Configuration::is_collection_base` helper (#836)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-23T18:40:52Z",
          "url": "https://github.com/sourcemeta/one/commit/d1d723c739c980a744b51ac2fc760bbe3ef44eb5"
        },
        "date": 1776974046760,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 39,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 64,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 717,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 157,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 759,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 7,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 27,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 221,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 2236,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 41979,
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
          "id": "d78e942919453eb29ed2f37d75b76c3b132249fe",
          "message": "Misc improvements to `metapack_read_text` (#838)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-23T16:04:51-04:00",
          "tree_id": "0341547c83a1b6362c6a82e1d3014e34312fe631",
          "url": "https://github.com/sourcemeta/one/commit/d78e942919453eb29ed2f37d75b76c3b132249fe"
        },
        "date": 1776975636275,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 60,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 552,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 60,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 499,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 7,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 8,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 155,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 104,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 788,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 10721,
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
          "id": "87e23dd7c59dd990f8359167008c19d32379de34",
          "message": "Summarise the stateless design in the front page of the docs (#840)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-23T16:32:18-04:00",
          "tree_id": "ba6281394aab733c3a4c8b849f20abaa979f19f7",
          "url": "https://github.com/sourcemeta/one/commit/87e23dd7c59dd990f8359167008c19d32379de34"
        },
        "date": 1776977440354,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 73,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 686,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 674,
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
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 315,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 107,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 866,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13700,
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
          "id": "597b15a54b206e3a6d31b72ea87046e0dad43805",
          "message": "Cite \"Crafting Great APIs with Domain-Driven Design\" book in the guide (#842)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-23T17:10:19-04:00",
          "tree_id": "a5d2ad3e3392612f2827ebbe03d88e8ac2d2c4a1",
          "url": "https://github.com/sourcemeta/one/commit/597b15a54b206e3a6d31b72ea87046e0dad43805"
        },
        "date": 1776979669353,
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
            "value": 75,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 723,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 74,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 636,
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
            "value": 30,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 243,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 135,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1258,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14585,
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
          "id": "3a6b34f29a093305c999faecff0c135ea1372631",
          "message": "Remove `@sourcemeta/std/v0` as a `extends` (use a git submodule instead) (#843)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-24T09:58:57-04:00",
          "tree_id": "915365136534c60b3eb0597cc976adf246a83b46",
          "url": "https://github.com/sourcemeta/one/commit/3a6b34f29a093305c999faecff0c135ea1372631"
        },
        "date": 1777040205318,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
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
            "value": 687,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
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
            "value": 807,
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
            "value": 253,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 130,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1034,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13525,
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
          "id": "43e3f828e618459db331d674b50f8e9407dc3a68",
          "message": "Pull the JSON Schema CLI using NPM (#844)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-24T10:19:59-04:00",
          "tree_id": "3940cf5e26ada994afcc01a71173abc44a345994",
          "url": "https://github.com/sourcemeta/one/commit/43e3f828e618459db331d674b50f8e9407dc3a68"
        },
        "date": 1777041254341,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 18,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 23,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 80,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 782,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 16,
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
            "value": 685,
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
            "value": 32,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 258,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 121,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1030,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13335,
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
          "id": "6ef06669140acfc2aee1bbe73103cf3c8f1057df",
          "message": "Upgrade Core to `f1405788195cb51a4ce3bef043e8275fa04afd03` (#845)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-24T11:38:39-04:00",
          "tree_id": "82b886f8263e30dda56a2d177674755327466293",
          "url": "https://github.com/sourcemeta/one/commit/6ef06669140acfc2aee1bbe73103cf3c8f1057df"
        },
        "date": 1777045932290,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 15,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 62,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 775,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 63,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 599,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 8,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 9,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 176,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 95,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 844,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 11321,
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
          "id": "c45278a9fe8a6cc6029a259075b3d855aa0305f0",
          "message": "Totally decouple `self/v1` from the standard library (#846)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-24T12:56:17-04:00",
          "tree_id": "99e8baeb51b4e8d435267110171ac5781c25fe5b",
          "url": "https://github.com/sourcemeta/one/commit/c45278a9fe8a6cc6029a259075b3d855aa0305f0"
        },
        "date": 1777050643324,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 13,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 68,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 977,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 71,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 705,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 2,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 5,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 261,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 118,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 940,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13282,
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
          "id": "16c9671142cb72b02480f2327f56cc1c87e97867",
          "message": "Link to the guide in the documentation front page (#847)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-24T14:03:24-04:00",
          "tree_id": "3c9425177c77aa28f0d02b340a2966f049c9bb8d",
          "url": "https://github.com/sourcemeta/one/commit/16c9671142cb72b02480f2327f56cc1c87e97867"
        },
        "date": 1777054655328,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 69,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 839,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 69,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 829,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 2,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 4,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 260,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 133,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 942,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13002,
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
          "id": "bc49f6031b0a97fbbb877141a56c098993c1d125",
          "message": "Explicitly extend `@self/v1` in all indexing CLI tests (#848)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-24T14:57:43-04:00",
          "tree_id": "8154960108674b74e48e887db2d10e5c46269d0d",
          "url": "https://github.com/sourcemeta/one/commit/bc49f6031b0a97fbbb877141a56c098993c1d125"
        },
        "date": 1777058167335,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 66,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 839,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 66,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 807,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 2,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 4,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 246,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 134,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 951,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13255,
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
          "id": "bc49f6031b0a97fbbb877141a56c098993c1d125",
          "message": "Explicitly extend `@self/v1` in all indexing CLI tests (#848)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-24T18:57:43Z",
          "url": "https://github.com/sourcemeta/one/commit/bc49f6031b0a97fbbb877141a56c098993c1d125"
        },
        "date": 1777059682850,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 12,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 67,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 741,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 19,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 69,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 646,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 2,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 4,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 24,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 240,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 127,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 977,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 14536,
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
          "id": "6556947c8cdf38380d689f4406a4b0e5bffa449f",
          "message": "Always assume and enforce the presence of `/self` (#849)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-26T13:28:53-04:00",
          "tree_id": "205d57099a9a144d0d6ff39682fadf58140be44c",
          "url": "https://github.com/sourcemeta/one/commit/6556947c8cdf38380d689f4406a4b0e5bffa449f"
        },
        "date": 1777225394336,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 132,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 603,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 20,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 814,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 4,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 6,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 25,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 239,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 103,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 871,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 12911,
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
          "id": "a46161bd046e3da2b5f72a940656f80ea917baac",
          "message": "Introduce a new `api` configuration property (#850)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-26T16:38:08-04:00",
          "tree_id": "4f4067324007146f86ea53089698b1a2822deef8",
          "url": "https://github.com/sourcemeta/one/commit/a46161bd046e3da2b5f72a940656f80ea917baac"
        },
        "date": 1777236814325,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 126,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 21,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 77,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 834,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 14,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 22,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 78,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 686,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 4,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 7,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 29,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 271,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 129,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 1038,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 13342,
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
          "id": "961280ca6d4e907e05f4a14b460fff9322564d47",
          "message": "Only allow `extends` at the top level of `one.json` (#851)\n\nThis was a mistake on our end. `extends` was supposed to be only at the\ntop level. The docs had it right.\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2026-04-26T17:28:59-04:00",
          "tree_id": "ed58097cb3dc47c7c21f961cda092577a901a0a4",
          "url": "https://github.com/sourcemeta/one/commit/961280ca6d4e907e05f4a14b460fff9322564d47"
        },
        "date": 1777239807264,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "Add one schema (0 existing)",
            "value": 146,
            "unit": "ms"
          },
          {
            "name": "Add one schema (100 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Add one schema (1000 existing)",
            "value": 57,
            "unit": "ms"
          },
          {
            "name": "Add one schema (10000 existing)",
            "value": 785,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1 existing)",
            "value": 10,
            "unit": "ms"
          },
          {
            "name": "Update one schema (101 existing)",
            "value": 16,
            "unit": "ms"
          },
          {
            "name": "Update one schema (1001 existing)",
            "value": 57,
            "unit": "ms"
          },
          {
            "name": "Update one schema (10001 existing)",
            "value": 694,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1 existing)",
            "value": 3,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (101 existing)",
            "value": 4,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (1001 existing)",
            "value": 17,
            "unit": "ms"
          },
          {
            "name": "Cached rebuild (10001 existing)",
            "value": 159,
            "unit": "ms"
          },
          {
            "name": "Index 100 schemas",
            "value": 107,
            "unit": "ms"
          },
          {
            "name": "Index 1000 schemas",
            "value": 797,
            "unit": "ms"
          },
          {
            "name": "Index 10000 schemas",
            "value": 10486,
            "unit": "ms"
          }
        ]
      }
    ]
  }
}