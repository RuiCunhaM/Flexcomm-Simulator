# Makefile wrapper for waf

MODULES=applications,core,csma,ecofen,energy-api,flow-monitor,internet,internet-apps,link-stats,ofswitch13,parser,point-to-point-ethernet,switch-stats,topology,tap-bridge

TOPO=example
CONTROLLER=ns3::SimpleController
CHECKSUM=false
OUTPUTS=outputs
ESTIFILE=estimate.json
FLEXFILE=flex.json
LINKFAILURES=NONE
OPTIONS="flexcomm --topo=$(TOPO) --ctrl=$(CONTROLLER) --checksum=$(CHECKSUM) --estifile=$(ESTIFILE) --flexfile=$(FLEXFILE) --linkfailures=$(LINKFAILURES)" --cwd=../$(OUTPUTS)
CXXFLAGS="-Wall"

.PHONY: all
all:
	./ns-3.35/waf -t ns-3.35

.PHONY: run
run: create_outpu_folder
	./ns-3.35/waf -t ns-3.35 --run $(OPTIONS) 

.PHONY: perf
perf: create_outpu_folder
	perf record ./ns-3.35/waf -t ns-3.35 --run $(OPTIONS) 

.PHONY: run_debug
run_debug: create_outpu_folder
	./ns-3.35/waf -t ns-3.35 --run $(OPTIONS) --gdb 

.PHONY: configure
configure: optimize

.PHONY: optimize
optimize:
	CXXFLAGS=$(CXXFLAGS) ./ns-3.35/waf -t ns-3.35 -d optimized configure --disable-python --disable-gtk --enable-modules=$(MODULES)

.PHONY: debug
debug:
	CXXFLAGS=$(CXXFLAGS) ./ns-3.35/waf -t ns-3.35 configure --disable-python --disable-gtk --enable-modules=$(MODULES)

.PHONY: format
format:
	./formatter.sh ns-3.35

.PHONY: create_outpu_folder
create_outpu_folder:
	mkdir -p $(OUTPUTS)/$(TOPO)

.PHONY: clean
clean:
	rm -rf $(OUTPUTS) perf.data
