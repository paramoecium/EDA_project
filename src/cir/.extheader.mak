cir.d: ../../include/cirDef.h ../../include/cirMgr.h 
../../include/cirDef.h: cirDef.h
	@rm -f ../../include/cirDef.h
	@ln -fs ../src/cir/cirDef.h ../../include/cirDef.h
../../include/cirMgr.h: cirMgr.h
	@rm -f ../../include/cirMgr.h
	@ln -fs ../src/cir/cirMgr.h ../../include/cirMgr.h
