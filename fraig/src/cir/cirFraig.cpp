/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
	HashMap<strashkey, CirGate*> strash(_dfsList.size());
	for(size_t i = 0; i < _dfsList.size(); i++)
	{
		CirGate* g = _dfsList[i];
		if(g->isAig())
		{
			strashkey k(g);
			CirGate* d = 0;
			if(strash.query(k,d))
			{
				d->getin(0)->deleteout(g);
				d->getin(1)->deleteout(g);
				for(size_t j = 0; j < g->_fanoutList.size(); j++)
				{
					d->addfanout(g->_fanoutList[j]);
					bool am_i_inv;
					for(size_t k = 0; k < g->_fanoutList[j]->_faninList.size(); k++)
					{	
						if(g->_fanoutList[j]->getin(k) == g)
						{
							am_i_inv = g->_fanoutList[j]->invin(k);
							g->_fanoutList[j]->_faninList[k] = (CirGate*)((size_t)d|(size_t)am_i_inv);//update the fanin of the fanout of d, it should be "replaced" instead of "push back" to maintain the order of fanins
						}
					}
				}
				cout << "Strashing: " << d->getID() << " merging " << g->getID() << "...\n";
				_netList[g->getID()] = 0;
				delete g;
				--MILOA[4];
			}
			else
				strash.insert(k,g);				
		}
	}
	dodfs(&_dfsList);	
}
void CirMgr::assignFEC()
{
	for(list<FECGroup>::iterator it = _fecGrps.begin(); it != _fecGrps.end(); ++it)
	{
		for(FECGroup::iterator gate = (*it).begin(); gate != (*it).end(); ++gate)
			(*gate)->_grp = &(*it);
	}
}
void CirMgr::genProofModel(SatSolver &s)
{
	for(size_t i = 0; i < _dfsList.size(); i++)
	{
		CirGate* g = _dfsList[i];
		g->_satVar = s.newVar();
		if(g->getTypeStr() == "AIG")
			s.addAigCNF(g->_satVar,g->getin(0)->_satVar,g->invin(0),g->getin(1)->_satVar,g->invin(1));
	}
	s.assertProperty(_netList[0]->_satVar,false);
}
bool CirMgr::XorProve(SatSolver &s, CirGate* ga, CirGate* gb, bool inv)
{
	s.assumeRelease();
	Var result = s.newVar();
	s.addXorCNF(result, ga->_satVar, false, gb->_satVar, inv);
	s.assumeProperty(result, true);
	return s.assumpSolve();
}
void CirMgr::fraig_const(SatSolver& s, FECGroup* const_group)
{
	for(FECGroup::iterator it = (*const_group).begin(); it != (*const_group).end(); ++it)
	{
		CirGate* g = (*it);
		bool non_inv = (g->_simvalue == 0);
		if(g->getTypeStr() == "CONST0")
			continue;
//		cout << "Proving " << g->_ID << " = " << (non_inv ? 1 : 0) << "... " ;
		s.assumeRelease();
		s.assumeProperty(g->_satVar, non_inv);
		bool result = s.assumpSolve();
		if(!result)
		{
			//UNSAT
			g->getin(0)->deleteout(g);
			g->getin(1)->deleteout(g);
			for(size_t j = 0; j < g->_fanoutList.size(); j++)
			{
				_netList[0]->addfanout(g->_fanoutList[j]);
				bool am_i_inv;
				for(size_t k = 0; k < g->_fanoutList[j]->_faninList.size(); k++)
				{	
					if(g->_fanoutList[j]->getin(k) == g)
					{
						am_i_inv = (non_inv) ^ (g->_fanoutList[j]->invin(k));
						g->_fanoutList[j]->_faninList[k] = (CirGate*)((size_t)(_netList[0])|(size_t)am_i_inv);
					}
				}
			}
			cout << "Fraig: " << (non_inv? "" : "!" ) << "0 merging " << g->getID() << "...\n";
			_netList[g->getID()] = 0;
			--MILOA[4];
		}
	}
}
void CirMgr::fraig_done(SatSolver& s, FECGroup* group)
{
	CirGate* begin = 0;
	for(FECGroup::iterator it = (*group).begin(); it != (*group).end(); ++it)
	{
		if(it == (*group).begin())
		{
			begin = (*it);
			continue;
		}
		CirGate* g = (*it);
		bool inv = (*group).getfirstinv() ^ it.inv();
		cout << "Proving (" << begin->_ID << ", " << (inv ? "!" : "") << g->_ID  << ")...\n" ;
		bool result = XorProve(s,begin,g,inv);
		if(!result)
		{
			//UNSAT
			g->getin(0)->deleteout(g);
			g->getin(1)->deleteout(g);
			for(size_t j = 0; j < g->_fanoutList.size(); j++)
			{
				if(g->_fanoutList[j])
				{
				begin->addfanout(g->_fanoutList[j]);
				bool am_i_inv;
				for(size_t k = 0; k < g->_fanoutList[j]->_faninList.size(); k++)
				{	
					if(g->_fanoutList[j]->getin(k) == g)
					{
						am_i_inv = inv ^ (g->_fanoutList[j]->invin(k));
						g->_fanoutList[j]->_faninList[k] = (CirGate*)((size_t)(begin)|(size_t)am_i_inv);
					}
				}
				}
			}
			cout << "Fraig: " <<  begin->getID() << " merging " << (inv? "" : "!" ) << g->getID() << "...\n";
			_netList[g->getID()] = 0;
			--MILOA[4];
		}
	}

}
void
CirMgr::fraig()
{
	assert(_fecGrps.size());
	SatSolver s;
	s.initialize();
	genProofModel(s);
	size_t counter = 0;
	for(list<FECGroup>::iterator it = _fecGrps.begin(); it!= _fecGrps.end(); ++it, counter ++)
	{
		cout << "FEC Group # " << counter << endl;
		if(&(*it)  == _netList[0]->_grp)
			fraig_const(s, &(*it));			
		else
			fraig_done(s, &(*it));
	}
	sweep();
	strash();
	for(size_t i = 0; i < _netList.size(); ++i)
	{
		if(_netList[i])
		_netList[i]->_grp = 0;
	}
	_fecGrps.clear();
	dodfs(&_dfsList);	
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
