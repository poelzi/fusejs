SPECS = spec/*.js 
REPORTER = spec

build: 
	./node_modules/.bin/node-gyp distclean configure build

test: build
	@NODE_ENV=$(NODE_ENV) ./node_modules/.bin/mocha \
		--require should \
		--reporter $(REPORTER) \
		--ui bdd \
		$(SPECS)

install:
	npm install

.PHONY: build test install
