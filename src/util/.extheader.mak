util.d: ../../include/util.h ../../include/rnGen.h ../../include/myUsage.h ../../include/myHashMap.h ../../include/myHash.h ../../include/myHashSet.h 
../../include/util.h: util.h
	@rm -f ../../include/util.h
	@ln -fs ../src/util/util.h ../../include/util.h
../../include/rnGen.h: rnGen.h
	@rm -f ../../include/rnGen.h
	@ln -fs ../src/util/rnGen.h ../../include/rnGen.h
../../include/myUsage.h: myUsage.h
	@rm -f ../../include/myUsage.h
	@ln -fs ../src/util/myUsage.h ../../include/myUsage.h
../../include/myHashMap.h: myHashMap.h
	@rm -f ../../include/myHashMap.h
	@ln -fs ../src/util/myHashMap.h ../../include/myHashMap.h
../../include/myHash.h: myHash.h
	@rm -f ../../include/myHash.h
	@ln -fs ../src/util/myHash.h ../../include/myHash.h
../../include/myHashSet.h: myHashSet.h
	@rm -f ../../include/myHashSet.h
	@ln -fs ../src/util/myHashSet.h ../../include/myHashSet.h
