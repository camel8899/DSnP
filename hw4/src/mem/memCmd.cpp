/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
	vector<string> options;
	if(!CmdExec::lexOptions(option, options))
		return CMD_EXEC_ERROR;
	if(options.size() == 0)
		return CmdExec::errorOption(CMD_OPT_MISSING, "");
	if (options.size() > 3)
		return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
	int numObjects = 0, arraySize = 0;
	bool numflag = false, arrflag = false; 
	for (size_t i = 0; i < options.size(); i++)
	{
		if(myStrNCmp("-Array", options[i], 2)==0)
		{
			if(arrflag)
				return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]); 
			if(++i >= options.size())
				return CmdExec::errorOption(CMD_OPT_MISSING, options[i-1]);
			if(!myStr2Int(options[i],arraySize))
				return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
			if(arraySize <= 0)	
				return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
			arrflag = true;
		}
		else
		{
			if(numflag)
				return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
			if(!myStr2Int(options[i], numObjects))
				return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
			if(numObjects <= 0)
				return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
			numflag = true;
		}		
	}
	if(numObjects <= 0 )
		return CmdExec::errorOption(CMD_OPT_MISSING, "");
	try
	{
		if(arrflag)
			mtest.newArrs(numObjects,arraySize);
		else
			mtest.newObjs(numObjects);
	}
	catch(bad_alloc)
	{
		return CMD_EXEC_ERROR;
	}

   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
	vector<string> options;
	if(!CmdExec::lexOptions(option, options))
		return CMD_EXEC_ERROR;
	if(options.size() == 0)
		return CmdExec::errorOption(CMD_OPT_MISSING, "");
	if(options.size() == 1)
		return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
	if (options.size() > 3)
		return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
	bool objflag = false, randflag = false, arrflag = false;
	int objId = 0,numRandId = 0;
	size_t  obj_index = 0, rand_index = 0; 
	for (size_t i = 0; i < options.size(); i++)
	{
		if(myStrNCmp("-Array",options[i],2) == 0)
		{
			if(arrflag)
				return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
			arrflag = true;
		}
		else if(myStrNCmp("-Index",options[i],2) == 0)
		{
			if(objflag)
				return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
			if(++i >= options.size())
				return CmdExec::errorOption(CMD_OPT_MISSING, options[i-1]);
			if(!myStr2Int(options[i], objId))
				return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
			if(objId < 0)
				return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
			objflag = true;
			obj_index = i;
		}
		else if(myStrNCmp("-Random",options[i],2) == 0)
		{
			if(randflag)
				return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
			if(++i >= options.size())
				return CmdExec::errorOption(CMD_OPT_MISSING, options[i-1]);
			if(!myStr2Int(options[i], numRandId))
				return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
			if(numRandId <= 0)
				return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
			randflag = true;
			rand_index = i;
		}
		else 
			return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
	}	
	if(arrflag)
	{
		if(objflag)
		{
			assert(!randflag);
			if((size_t)objId >= mtest.getArrListSize())
			{
				cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << objId << "!!\n";
					return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[obj_index]);
			}
			mtest.deleteArr(objId);
		
		}
		else if (randflag)
		{
			assert(!objflag);
			if(mtest.getArrListSize() == 0)
			{
				cerr << "Size of array list is 0!!\n";
					return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[rand_index]);
			}
			for(int i = 0; i < numRandId; i++)
				mtest.deleteArr(rnGen(mtest.getArrListSize()));
				
		}
	}
	else
	{
		if(objflag)
		{
			assert(!randflag);
			if((size_t)objId >= mtest.getArrListSize())
			{
				cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << objId << "!!\n";
					return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[obj_index]);
			}
			mtest.deleteObj(objId);
			
		}
		else if (randflag)
		{
			assert(!objflag);
			if(mtest.getObjListSize() == 0)
			{
				cerr << "Size of object list is 0!!\n";
					return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[rand_index]);
			}
			for(int i = 0; i < numRandId; i++)
				mtest.deleteObj(rnGen(mtest.getObjListSize()));
			
		}
	}
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


