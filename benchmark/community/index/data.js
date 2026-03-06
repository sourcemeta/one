window.BENCHMARK_DATA = {
  "lastUpdate": 1772813406279,
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
      }
    ]
  }
}