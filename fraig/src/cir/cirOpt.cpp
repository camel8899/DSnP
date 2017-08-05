/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <vector>
using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
	for(size_t i = 0; i < _netList.size(); i++)
	{
		CirGate* g = _netList[i];
		if(!g)
			continue;
		if(!g->isGlobal() && (g->isAig() || g->getTypeStr() == "UNDEF"))
		{
			cout << "Sweeping: " << g->getTypeStr() << "(" << g->getID() << ") removed..." << endl;
			if(g->isAig())
				--MILOA[4];
			for(size_t j = 0; j < g->_faninList.size(); j++)
			{
				CirGate* input = g->getin(j);
				if(input && (input->isGlobal() || input->getTypeStr() == "PI"))
					input->deleteout(g);				
			}
			delete g;
			_netList[i] = 0;
		}
	}
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
	CirGate* zero = _netList[0];
	CirGate *gin1, *gin2, *gre;
	bool inv1, inv2, invre;
	for(size_t i = 0; i < _dfsList.size(); i++)
	{
		CirGate* g = _dfsList[i];
		if(g->isAig())
		{
			gin1 = g->getin(0);
			gin2 = g->getin(1);
			inv1 = g->invin(0);
			inv2 = g->invin(1);
			if(gin1 == gin2)
			{
				if(inv1 == inv2)
				{
					gre = gin1;
					invre = inv1;
				}
				else
				{
					gre = zero;
					invre = false;
				}
			}
			else if(gin1 == zero)
			{
				if(inv1)
				{
					gre = gin2;
					invre = inv2;
				}
				else
				{
					gre = zero;
					invre = false;
				}	
			}
			else if(gin2 == zero)
			{
				if(inv2)
				{
					gre = gin1;
					invre = inv1;
				}
				else
				{
					gre = zero;
					invre = false;
				}
			}
			else
				continue;
			gin1->deleteout(g);
			gin2->deleteout(g);
			for(size_t j = 0; j < g->_fanoutList.size(); j++)
			{
				gre->addfanout(g->_fanoutList[j]);//add fanout to gre
				bool am_i_inv;
				for(size_t k = 0; k < g->_fanoutList[j]->_faninList.size(); k++)
				{	
					if(g->_fanoutList[j]->getin(k) == g)
					{
						am_i_inv = invre^(g->_fanoutList[j]->invin(k));
						g->_fanoutList[j]->_faninList[k] = (CirGate*)((size_t)gre|(size_t)am_i_inv);//update the fanin of the fanout of gre, it should be "replaced" instead of "push back" to maintain the order of fanins
					}
				}
			}
			cout << "Simplifying: " << gre->getID() << " merging ";
			if(invre)
				cout << "!";
			cout << g->getID() << "...\n";
			_netList[g->getID()] = 0;
			delete g;
			--MILOA[4];
		}
	}
	dodfs(&_dfsList);
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
