# Copyright (c) 2026 ItsDropper
# Licensed under the MIT License.
.PHONY: all test clean
all:
	mkdir -p build
	../vnt --compile interpreter.vnt -o build/vnx
test: all
	printf 'tests/smoke.vnx\n' | build/vnx
clean:
	rm -rf build
