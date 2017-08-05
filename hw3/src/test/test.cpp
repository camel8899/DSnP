/****************************************************************************
  FileName     [ test.cpp ]
  PackageName  [ test ]
  Synopsis     [ Test program for simple database db ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <cstdlib>
#include "dbTable.h"

using namespace std;

extern DBTable dbtbl;

class CmdParser;
CmdParser* cmdMgr = 0; // for linking purpose

int
main(int argc, char** argv)
{
   if (argc != 2) {  // testdb <cvsfile>
      cerr << "Error: using testdb <cvsfile>!!" << endl;
      exit(-1);
   }

   ifstream inf(argv[1]);

   if (!inf) {
      cerr << "Error: cannot open file \"" << argv[1] << "\"!!\n";
      exit(-1);
   }

   if (dbtbl) {
      cout << "Table is resetting..." << endl;
      dbtbl.reset();
   }
   if (!(inf >> dbtbl)) {
      cerr << "Error in reading csv file!!" << endl;
      exit(-1);
   }

   cout << "========================" << endl;
   cout << " Print table " << endl;
   cout << "========================" << endl;
   cout << dbtbl << endl;

   // TODO
   // Insert what you want to test here by calling DBTable's member functions
	cout << dbtbl.getMax(0) << endl;
	cout << dbtbl.getMin(0) << endl;
	cout << dbtbl.getSum(0) << endl;
cout << dbtbl.getCount(0) << endl;
cout << dbtbl.getAve(0) << endl;
cout << dbtbl.getMax(1) << endl;
cout << dbtbl.getMin(1) << endl;
cout << dbtbl.getSum(1) << endl;
cout << dbtbl.getCount(1) << endl;
cout << dbtbl.getAve(1) << endl;
DBRow r;
r.addData(1);
r.addData(2);
dbtbl.addRow(r);
cout << dbtbl << endl;
  	cout << dbtbl.getMax(0) << endl;
	cout << dbtbl.getMin(0) << endl;
	cout << dbtbl.getSum(0) << endl;
cout << dbtbl.getCount(0) << endl;
cout << dbtbl.getAve(0) << endl;
cout << dbtbl.getMax(1) << endl;
cout << dbtbl.getMin(1) << endl;
cout << dbtbl.getSum(1) << endl;
cout << dbtbl.getCount(1) << endl;
cout << dbtbl.getAve(1) << endl;
cout << dbtbl.nRows() << endl;
vector<int> c;
//c.push_back(1);
//c.push_back(2);
dbtbl.addCol(c);
cout << dbtbl ;
   	cout << dbtbl.getMax(0) << endl;
	cout << dbtbl.getMin(0) << endl;
	cout << dbtbl.getSum(0) << endl;
cout << dbtbl.getCount(0) << endl;
cout << dbtbl.getAve(0) << endl;
cout << dbtbl.getMax(1) << endl;
cout << dbtbl.getMin(1) << endl;
cout << dbtbl.getSum(1) << endl;
cout << dbtbl.getCount(1) << endl;
cout << dbtbl.getAve(1) << endl;
cout << dbtbl.getMax(2) << endl;
cout << dbtbl.getMin(2) << endl;
cout << dbtbl.getSum(2) << endl;
cout << dbtbl.getCount(2) << endl;
cout << dbtbl.getAve(2) << endl;
dbtbl.printCol(2);
dbtbl.printSummary();

dbtbl.delCol(1);

cout << dbtbl ;
   	cout << dbtbl.getMax(0) << endl;
	cout << dbtbl.getMin(0) << endl;
	cout << dbtbl.getSum(0) << endl;
cout << dbtbl.getCount(0) << endl;
cout << dbtbl.getAve(0) << endl;
cout << dbtbl.getMax(1) << endl;
cout << dbtbl.getMin(1) << endl;
cout << dbtbl.getSum(1) << endl;
cout << dbtbl.getCount(1) << endl;
cout << dbtbl.getAve(1) << endl;
cout << dbtbl.getMax(2) << endl;
cout << dbtbl.getMin(2) << endl;
cout << dbtbl.getSum(2) << endl;
cout << dbtbl.getCount(2) << endl;
cout << dbtbl.getAve(2) << endl;
dbtbl.printCol(2);
dbtbl.printSummary();

return 0;
}
