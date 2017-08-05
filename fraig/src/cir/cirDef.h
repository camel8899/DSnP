/****************************************************************************
  FileName     [ cirDef.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic data or var for cir package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_DEF_H
#define CIR_DEF_H

#include <vector>
#include "myHashMap.h"

using namespace std;

// TODO: define your own typedef or enum

class CirGate;
class CirMgr;
class SatSolver;
typedef vector<CirGate*>           GateList;
typedef vector<size_t>           IdList;

enum GateType
{
   UNDEF_GATE = 0,
   PI_GATE    = 1,
   PO_GATE    = 2,
   AIG_GATE   = 3,
   CONST_GATE = 4,

   TOT_GATE
};

class FECGroup
{
	friend class CirMgr;
	public:
		FECGroup (){}
		FECGroup (CirGate* g){_groupList.push_back((size_t)g);}
		~FECGroup () {}
		void add (const CirGate* g,const bool inv)
		{
			_groupList.push_back((size_t)g | (size_t)inv);
		}
		size_t size(){return _groupList.size();}
		void print()
		{
			for(size_t i = 0; i < _groupList.size(); i++)
			{
				cout << _groupList[i] << " ";
			}
			cout << endl;
		}

	bool getfirstinv(){return _groupList[0] & (size_t)0x1;}
	size_t getfirst(){return _groupList[0];}
	class iterator
	{
		friend class CirMgr;
		friend class FECGroup;
		public:
			iterator(){}
			iterator(vector<size_t>::iterator it){_it = it;}
			~iterator(){}

			CirGate* operator *(){return (CirGate*)((*_it) & ~(size_t)0x1);}
			iterator& operator++(){++_it ;return (*this);}
			iterator operator++(int)
			{
				iterator tmp = (*this);
				++(*this);
				return tmp;
			}
			iterator& operator =(const iterator& cpy){_it = cpy._it; return (*this);}
			bool operator == (const iterator& com){return (_it == com._it);}
			bool operator != (const iterator& com){return (_it != com._it);}
			bool inv() const {return ((*_it) & (size_t)0x1) ;}
		private:
			vector<size_t>::iterator _it;
		
	};
	iterator begin(){return iterator(_groupList.begin());}
	iterator end() {return iterator(_groupList.end());}
	iterator erase(iterator& erase){ erase._it = _groupList.erase(erase._it); return erase;}
	private:
		IdList _groupList;

};

class Simkey
{
	public:
		Simkey(size_t _number = 0):_simnumber(_number){}
		~Simkey(){}
		size_t operator() () const { return _simnumber;}
		bool operator == (const Simkey& k){return (k._simnumber == _simnumber);}
	private:
		size_t _simnumber;
};
#endif // CIR_DEF_H
