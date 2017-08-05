/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <list>
#include <bitset>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
bool gateCmp(size_t g1, size_t g2)
{
	return ((CirGate*)(g1 & ~(size_t)0x1))->getID() < ((CirGate*)(g2 & ~(size_t)0x1))->getID();
}
bool groupCmp(FECGroup g1, FECGroup g2)
{
	return ((CirGate*)(g1.getfirst() & ~(size_t)0x1))->getID() <  ((CirGate*)(g2.getfirst() & ~(size_t)0x1))->getID();
}
void
CirMgr::randomSim()
{
	_fecGrps.clear();
	size_t _failed = 0, _simcount = 0, _failthresh = (size_t)(log2(MILOA[1]+MILOA[4]));
	cout << "MAX_FAILS = " << _failthresh << endl;
	HashMap<Simkey,FECGroup> newgrps(getHashSize(_dfsList.size()));
//do the first time simulation and collection
	FECGroup ini(_netList[0]);
	newgrps.insert(Simkey(0),ini);
	DFS_simvalue_gen(1);
	write_sim(64);
	for(size_t i = 0; i < _dfsList.size(); i++)
		Collect_groups(newgrps,_dfsList[i]);
	for(HashMap<Simkey,FECGroup>::iterator it = newgrps.begin(); it != newgrps.end(); ++it)
	{
		if((*it).second.size() > 1)
			_fecGrps.push_back((*it).second);
	}
	_simcount += 64;

//start random simulation and collection until done
	while(_failed < _failthresh)
	{
		size_t _fecGrps_size = _fecGrps.size();
		DFS_simvalue_gen(1);	
		write_sim(64);
		size_t iterate_count = 0;
		for(list<FECGroup>::iterator it = _fecGrps.begin(); it != _fecGrps.end() && iterate_count < _fecGrps_size; ++iterate_count)
		{
			newgrps.init(getHashSize((*it).size()));
			assert((*it).size() > 1);
			for(FECGroup::iterator gate = (*it).begin(); gate != (*it).end(); gate++)
				Collect_groups(newgrps,(*gate));
			it = _fecGrps.erase(it);
			for(HashMap<Simkey,FECGroup>::iterator groups = newgrps.begin(); groups != newgrps.end(); ++groups)
			{
				if((*groups).second.size() > 1)
					_fecGrps.push_back((*groups).second);
			}
		}
		_simcount += 64;
		if(_fecGrps_size == _fecGrps.size())
			++_failed;
	}

	for(list<FECGroup>::iterator it = _fecGrps.begin(); it != _fecGrps.end(); ++it)
		std::sort( (*it)._groupList.begin(),(*it)._groupList.end(),gateCmp );

	_fecGrps.sort(groupCmp);	
	if(_fecGrps.size())
		assignFEC();
	cout << _simcount << " patterns simulated." << endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
	_fecGrps.clear();
	string pat;
	size_t input[MILOA[1]] = {0};
	size_t counter = 0;
	HashMap<Simkey, FECGroup>newgrps(getHashSize(_dfsList.size()));
	FECGroup ini(_netList[0]);
	newgrps.insert(Simkey(0), ini);
	while(patternFile >> pat)
	{
		if(pat.size() != MILOA[1])
		{
			cout << "Error: Pattern(" << pat << ") length(" << pat.size() << ")does not match the number of inputs(" << MILOA[1] << ") in a circuit!!\n";
			return;
		}
		for(size_t i = 0; i < pat.size(); ++i)
		{
			if(pat[i] != '0' && pat[i] != '1')
			{
				cout << "Error: Pattern(" << pat << ") contains a non-0/1 characters(\'" << pat[i] << "\').\n";
				return;
			}
			else
				input[i] = ( input[i] << 1 | (pat[i]-'0') );
		}	
		if(++counter%64 == 0)
		{
			if(counter == 64)
			{
				for(size_t i = 0; i < _PIList.size(); i++)
					_PIList[i]->_simvalue = input[i];
				DFS_simvalue_gen(0);
				write_sim(64);
				for(size_t i = 0; i < _dfsList.size(); i++)
					Collect_groups(newgrps,_dfsList[i]);
				for(HashMap<Simkey,FECGroup>::iterator it = newgrps.begin(); it != newgrps.end(); ++it)
				{
					if((*it).second.size() > 1)
						_fecGrps.push_back((*it).second);
				}

			}
			else 
			{
				size_t _fecGrps_size = _fecGrps.size();
				for(size_t i = 0; i < _PIList.size(); i++)
					_PIList[i]->_simvalue = input[i];
				DFS_simvalue_gen(0);
				write_sim(64);
				size_t iterate_count = 0;
				for(list<FECGroup>::iterator it = _fecGrps.begin(); it != _fecGrps.end() && iterate_count < _fecGrps_size; ++iterate_count)
				{
					newgrps.init(getHashSize((*it).size()));
					assert((*it).size() > 1);
					for(FECGroup::iterator gate = (*it).begin(); gate != (*it).end(); gate++)
						Collect_groups(newgrps,(*gate));
					it = _fecGrps.erase(it);
					for(HashMap<Simkey,FECGroup>::iterator groups = newgrps.begin(); groups != newgrps.end(); ++groups)
					{
						if((*groups).second.size() > 1)
							_fecGrps.push_back((*groups).second);
					}
				}
			}
		}
	}
	if(counter%64)
	{
		if(counter < 64)
		{
			for(size_t i = 0; i < _PIList.size(); i++)
				_PIList[i]->_simvalue = input[i];
			DFS_simvalue_gen(0);
			write_sim(counter);
			for(size_t i = 0; i < _dfsList.size(); i++)
				Collect_groups(newgrps,_dfsList[i]);
			for(HashMap<Simkey,FECGroup>::iterator it = newgrps.begin(); it != newgrps.end(); ++it)
			{
				if((*it).second.size() > 1)
					_fecGrps.push_back((*it).second);
			}
		}
		else 
		{
			size_t _fecGrps_size = _fecGrps.size();
			for(size_t i = 0; i < _PIList.size(); i++)
				_PIList[i]->_simvalue = input[i];
			DFS_simvalue_gen(0);
			write_sim(counter%64);
			size_t iterate_count = 0;
			for(list<FECGroup>::iterator it = _fecGrps.begin(); it != _fecGrps.end() && iterate_count < _fecGrps_size; ++iterate_count)
			{
				newgrps.init(getHashSize((*it).size()));
				assert((*it).size() > 1);
				for(FECGroup::iterator gate = (*it).begin(); gate != (*it).end(); gate++)
					Collect_groups(newgrps,(*gate));
				it = _fecGrps.erase(it);
				for(HashMap<Simkey,FECGroup>::iterator groups = newgrps.begin(); groups != newgrps.end(); ++groups)
				{
					if((*groups).second.size() > 1)
						_fecGrps.push_back((*groups).second);
				}
			}
		}
	}

	for(list<FECGroup>::iterator it = _fecGrps.begin(); it != _fecGrps.end(); ++it)
		std::sort( (*it)._groupList.begin(),(*it)._groupList.end(),gateCmp );
	_fecGrps.sort(groupCmp);	
	if(_fecGrps.size())
	assignFEC();
	cout << "Total # of FEC Groups: " << _fecGrps.size() << endl;
	cout << counter << " patterns simulated." << endl;
}
void CirMgr::DFS_simvalue_gen(bool rand)
{
		for(size_t i = 0; i < _dfsList.size(); i++)
		{
			CirGate* g = _dfsList[i];
			if (g->getTypeStr() == "CONST0")
				g->_simvalue = 0;
			else if (g->getTypeStr() == "PI" && rand)
				g->_simvalue = (((size_t)(rnGen(INT_MAX))) << 32)| (rnGen(INT_MAX));
			else if (g->getTypeStr() == "AIG")
			{
				CirGate* gin0 = g->getin(0);
				CirGate* gin1 = g->getin(1);
				if(gin0->getTypeStr() == "UNDEF")
					gin0->_simvalue = 0;
				if(gin1->getTypeStr() == "UNDEF")
					gin1->_simvalue = 0;
				if(!g->invin(0) &&  !g->invin(1))
					g->_simvalue = ((gin0->_simvalue) & (gin1->_simvalue));
				else if(!g->invin(0) &&  g->invin(1))
					g->_simvalue = ((gin0->_simvalue) &  ~(gin1->_simvalue));
				else if(g->invin(0) &&  !g->invin(1))
					g->_simvalue = (~(gin0->_simvalue) & (gin1->_simvalue));
				else if(g->invin(0) &&  g->invin(1))
					g->_simvalue = (~(gin0->_simvalue) & ~(gin1->_simvalue));
			}
			else if(g->getTypeStr() == "PO")
			{
				if(g->invin(0) == 0 )
					g->_simvalue = g->getin(0)->_simvalue; 
				else
					g->_simvalue = ~(g->getin(0)->_simvalue);
			}
			else
				continue;
		}
}
void CirMgr::Collect_groups(HashMap<Simkey, FECGroup> &newgrps,CirGate* g)
{
	if(g->getTypeStr() != "AIG" && g->getTypeStr() != "CONST0")
		return;
	FECGroup *group = 0;
	if(newgrps.query(Simkey(g->_simvalue),group))
	{
		if(g->getTypeStr() == "CONST0")
			return;
		group->add(g,0);
	}
	else if (newgrps.query(Simkey(~(g->_simvalue)),group))
		group->add(g,1);
	else
	{
		FECGroup n(g);
		newgrps.insert(Simkey(g->_simvalue),n);
	}
}
void CirMgr::write_sim(size_t used)
{
	if(_simLog)
	{
		size_t input[_PIList.size()], output[_POList.size()];
		for(size_t i = 0; i < _PIList.size(); i++)
			input[i] = _PIList[i]->_simvalue;
		for(size_t i = 0; i < _POList.size(); i++)
			output[i] = _POList[i]->_simvalue;
		for(size_t i = 0; i < used; i ++)
		{
			for(size_t j = 0; j < _PIList.size(); j++)
			{
				bitset<64> b_in(input[j]);
				*_simLog << b_in[used-1-i];
			}
			*_simLog << " ";
			for(size_t j = 0; j < _POList.size(); j++)
			{
				bitset<64> b_out(output[j]);
				*_simLog << b_out[used-1-i];
			}
			*_simLog << endl;
		}
	}
}
/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
