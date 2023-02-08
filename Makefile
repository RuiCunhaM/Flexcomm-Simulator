# Makefile wrapper for waf

EXE=ofswitch13-first

all:
	./ns-3.35/waf -t ns-3.35

run:
	./ns-3.35/waf -t ns-3.35 --run $(EXE) 

optimize:
	./ns-3.35/waf -t ns-3.35 -d optimized configure --disable-python --disable-gtk 

debug:
	./ns-3.35/waf -t ns-3.35 configure --disable-python --disable-gtk 
