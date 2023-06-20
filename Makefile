# Makefile wrapper for waf

MODULES=applications,core,csma,ecofen,energy-api,flow-monitor,internet,internet-apps,link-stats,ofswitch13,parser,point-to-point-ethernet,switch-stats,topology,tap-bridge

TOPO=example
CONTROLLER=ns3::SimpleController
CHECKSUM=false
OUTPUTS=outputs
ESTIFILE=estimate.json
FLEXFILE=flex.json
NRANKS=2
OPTIONS=--topo=$(TOPO) --ctrl=$(CONTROLLER) --checksum=$(CHECKSUM) --estifile=$(ESTIFILE) --flexfile=$(FLEXFILE)
CXXFLAGS="-Wall"

all:
	./ns-3.35/waf -t ns-3.35

run: create_outpu_folder
	./ns-3.35/waf -t ns-3.35/ --run flexcomm --command-template="mpiexec -np $(NRANKS) --mca opal_warn_on_missing_libcuda 0 --use-hwthread-cpus %s $(OPTIONS)"  --cwd=../$(OUTPUTS)

optimize:
	CXXFLAGS=$(CXXFLAGS) ./ns-3.35/waf -t ns-3.35 -d optimized configure --disable-python --disable-gtk --enable-modules=$(MODULES) --enable-mpi

format:
	./formatter.sh ns-3.35

create_outpu_folder:
	mkdir -p $(OUTPUTS)/$(TOPO)
