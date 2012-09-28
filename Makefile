SPECS = spec/*.js
REPORTER = spec

all: build
clean:
	./node_modules/.bin/node-gyp clean

configure: binding.gyp
	./node_modules/.bin/node-gyp configure

build: configure
	./node_modules/.bin/node-gyp build

test: build
	@NODE_ENV=$(NODE_ENV) ./node_modules/.bin/mocha \
		--require should \
		--reporter $(REPORTER) \
		--ui bdd \
		$(SPECS)

install:
	npm install

.PHONY: build test install
