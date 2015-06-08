cirCEC.o: cirCEC.cpp cirMgr.h cirDef.h cirGate.h ../../include/sat.h \
 ../../include/Solver.h ../../include/SolverTypes.h \
 ../../include/Global.h ../../include/VarOrder.h ../../include/Heap.h \
 ../../include/Proof.h ../../include/File.h ../../include/myHashMap.h \
 ../../include/util.h ../../include/rnGen.h ../../include/myUsage.h
cirGate.o: cirGate.cpp cirGate.h cirDef.h ../../include/sat.h \
 ../../include/Solver.h ../../include/SolverTypes.h \
 ../../include/Global.h ../../include/VarOrder.h ../../include/Heap.h \
 ../../include/Proof.h ../../include/File.h cirMgr.h
cirMgr.o: cirMgr.cpp cirMgr.h cirDef.h cirGate.h ../../include/sat.h \
 ../../include/Solver.h ../../include/SolverTypes.h \
 ../../include/Global.h ../../include/VarOrder.h ../../include/Heap.h \
 ../../include/Proof.h ../../include/File.h ../../include/util.h \
 ../../include/rnGen.h ../../include/myUsage.h
cirSim.o: cirSim.cpp cirMgr.h cirDef.h cirGate.h ../../include/sat.h \
 ../../include/Solver.h ../../include/SolverTypes.h \
 ../../include/Global.h ../../include/VarOrder.h ../../include/Heap.h \
 ../../include/Proof.h ../../include/File.h ../../include/util.h \
 ../../include/rnGen.h ../../include/myUsage.h ../../include/myHashMap.h
