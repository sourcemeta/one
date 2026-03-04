# Programs
CMAKE ?= cmake
CTEST ?= ctest
HURL ?= hurl
DOCKER ?= docker
SHELLCHECK ?= shellcheck
MKDOCS ?= mkdocs
NPM ?= npm
NPX ?= npx
NODE ?= node

# Options
INDEX ?= ON
SERVER ?= ON
PRESET ?= Debug
OUTPUT ?= ./build
PREFIX ?= $(OUTPUT)/dist
PUBLIC ?= ./public
PARALLEL ?= 4
# Only for local development
ENTERPRISE ?= ON
DOCKERFILE = $(if $(filter ON,$(ENTERPRISE)),enterprise/Dockerfile,Dockerfile)
EDITION = $(if $(filter ON,$(ENTERPRISE)),enterprise,community)
EDITION_DISPLAY = $(if $(filter ON,$(ENTERPRISE)),Enterprise,Community)

ifeq ($(ENTERPRISE),ON)
SANDBOX ?= test/e2e/enterprise
else
SANDBOX ?= test/e2e/html
endif
SANDBOX_PORT ?= 8000

.PHONY: all
all: configure compile test

node_modules: package.json package-lock.json
	$(NPM) ci

.PHONY: configure
configure: node_modules
	$(CMAKE) -S . -B $(OUTPUT) \
		-DCMAKE_BUILD_TYPE:STRING=$(PRESET) \
		-DCMAKE_COMPILE_WARNING_AS_ERROR:BOOL=ON \
		-DONE_TESTS:BOOL=ON \
		-DONE_INDEX:BOOL=$(INDEX) \
		-DONE_SERVER:BOOL=$(SERVER) \
		-DONE_ENTERPRISE:BOOL=$(ENTERPRISE) \
		-DONE_PREFIX:STRING=$(or $(realpath $(PREFIX)),$(abspath $(PREFIX))) \
		-DBUILD_SHARED_LIBS:BOOL=OFF

.PHONY: compile
compile:
	$(CMAKE) --build $(OUTPUT) --config $(PRESET) --target clang_format
	$(CMAKE) --build $(OUTPUT) --config $(PRESET) --parallel $(PARALLEL)
	$(CMAKE) --install $(OUTPUT) --prefix $(PREFIX) --config $(PRESET) --verbose \
		--component sourcemeta_one --component sourcemeta_one
	$(CMAKE) --install $(OUTPUT) --prefix $(PREFIX) --config $(PRESET) --verbose \
		--component sourcemeta_one --component sourcemeta_jsonschema

.PHONY: lint
lint:
	$(CMAKE) --build $(OUTPUT) --config $(PRESET) --target clang_format_test
	$(CMAKE) --build $(OUTPUT) --config $(PRESET) --target shellcheck
	$(CMAKE) --build $(OUTPUT) --config $(PRESET) --target jsonschema_metaschema
	$(CMAKE) --build $(OUTPUT) --config $(PRESET) --target jsonschema_lint

.PHONY: test
test:
	$(CTEST) --test-dir $(OUTPUT) --build-config $(PRESET) --output-on-failure --parallel

.PHONY: docker-build
docker-build: $(DOCKERFILE)
	$(DOCKER) build --tag one . --file $< --progress plain \
		--build-arg SOURCEMETA_ONE_BUILD_TYPE=$(PRESET) \
		--build-arg SOURCEMETA_ONE_PARALLEL=$(PARALLEL)

# Useful to run the entire main suite in a single command
.PHONY: docker
docker: docker-build
	$(MAKE) -C test/e2e/empty EDITION=$(EDITION)
	$(MAKE) -C test/e2e/headless EDITION=$(EDITION)
	$(MAKE) -C test/e2e/html EDITION=$(EDITION)
	$(MAKE) -C test/e2e/chaos EDITION=$(EDITION)
ifeq ($(ENTERPRISE),ON)
	$(MAKE) -C test/e2e/enterprise EDITION=$(EDITION)
endif

.PHONY: docker-benchmark
docker-benchmark: benchmark/Dockerfile
	$(DOCKER) build --tag one-benchmark . --file $< --progress plain

.PHONY: benchmark
benchmark:
	./benchmark/index.sh $(OUTPUT)/dist/bin/sourcemeta-one-index

.PHONY: sandbox-index
sandbox-index: compile
	$(PREFIX)/bin/sourcemeta-one-index \
		$(SANDBOX)/one.json $(OUTPUT)/sandbox --configuration
	$(PREFIX)/bin/sourcemeta-one-index \
		$(SANDBOX)/one.json $(OUTPUT)/sandbox --profile

.PHONY: sandbox
sandbox: sandbox-index
	$(PREFIX)/bin/sourcemeta-one-server \
		$(realpath $(OUTPUT)/sandbox) $(SANDBOX_PORT)

.PHONY: test-e2e
test-e2e:
	$(HURL) --test \
		--variable base=http://localhost:$(SANDBOX_PORT) \
		--variable edition=$(EDITION_DISPLAY) \
		$(SANDBOX)/hurl/*.hurl

.PHONY: test-ui
test-ui: node_modules
	$(NPX) playwright install --with-deps
	env PLAYWRIGHT_BASE_URL=http://localhost:$(SANDBOX_PORT) \
		$(NPX) playwright test --config $(SANDBOX)/playwright/playwright.config.js

.PHONY: docs
docs: mkdocs.yml
	$(MKDOCS) serve --config-file $< --strict --open

.PHONY: public
public:
	$(PREFIX)/bin/sourcemeta-one-index $(PUBLIC)/one.json $(OUTPUT)/public --configuration
	$(PREFIX)/bin/sourcemeta-one-index $(PUBLIC)/one.json $(OUTPUT)/public --verbose
	$(PREFIX)/bin/sourcemeta-one-server $(OUTPUT)/public 8000

.PHONY: clean
clean:
	$(CMAKE) -E rm -R -f build
	$(DOCKER) system prune --force --all --volumes || true
