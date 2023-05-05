# Makefile wrapper for waf

TOPO=example
OUTPUTS=outputs
OPTIONS="flexcomm --topo=$(TOPO)" --cwd=../$(OUTPUTS)
CXXFLAGS="-Wall"

all:
	./ns-3.35/waf -t ns-3.35

run: create_outpu_folder
	./ns-3.35/waf -t ns-3.35 --run $(OPTIONS) 

run_debug: create_outpu_folder
	./ns-3.35/waf -t ns-3.35 --run $(OPTIONS) --gdb 

optimize:
	CXXFLAGS=$(CXXFLAGS) ./ns-3.35/waf -t ns-3.35 -d optimized configure --disable-python --disable-gtk 

debug:
	CXXFLAGS=$(CXXFLAGS) ./ns-3.35/waf -t ns-3.35 configure --disable-python --disable-gtk 

format:
	./formatter.sh ns-3.35

create_outpu_folder:
	mkdir -p $(OUTPUTS)/$(TOPO)
