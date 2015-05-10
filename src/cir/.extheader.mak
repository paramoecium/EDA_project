cir.d: ../../include/cirMgr.h 
../../include/cirMgr.h: cirMgr.h
	@rm -f ../../include/cirMgr.h
	@ln -fs ../src/cir/cirMgr.h ../../include/cirMgr.h
