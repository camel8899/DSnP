/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <list>
using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"
#include "cirGate.h"
extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr() {}
 ~CirMgr()
	{
		_PIList.clear();
		_POList.clear();
		for(GateList::iterator it = _netList.begin(); it != _netList.end(); ++it)
		{
			if(*it != 0)
				delete *it;
		}	
		_netList.clear();
    } 
   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const 
	{
		if(_netList[gid])
			return _netList[gid]; 
		else
		{
			return 0;
		}
	}

   // Member functions about circuit construction
   bool readCircuit(const string&);
   CirGate* addUndef(unsigned gid);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }
   void DFS_simvalue_gen(bool);
   void Collect_groups(HashMap<Simkey,FECGroup>& , CirGate*);
   void write_sim(size_t);
	// Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();
   void assignFEC();
   void genProofModel(SatSolver&);
   static bool XorProve(SatSolver&, CirGate*, CirGate*, bool);
   void fraig_const(SatSolver&, FECGroup* );
   void fraig_done(SatSolver&, FECGroup* );
   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() ;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;
   void dodfs(GateList* current);
private:
   GateList _PIList;
   GateList _POList;
   GateList _netList;
   GateList _dfsList;
   unsigned MILOA[5];
   ofstream           *_simLog;
   list<FECGroup> _fecGrps;
};

#endif // CIR_MGR_H
