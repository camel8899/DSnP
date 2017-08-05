/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include "dbTable.h"
#include "util.h"
using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
	for(size_t i = 0; i < r.size(); i++)
	{
		if(i != r.size()-1)	{
			DBTable::printData(os,r[i]);
			os << ' ';
		}
		else
			DBTable::printData(os,r[i]);
	}
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
	for(size_t i = 0; i < t.nRows(); i++){
		for(size_t j = 0; j < t.nCols(); j++){
			if(t[i][j] == INT_MAX)
				os << right << setw(6) << ".";
			else
				os << right << setw(6) << t[i][j];
		}
	os << endl;
	}
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.
	char tmp;
	bool rflag = false;
    string num;
	DBRow tmpRow;
	while(ifs.get(tmp))
	{
			if(tmp == ',')
			{
				if(num != "")
					tmpRow.addData(atoi(num.c_str()));
				else
					tmpRow.addData(INT_MAX);
				num = "";
				rflag = false;
			}
			else if(tmp == '\r')
			{
				if(rflag == true)
					break;
				else
				{
						tmpRow.addData(num == "" ? INT_MAX :atoi(num.c_str()));
						t.addRow(tmpRow);
						tmpRow.reset();
						num="";
				}
				rflag = true;
			}
			else
			{
				 num+=tmp;
				 rflag = false;
			}
	}
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
	_data.erase(_data.begin()+c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
	for(size_t i = 0; i < _sortOrder.size(); i++)
	{
		if(r1[_sortOrder[i]]!=r2[_sortOrder[i]])
			return (r1[_sortOrder[i]] < r2[_sortOrder[i]]);
	}
   return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
	_table.clear();
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
	size_t min;
	if(d.size() < _table.size())
		min = d.size();
	else
		min = _table.size();
	for(size_t i = 0; i < min; i++)
		_table[i].addData(d[i]);
	for(size_t i = min; i < _table.size(); i++)
		_table[i].addData(INT_MAX);
	
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
	_table.erase(_table.begin()+c);
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
	int m = 0;
	size_t counter = 0;
	for(size_t i = 0; i < _table.size(); i++)
	{
		if(_table[i][c]!=INT_MAX)
			break;
		counter++;
	}
	if(counter == _table.size())
		return NAN;
	else{
		m = _table[counter][c];
		for(size_t  j = counter+1; j < _table.size(); j++)
		{
			if(_table[j][c] != INT_MAX)
			{	
				if(_table[j][c] > m)
					m = _table[j][c];
			}
		}
	}
   return (float)m;
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
	int m = 0;
	size_t counter = 0;
	for(size_t i = 0; i < _table.size(); i++)
	{
		if(_table[i][c]!=INT_MAX)
			break;
		counter++;
	}
	if(counter == _table.size())
		return NAN;
	else{
		m = _table[counter][c];
		for(size_t  j = counter+1; j < _table.size(); j++)
		{
			if(_table[j][c] != INT_MAX)
			{	
				if(_table[j][c] < m)
					m = _table[j][c];
			}
		}
	}
   return (float)m;
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
	int s = 0;
	size_t counter = 0;
	for(size_t i = 0; i < _table.size(); i++)
	{
		if(_table[i][c]!=INT_MAX)
			break;
		counter++;
	}
	if(counter == _table.size())
		return NAN;
	else{
		s = _table[counter][c];
		for(size_t  j = counter+1; j < _table.size(); j++)
		{
			if(_table[j][c] != INT_MAX)
			{	
				s+=_table[j][c];
			}
		}
	}
   return (float)s;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
	size_t cnt = _table.size();
	for(size_t i = 0; i < _table.size(); i++)
	{
		if(_table[i][c] == INT_MAX)
			cnt--;
		else
		{
			for(size_t j = 0; j < i; j++)
			{
				if((i != j) && (_table[i][c] == _table[j][c]))
					{
						cnt--;
						break;
					}
			}
		}
	}
   return cnt;
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
   	int cell = 1;
	float avg;
	int s = 0;
	size_t counter = 0;
	for(size_t i = 0; i < _table.size(); i++)
	{
		if(_table[i][c]!=INT_MAX)
			break;
		counter++;
	}
	if(counter == _table.size())
		return NAN;
	else{
		s = _table[counter][c];
		for(size_t j = counter+1; j < _table.size(); j++)
		{
			if(_table[j][c] != INT_MAX)
			{
				s += _table[j][c];
				cell++;
			}
		}
	}
	avg = (float)s/(float)cell;
	return avg;
//	return (float)cell;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
	std::sort(_table.begin(),_table.end(),s);
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
	for(size_t i = 0; i < _table.size(); i++)
	{
		if(i != _table.size()-1)	{
			DBTable::printData(cout,_table[i][c]);
			cout << ' ';
		}
		else
			DBTable::printData(cout,_table[i][c]);
	}
	cout << endl;
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

