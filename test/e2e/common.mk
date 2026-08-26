DOCKER ?= docker
HURL ?= hurl
NPM ?= npm
NPX ?= npx
PYTHON ?= python3
ROOT := $(dir $(lastword $(MAKEFILE_LIST)))../..
# Where this suite lives, which is what a measurement it takes is named after
SUITE := $(patsubst $(abspath $(ROOT))/%,%,$(CURDIR))
# Where a measurement this suite takes is left for whoever collects them
BENCHMARK_RESULT := $(abspath $(ROOT))/build/benchmark/$(subst /,-,$(SUITE)).json

COMPOSE = compose.yml
BASE ?= http://localhost
PORT ?= 8000
EDITION ?= community

export PORT
export EDITION

.PHONY: all
all:
	$(MAKE) down
	$(MAKE) up
	$(MAKE) test-benchmark test-hurl test-playwright; \
		status=$$?; $(MAKE) down; exit $$status

.PHONY: up
up: $(COMPOSE)
	$(DOCKER) compose --progress plain --file $< build
	$(DOCKER) compose --progress plain --file $< up --detach --wait --wait-timeout 120

# Note we run the tests multiple times to have a higher
# chance of catching any potential flakiness
.PHONY: test-hurl
test-hurl:
	$(HURL) $(HURL_FLAGS) --repeat 10 --test --variable base=$(BASE):$(PORT) --variable port=$(PORT) \
		$(wildcard hurl/*.all.hurl) \
		$(wildcard hurl/*.$(EDITION).hurl)

# A suite that carries a script saying what it wants measured is measured
# before its tests run, so what is timed is an instance warmed deliberately
# rather than one that has just served a whole test suite. What a run writes is
# named apart until it is complete, so a run that stops part way through cannot
# leave behind a partial answer that reads like a whole one
.PHONY: test-benchmark
test-benchmark:
ifneq ($(wildcard benchmark.py),)
	mkdir -p $(dir $(BENCHMARK_RESULT))
	PYTHONPATH=$(abspath $(ROOT))/benchmark \
		$(PYTHON) ./benchmark.py $(SUITE) $(BASE):$(PORT) \
		> $(BENCHMARK_RESULT).part
	mv $(BENCHMARK_RESULT).part $(BENCHMARK_RESULT)
endif

.PHONY: test-playwright
test-playwright:
ifneq ($(wildcard playwright/),)
	$(MAKE) -C $(ROOT) node_modules
	$(NPX) playwright install chromium
	@echo "Playwright dependencies installed"
	env PLAYWRIGHT_BASE_URL=$(BASE):$(PORT) \
		$(NPX) playwright test --config playwright/playwright.config.js
endif

.PHONY: down
down: $(COMPOSE)
	$(DOCKER) compose --progress plain --file $< down --volumes

.PHONY: dev
dev: $(COMPOSE)
	$(DOCKER) compose --progress plain --file $< up --build
