/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include <set>
#include "cirDef.h"
#include "sat.h"
#define NEG 0x1
using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate
{
	friend CirMgr;
public:
   CirGate(GateType type = UNDEF_GATE,unsigned line = 0,unsigned ID = 0):_ref(0),_type(type), _line(line), _ID(ID),_sym(""),_grp(0),_simvalue(0){}
   virtual ~CirGate() {_faninList.clear();_fanoutList.clear();}

   // Basic access methods
   void settype(GateType type){_type = type;}
   string getTypeStr() const 
	{ 
		switch(_type)
		{
			case UNDEF_GATE:return "UNDEF";
			case PI_GATE:return "PI";
			case PO_GATE:return "PO";
			case AIG_GATE:return "AIG";
			case CONST_GATE:return "CONST0";
			default:return "";
		}
	 }
   void setLineNo(unsigned no){_line = no;}
   unsigned getLineNo() const { return _line; }
   void setID(unsigned ID){_ID = ID;}
   unsigned getID() const {return _ID;}
   void setSymbol(string s){_sym = s;};
   string getSymbol() const{return _sym;}
   bool isAig(){return (_type == AIG_GATE);}
   // Printing functions
   void printGate() const ;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void recursive_in(int level, set<unsigned>& known,bool inv) const;
   void recursive_out(int level, set<unsigned>& known, bool inv) const;
   //dfs functions
   void setRef(){_ref = _globalref;}
   bool isGlobal(){return (_ref == _globalref);}
   static void setGlobal() {_globalref ++;}
   void add_dfs(GateList *current);
   //access circuits
   void addfanin(CirGate* in, bool _inv = false)
	{	
		if (_inv)
			in =(CirGate*) ((size_t)in | (size_t)NEG);
		_faninList.push_back(in);
	}
	bool invin(size_t i) const
	{
		return ((size_t)_faninList[i] & NEG);
	}
	CirGate* getin(size_t i) const
	{
		if(i > _faninList.size()) return 0;
		return(CirGate*)(((size_t)_faninList[i]) & ~size_t(NEG));
	}
	CirGate* getout(size_t i) const
	{
		if(i > _fanoutList.size()) return 0;
		return _fanoutList[i];
	}
	void addfanout(CirGate* out)
	{
		_fanoutList.push_back(out);
	}
	void deleteout(CirGate* out)
	{
		for(size_t i = 0; i < _fanoutList.size(); i++)
		{
			if(_fanoutList[i] == out)
				_fanoutList.erase(_fanoutList.begin()+i);
			return;
		}
	}
private:
//members for DFS
	size_t _ref;
	static size_t _globalref;
//members of gate
	GateType _type;
	GateList _faninList;
	GateList _fanoutList;
	unsigned _line;
	unsigned _ID;
	string _sym;
	FECGroup* _grp;
//members for simulation
	size_t _simvalue;
//members for SAT
	Var _satVar; 
protected:
};

class strashkey
{
	public:
		strashkey():_input0(0),_input1(0),_keynumber(0){}
		strashkey(CirGate* g)
		{
			_input0 = g->getin(0)->getID() << 1 | g->invin(0);
			_input1 = g->getin(1)->getID() << 1 | g->invin(1);
			if(_input0 < _input1)
				_keynumber = (_input0 << 32) + _input1; 
			else
				_keynumber = (_input1 << 32) + _input0;
		}
		~strashkey(){}
		size_t operator() () const {return _keynumber;}
		bool operator == (const strashkey& k)
			{return (_keynumber == k._keynumber);}
	private:
	
		size_t _input0,_input1,_keynumber;

};

#endif // CIR_GATE_H
