DOCKER ?= docker
HURL ?= hurl
NPM ?= npm
NPX ?= npx
ROOT := $(dir $(lastword $(MAKEFILE_LIST)))../..

COMPOSE = compose.yml
BASE ?= http://localhost
PORT ?= 8000
EDITION ?= enterprise

export PORT
export EDITION

EDITION_DISPLAY = $(if $(filter enterprise,$(EDITION)),Enterprise,Community)

.PHONY: all
all:
	$(MAKE) down
	$(MAKE) up
	$(MAKE) test-hurl
	$(MAKE) test-playwright
	$(MAKE) down

.PHONY: up
up: $(COMPOSE)
	$(DOCKER) compose --progress plain --file $< build
	$(DOCKER) compose --progress plain --file $< up --detach --wait

.PHONY: test-hurl
test-hurl:
	$(HURL) --test --variable base=$(BASE):$(PORT) \
		--variable edition=$(EDITION_DISPLAY) hurl/*.hurl

.PHONY: test-playwright
test-playwright:
ifneq ($(wildcard playwright/),)
	$(MAKE) -C $(ROOT) node_modules
	$(NPX) playwright install --with-deps
	env PLAYWRIGHT_BASE_URL=$(BASE):$(PORT) \
		$(NPX) playwright test --config playwright/playwright.config.js
endif

.PHONY: down
down: $(COMPOSE)
	$(DOCKER) compose --progress plain --file $< down --volumes

.PHONY: dev
dev: $(COMPOSE)
	$(DOCKER) compose --progress plain --file $< up --build
