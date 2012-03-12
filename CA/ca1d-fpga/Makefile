all: main

main: main-cells.vv main-subseq.vv

%: %.v
	iverilog $< -o $@

# This will fail if we reference a non-existant
# file here. Remove dependences if you get mad.

%: gen/%.py main.v Makefile gen/VeriGen.py
	python $< > $@

.PHONY: clean
clean:
	rm -rf  main *.vv gen/VeriGen.pyc
