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
SANDBOX ?= ./test/sandbox
SANDBOX_CONFIGURATION ?= html
SANDBOX_PORT ?= 8000
SANDBOX_URL ?= http://localhost:$(SANDBOX_PORT)
PUBLIC ?= ./public
PARALLEL ?= 4
# Only for local development
ENTERPRISE ?= ON
DOCKERFILE = $(if $(filter ON,$(ENTERPRISE)),enterprise/Dockerfile,Dockerfile)
EDITION = $(if $(filter ON,$(ENTERPRISE)),enterprise,community)

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

.PHONY: test-e2e
HURL_TESTS += test/e2e/$(SANDBOX_CONFIGURATION)/*.hurl
ifneq ($(SANDBOX_CONFIGURATION),empty)
HURL_TESTS += test/e2e/populated/schemas/*.hurl
HURL_TESTS += test/e2e/populated/api/common/*.hurl
ifeq ($(ENTERPRISE),ON)
HURL_TESTS += test/e2e/populated/api/enterprise/*.hurl
endif
endif
test-e2e:
	$(HURL) --test \
		--variable base=$(SANDBOX_URL) \
		--variable edition=$(if $(filter ON,$(ENTERPRISE)),Enterprise,Community) \
		$(HURL_TESTS)

.PHONY: test-ui
test-ui: node_modules
	$(NPX) playwright install --with-deps
	env PLAYWRIGHT_BASE_URL=$(SANDBOX_URL) \
		$(NPX) playwright test --config test/ui/playwright.config.js

.PHONY: sandbox-index
sandbox-index: compile
	$(PREFIX)/bin/sourcemeta-one-index \
		$(SANDBOX)/one-$(SANDBOX_CONFIGURATION)-$(EDITION).json \
		$(OUTPUT)/sandbox --url $(SANDBOX_URL) --configuration
	$(PREFIX)/bin/sourcemeta-one-index \
		$(SANDBOX)/one-$(SANDBOX_CONFIGURATION)-$(EDITION).json \
		$(OUTPUT)/sandbox --url $(SANDBOX_URL) --profile
	./test/sandbox/manifest-check.sh $(OUTPUT)/sandbox \
		$(SANDBOX)/manifest-$(SANDBOX_CONFIGURATION)-$(EDITION).txt

.PHONY: sandbox
sandbox: sandbox-index
	$(PREFIX)/bin/sourcemeta-one-server \
		$(OUTPUT)/sandbox $(SANDBOX_PORT)

.PHONY: sandbox-manifest-refresh
sandbox-manifest-refresh: configure compile
	$(CMAKE) -E rm -R -f build/sandbox && $(MAKE) configure compile sandbox-index ENTERPRISE=ON SANDBOX_CONFIGURATION=empty || true
	$(CMAKE) -E rm -R -f build/sandbox && $(MAKE) configure compile sandbox-index ENTERPRISE=ON SANDBOX_CONFIGURATION=headless || true
	$(CMAKE) -E rm -R -f build/sandbox && $(MAKE) configure compile sandbox-index ENTERPRISE=ON SANDBOX_CONFIGURATION=html || true
	$(CMAKE) -E rm -R -f build/sandbox && $(MAKE) configure compile sandbox-index ENTERPRISE=OFF SANDBOX_CONFIGURATION=empty || true
	$(CMAKE) -E rm -R -f build/sandbox && $(MAKE) configure compile sandbox-index ENTERPRISE=OFF SANDBOX_CONFIGURATION=headless || true
	$(CMAKE) -E rm -R -f build/sandbox && $(MAKE) configure compile sandbox-index ENTERPRISE=OFF SANDBOX_CONFIGURATION=html || true

.PHONY: docker
docker: $(DOCKERFILE)
	$(DOCKER) build --tag one . --file $< --progress plain \
		--build-arg SOURCEMETA_ONE_BUILD_TYPE=$(PRESET) \
		--build-arg SOURCEMETA_ONE_PARALLEL=$(PARALLEL)

.PHONY: docker-sandbox-build
docker-sandbox-build: test/sandbox/compose.yaml
	SOURCEMETA_ONE_SANDBOX_CONFIGURATION=$(SANDBOX_CONFIGURATION) \
	SOURCEMETA_ONE_EDITION=$(EDITION) \
	SOURCEMETA_ONE_SANDBOX_PORT=$(SANDBOX_PORT) \
		$(DOCKER) compose --file $< config
	BUILDX_NO_DEFAULT_ATTESTATIONS=1 \
	SOURCEMETA_ONE_SANDBOX_CONFIGURATION=$(SANDBOX_CONFIGURATION) \
	SOURCEMETA_ONE_EDITION=$(EDITION) \
	SOURCEMETA_ONE_SANDBOX_PORT=$(SANDBOX_PORT) \
		$(DOCKER) compose --progress plain --file $< build

.PHONY: docker-sandbox-up
docker-sandbox-up: test/sandbox/compose.yaml
	SOURCEMETA_ONE_SANDBOX_CONFIGURATION=$(SANDBOX_CONFIGURATION) \
	SOURCEMETA_ONE_EDITION=$(EDITION) \
	SOURCEMETA_ONE_SANDBOX_PORT=$(SANDBOX_PORT) \
		$(DOCKER) compose --progress plain --file $< up --detach --wait

.PHONY: docker-sandbox-down
docker-sandbox-down: test/sandbox/compose.yaml
	SOURCEMETA_ONE_SANDBOX_CONFIGURATION=$(SANDBOX_CONFIGURATION) \
	SOURCEMETA_ONE_EDITION=$(EDITION) \
	SOURCEMETA_ONE_SANDBOX_PORT=$(SANDBOX_PORT) \
		$(DOCKER) compose --progress plain --file $< down

.PHONY: docker-sandbox
docker-sandbox: test/sandbox/compose.yaml
	SOURCEMETA_ONE_SANDBOX_CONFIGURATION=$(SANDBOX_CONFIGURATION) \
	SOURCEMETA_ONE_EDITION=$(EDITION) \
	SOURCEMETA_ONE_SANDBOX_PORT=$(SANDBOX_PORT) \
		$(DOCKER) compose --file $< config
	SOURCEMETA_ONE_SANDBOX_CONFIGURATION=$(SANDBOX_CONFIGURATION) \
	SOURCEMETA_ONE_EDITION=$(EDITION) \
	SOURCEMETA_ONE_SANDBOX_PORT=$(SANDBOX_PORT) \
		$(DOCKER) compose --progress plain --file $< up --build

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
