/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}
//functions for util
void lexOptions(const string& option, vector<string>& tokens) 
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
}
unsigned
myStr2Uns(const string& str)
{
   unsigned num = 0;
   size_t i = 0;
   for (; i < str.size(); ++i) {
      if (isdigit(str[i])) {
         num *= 10;
         num += int(str[i] - '0');
      }
      else return false;
   }
   return num;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirGate*
CirMgr::addUndef(unsigned gid)
{
	CirGate* undef = new CirGate (UNDEF_GATE, 0, gid);
	_netList[gid] = undef;
	return undef;
}
bool
CirMgr::readCircuit(const string& fileName)
{
  ifstream in(fileName.c_str());
	if(!in.is_open())
		return false;
	string s;
	vector<string> line;
	while(getline(in, s))
	{
		line.push_back(s);
	}
	in.close();
	if (line.empty())
		return false;	
	lineNo = 0;
	//Header
	vector<string>options;
	lexOptions(line[0],options);
	if(options.size() != 6)
	{
		return false;
	}
	MILOA[0] = myStr2Uns(options[1]);
	MILOA[1] = myStr2Uns(options[2]);
	MILOA[2] = myStr2Uns(options[3]);
	MILOA[3] = myStr2Uns(options[4]);
	MILOA[4] = myStr2Uns(options[5]);
	_netList.resize(MILOA[0] + MILOA[3] +1,0);//1 For const gate 0
	_netList[0] = new CirGate(CONST_GATE,0,0);
	//PI
	while(lineNo <  MILOA[1])
	{
		lineNo++;		
		unsigned _literalid = myStr2Uns(line[lineNo]);
		CirGate * pi = new CirGate(PI_GATE,lineNo+1,_literalid/2);
		_PIList.push_back(pi);
		_netList[_literalid/2] = pi;
	}
	//PO
	while(lineNo < MILOA[1] + MILOA[3])
	{
		lineNo++;
		unsigned poid = MILOA[0] + _POList.size() + 1;
		CirGate *po = new CirGate(PO_GATE,lineNo+1,poid);
		_POList.push_back(po);
		_netList[poid] = po;
		unsigned _literalid = myStr2Uns(line[lineNo]);
		CirGate* from = getGate(_literalid/2);
		if(!from)
			from = addUndef(_literalid/2);
		po->addfanin(from, _literalid % 2);
		from -> addfanout(po);
	}
	//AIG
	while(lineNo < MILOA[1] + MILOA[3] + MILOA[4])
	{
		lineNo++;
		vector<string>_aig;
		lexOptions(line[lineNo],_aig);
		unsigned _rhsid = myStr2Uns(_aig[0]);
		unsigned _lhs1id = myStr2Uns(_aig[1]);
		unsigned _lhs2id = myStr2Uns(_aig[2]);
		CirGate* rhs = getGate(_rhsid/2);
		if(!rhs)
			rhs = new CirGate(AIG_GATE, lineNo+1,_rhsid/2);
		_netList[_rhsid/2] = rhs;
		CirGate *lhs1 = getGate(_lhs1id/2);
		if(!lhs1)
			lhs1 = addUndef(_lhs1id/2);
		CirGate *lhs2 = getGate(_lhs2id/2);
		if(!lhs2)
			lhs2 = addUndef(_lhs2id/2);
		if(rhs->getTypeStr() == "UNDEF")
		{
			assert(rhs->getLineNo() == 0);
			rhs->settype(AIG_GATE);
			rhs->setLineNo(lineNo+1);	
		}
		rhs->addfanin(lhs1, _lhs1id % 2);
		lhs1->addfanout(rhs);
		rhs->addfanin(lhs2, _lhs2id % 2);
		lhs2->addfanout(rhs);
	}
	//symbol
	while(lineNo < line.size()-1)
	{
		lineNo++;
		string after = line[lineNo];
		if(after[0] == 'c')
			break;
		if(after[0] == 'i')			
		{	
			unsigned index = myStr2Uns(after.substr(1,after.find_first_of(" ")-1));			
			_PIList[index]->setSymbol(after.substr(after.find_first_of(" ")+1));
		}
		else if (after[0] == 'o')
		{
			unsigned index = myStr2Uns(after.substr(1,after.find_first_of(" ")-1));			
			_POList[index]->setSymbol(after.substr(after.find_first_of(" ")+1));
		}
	}
	dodfs(&_dfsList);
	return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
	cout << endl;
	cout << "Circuit Statistics\n";
	cout << "==================\n";
	cout << "  PI" << setw(12) << right << MILOA[1] << endl;
	cout << "  PO" << setw(12) << right << MILOA[3] << endl;
	cout << "  AIG" << setw(11) << right << MILOA[4] << endl;
	cout << "------------------\n";
	cout << "  Total" << setw(9) << right << MILOA[1] + MILOA[3] + MILOA[4] << endl;

}

void
CirMgr::dodfs(GateList* current)  
{
	CirGate::setGlobal();
	_dfsList.clear();
	for(size_t i = 0; i < _POList.size(); i++)
		_POList[i]->add_dfs(current);
}

void
CirMgr::printNetlist() const
{
	cout << endl;
	for(size_t i = 0; i < _dfsList.size(); i++)
	{
		CirGate* g = _dfsList[i];
		if(g)
		{
			cout << "[" << i << "] ";
			g->printGate();
		}
	}	
/*
   cout << endl;
   for (unsigned i = 0, n = _dfsList.size(); i < n; ++i) {
      cout << "[" << i << "] ";
      _dfsList[i]->printGate();
   }
*/
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
	for(size_t i = 0; i < _PIList.size(); i++)
		cout << " " << _PIList[i] -> getID();
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
	for(size_t i = 0; i < _POList.size(); i++)
		cout << " " << _POList[i] -> getID();
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
	vector<unsigned>f_fin, d_n_used;
	for(GateList::const_iterator it = _netList.begin(); it != _netList.end(); ++it)
	{
		if(!(*it))
			continue;
		for(size_t i = 0; i < (*it)->_faninList.size(); ++i)
		{	
			if((*it)->getin(i)->getTypeStr() == "UNDEF")
			{
				assert((*it)->getTypeStr() != "PI");
				f_fin.push_back((*it)->_ID);
				break;
			}
		}
		if((*it)->getTypeStr() != "PO" && (*it)->getTypeStr() != "CONST0" &&  (*it)->_fanoutList.empty())
			d_n_used.push_back((*it)->_ID);
	}
	if(f_fin.size())
	{
		cout << "Gates with floating fanin(s):";
		for(size_t i = 0; i < f_fin.size(); i++)
			cout << " " << f_fin[i];
		cout << endl;
	}
	if(d_n_used.size())
	{
		cout << "Gates defined but not used  :";
		for(size_t i = 0; i < d_n_used.size(); i++)
			cout << " " << d_n_used[i];
		cout << endl;
	}
}

void
CirMgr::printFECPairs() 
{
	int counter = 0;
	for(list<FECGroup>::iterator it = _fecGrps.begin(); it !=_fecGrps.end(); ++it)
	{
		cout << "[" << counter << "]";
		for(FECGroup::iterator gate = (*it).begin(); gate != (*it).end(); ++gate)
		{
			cout << " ";
			if(gate.inv()^(*it).getfirstinv())
				cout << "!";
			cout << (*gate)->_ID;
		}
		cout << endl;
		counter ++;
	}
}

void
CirMgr::writeAag(ostream& outfile) const
{
	unsigned _aignum = 0;
	for(size_t i = 0; i < _dfsList.size(); i++)
	{
		if(_dfsList[i]->getTypeStr() == "AIG")
			_aignum++;
	}
	outfile << "aag " << MILOA[0] << " " << MILOA[1] << " " << MILOA[2] << " "<< MILOA[3] << " " << _aignum << endl;
	for(size_t i = 0; i < _PIList.size(); i++)
	{
		outfile << _PIList[i]->_ID*2 << endl;
	}
	for(size_t i = 0; i < _POList.size(); i++)
	{
		outfile << (_POList[i]->getin(0)->_ID)*2 + (_POList[i]->invin(0) ? 1 : 0 )  << endl;
	}
	for(size_t i = 0; i < _dfsList.size(); i++)
	{
		if(_dfsList[i]->getTypeStr() == "AIG")
		{
			outfile << _dfsList[i]->_ID *2 << " ";
			outfile << (_dfsList[i]->getin(0)->_ID)*2 + (_dfsList[i]->invin(0) ? 1 : 0 )  << " ";
			outfile << (_dfsList[i]->getin(1)->_ID)*2 + (_dfsList[i]->invin(1) ? 1 : 0 )  << endl;
		}
	}
	for(size_t i = 0; i < _PIList.size(); i++)
	{
		if(_PIList[i]-> _sym != "")
			outfile << "i" << i << " " << _PIList[i]->_sym << endl;
	}
	for(size_t i = 0; i < _POList.size(); i++)
	{
		if(_POList[i]-> _sym != "")
			outfile << "o" << i << " " << _POList[i]->_sym << endl;
	}
	outfile  << "c" << endl << "AAG output by Mike Hsiao (B03901030) DSnP-2016" << endl;
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
	unsigned _aignum = 0;
	for(size_t i = 0; i < _dfsList.size(); i++)
	{
		if(_dfsList[i]->getTypeStr() == "AIG")
			_aignum++;
	}
	outfile << "aag " << MILOA[0] << " " << MILOA[1] << " " << MILOA[2] << " "<< MILOA[3] << " " << _aignum << endl;
	for(size_t i = 0; i < _PIList.size(); i++)
	{
		outfile << _PIList[i]->_ID*2 << endl;
	}
	for(size_t i = 0; i < _POList.size(); i++)
	{
		outfile << (_POList[i]->getin(0)->_ID)*2 + (_POList[i]->invin(0) ? 1 : 0 )  << endl;
	}
	for(size_t i = 0; i < _dfsList.size(); i++)
	{
		if(_dfsList[i]->getTypeStr() == "AIG")
		{
			outfile << _dfsList[i]->_ID *2 << " ";
			outfile << (_dfsList[i]->getin(0)->_ID)*2 + (_dfsList[i]->invin(0) ? 1 : 0 )  << " ";
			outfile << (_dfsList[i]->getin(1)->_ID)*2 + (_dfsList[i]->invin(1) ? 1 : 0 )  << endl;
		}
	}
	for(size_t i = 0; i < _PIList.size(); i++)
	{
		if(_PIList[i]-> _sym != "")
			outfile << "i" << i << " " << _PIList[i]->_sym << endl;
	}
	for(size_t i = 0; i < _POList.size(); i++)
	{
		if(_POList[i]-> _sym != "")
			outfile << "o" << i << " " << _POList[i]->_sym << endl;
	}
	outfile  << "c" << endl << "AAG output by Mike Hsiao (B03901030) DSnP-2016" << endl;

}

