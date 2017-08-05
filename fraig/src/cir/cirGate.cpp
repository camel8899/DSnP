/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include <bitset>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
size_t CirGate::_globalref = 0;
void
CirGate::printGate() const
{
	cout << setw(3) << left << getTypeStr();
	if(getTypeStr() != "CONST0")
		cout  << " " << _ID;
	for(size_t i = 0; i < _faninList.size(); i++)
	{
		CirGate* g = getin(i);
		cout << " ";
		if(g->getTypeStr() == "UNDEF")
			cout << "*";
		if(invin(i))
			cout << "!";
		cout << g->_ID; 
	}
	if(_sym.size())
		cout << " (" << _sym << ")";
	cout << endl;
}
void
CirGate::add_dfs(GateList *current)
{
	if(isGlobal())
		return;
	if(getTypeStr() == "UNDEF")
	{
		setRef();
		return;
	}
	for(size_t i = 0; i < _faninList.size(); i++)
		getin(i)->add_dfs(current);
	current->push_back(this);
	setRef();
}
void
CirGate::reportGate() const
{
	const string seperate = "==================================================\n";
	stringstream ss;
	ss << "= " << getTypeStr() << "(" << _ID << ")";
	if (_sym.size())
		ss << "\"" << _sym << "\"";
	ss << ", line " << _line;
	cout << seperate;
	cout << setw(49) << left << ss.str() << "=\n";
	ss.str("");
	ss << "FECs: ";
	if(_grp)
	{
		for(FECGroup::iterator it = (*_grp).begin(); it != (*_grp).end(); ++it)
		{
			if((*it) == this)
				continue;
			else
			{
				if((*it)->_simvalue != _simvalue)
					ss << "!";
				ss << (*it)->_ID << " ";
			}	
		}
	}
	cout << "= " << setw(47) << left << ss.str()  << "=\n";
	ss.str("");
	ss << "Value: ";
	bitset<64> sim(_simvalue);
	for(int i = 0; i < 32; i++)
	{
		if(i && !(i%4))
			ss << "_";
		ss << sim[i];
	}	
	cout << "= " << setw(47) << left << ss.str() << "=\n";
	
	cout << seperate;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
	set<unsigned> known;
	recursive_in(level, known, false);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
	set<unsigned> known;
	recursive_out(level, known, false);
}

void
CirGate::recursive_in(int level, set<unsigned> &known, bool inv) const
{
	static int reverse_level = 0;
	for(int i = 0; i < 2*reverse_level; i++)
		cout << " ";
	if(inv)
		cout << "!";
	cout << getTypeStr() << " " ;
	if(getTypeStr() != "CONST0")
		cout << _ID;
	if(level)
	{
		if(known.find(_ID)!=known.end() && getTypeStr() != "PI" && getTypeStr() != "UNDEF")
			cout <<" (*)" << endl;
		else
		{
			cout << endl;
			known.insert(_ID);
			reverse_level ++;
			for(size_t i = 0;i < _faninList.size(); i++ )
				getin(i)->recursive_in(level-1, known,invin(i));
			reverse_level --;
		}
	}
	else
		cout << endl;
}

void
CirGate::recursive_out(int level, set<unsigned> &known, bool inv) const
{
	static int reverse_level = 0;
	for(int i = 0; i < 2*reverse_level; i++)
		cout << " ";
	if(inv)
		cout << "!";
	cout << getTypeStr() << " " << _ID;
	if(level)
	{
		if(known.find(_ID)!=known.end() && getTypeStr() != "PO" && getTypeStr() != "UNDEF")
			cout <<" (*)" << endl;
		else
		{
			cout << endl;
			known.insert(_ID);
			reverse_level ++;
			for(size_t i = 0;i < _fanoutList.size(); i++ )
			{
				CirGate* g = getout(i);
				bool am_i_inv = false;
				for(size_t j = 0; j < (g->_faninList).size(); j++)
				{
					if(this == g->getin(j))
					{
						am_i_inv = g->invin(j);
						break;
					}
				}
				_fanoutList[i]->recursive_out(level-1, known, am_i_inv);
			}
			reverse_level --;
		}
	}
	else
		cout << endl;
}

