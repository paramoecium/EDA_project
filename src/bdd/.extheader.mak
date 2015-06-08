bdd.d: ../../include/bddMgr.h ../../include/bddNode.h 
../../include/bddMgr.h: bddMgr.h
	@rm -f ../../include/bddMgr.h
	@ln -fs ../src/bdd/bddMgr.h ../../include/bddMgr.h
../../include/bddNode.h: bddNode.h
	@rm -f ../../include/bddNode.h
	@ln -fs ../src/bdd/bddNode.h ../../include/bddNode.h
