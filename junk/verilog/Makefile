all: count2  count  sum2  sum ac elementary-ac

# use cver

ac: ac-cells.vv

%: %.v
	iverilog $< -o $@

%: %.py
	cp $< $@

.PHONY: clean
clean:
	rm -rf count2  count  sum2  sum ac elementary-ac
