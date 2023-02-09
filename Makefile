# Makefile wrapper for waf

TOPO=ping

all:
	./ns-3.35/waf -t ns-3.35

run:
	./ns-3.35/waf -t ns-3.35 --run "flexcomm --topo=$(TOPO)" 

run_debug:
	./ns-3.35/waf -t ns-3.35 --run "flexcomm --topo=$(TOPO)" --gdb 

optimize:
	./ns-3.35/waf -t ns-3.35 -d optimized configure --disable-python --disable-gtk 

debug:
	./ns-3.35/waf -t ns-3.35 configure --disable-python --disable-gtk 

format:
	./formatter.sh ns-3.35
