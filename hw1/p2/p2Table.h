#ifndef P2_TABLE_H
#define P2_TABLE_H
#include <vector>
#include <string>
#include <climits>
#include<iostream>
using namespace std;
class Row
{
public:
   Row (size_t c){ _data = new int [c];
					for(int i = 0 ;i < c; i++)
					_data[i] = INT_MAX;
				 }
   const int operator[] (size_t i) const {return _data[i]; } // TODO
   int& operator[] (size_t i) {return _data[i]; } // TODO
private:
   int  *_data;
};

class Table
{
public:
   const Row& operator[] (size_t i) const {return _rows[i];}
   Row& operator[] (size_t i){ return _rows[i];}
   bool read(const string&);
   void print();
   void sum(int);
   void max(int);
   void min(int);
   void count(int);
   void ave(int);
   void add(vector<string>);
   int col_t = 1,row_t = 1;
private:
   vector<Row>  _rows;      
};

#endif // P2_TABLE_H
