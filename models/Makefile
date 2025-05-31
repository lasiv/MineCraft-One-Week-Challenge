.PHONY: all design domain clean

all: design domain

design: scripts/out/design.txt

scripts/out/design.txt:
	mkdir -p scripts/out
	python3 scripts/uxf2tikz.py design/design.uxf scripts/out/design.txt

domain: scripts/out/domain.txt

scripts/out/domain.txt:
	mkdir -p scripts/out
	python3 scripts/uxf2tikz.py domain/domain.uxf scripts/out/domain.txt

clean:
	rm -rf scripts/out/*.txt
