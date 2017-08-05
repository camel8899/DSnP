#include "p2Table.h"
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

// Implement member functions of class Row and Table here

bool
Table::read(const string& csvFile)
{   
    char tmp;
	bool rflag = false;
    string num;
	vector<int> first_line;
	int col = 0, row = 1;
    ifstream fin (csvFile.c_str());
    if(fin.is_open())
    {
		//find the numbers of column first
		while(fin.get(tmp))
		{
	    	if(tmp == ',')
			{
				col_t++;
				if(num != "")
					first_line.push_back(stoi(num));
				else
					first_line.push_back(INT_MAX);
				num = "";
			}
			else if(tmp == '\r')
			{
				if(num != "")
					first_line.push_back(stoi(num));
				else
					first_line.push_back(INT_MAX);
				num = "";				
				break;
			}
			else
				num += tmp;
		}
		//deal with the first line
		_rows.push_back(Row(col_t));
		for(int i = 0; i < col_t; i++)
		{
			if(first_line[i]!=INT_MAX)
			{
				_rows[0][i] = first_line[i];
			}
		}
		//start reading the others
  		_rows.push_back(Row(col_t));
		row_t++;
		while(fin.get(tmp))
		{
			if(tmp == ',')
			{
				if(num != "")
					_rows[row][col] = stoi(num);
				col++;
				num = "";
					
				rflag = false;
			}
			else if(tmp == '\r')
			{
				if(rflag == true)
				{
					break;
				}
				else
				{
					_rows.push_back(Row(col_t));
					row_t++;
					if(num != "")
						_rows[row][col] = stoi(num);
					row++;
  					col = 0;
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
		_rows.pop_back();
		row_t--;
		fin.close();
		return true;
    }
    else
    	return false;
}

void Table::print()
{
	for(int i = 0 ;i < row_t; i++)
	{
		for(int j = 0 ;j < col_t; j++)
		{
			if(_rows[i][j] == INT_MAX)
				cout << right << setw(4) << "";
			else
				cout << right << setw(4) << _rows[i][j];
		}
		cout << endl;
	}
}
void Table::sum(int col)
{
	int s = 0;
	for(int i = 0; i < row_t; i++)
	{
		if(_rows[i][col] != INT_MAX)
		{
			s += _rows[i][col];
		}
	}
	cout << "The summation of data in column #" << col << " is " << s << "." << endl;
}
void Table::max(int col)
{
	int m = INT_MIN;
	for(int i = 0; i < row_t; i++)
	{
		if(_rows[i][col] != INT_MAX)
		{
			if(_rows[i][col] > m)
			{
				m = _rows[i][col];
			}
		}
	}
	cout << "The maximum of data in column #" << col << " is " << m << "." << endl;
}
void Table::min(int col)
{
	int m = INT_MAX;
	for(int i = 0; i < row_t; i++)
	{
		if(_rows[i][col] != INT_MAX)
		{
			if(_rows[i][col] < m)
			{
				m = _rows[i][col];
			}
		}
	}
	cout << "The minimum of data in column #" << col << " is " << m << "." << endl;
}
void Table::count(int col)
{
	int cnt = row_t;
	for(int i = 0; i < row_t; i++)
	{
		if(_rows[i][col] == INT_MAX)
			cnt--;
		else
		{
			for(int j = 0; j < i; j++)
			{
				if((i != j) && (_rows[i][col] == _rows[j][col]))
					{
						cnt--;
						break;
					}
			}
		}
	}
	cout << "The distinct count of data in column #" << col << " is " << cnt << "." << endl;
}
void Table::ave(int col)
{
	int sum = 0, cell = 0;
	double avg;
	for(int i = 0; i < row_t; i++)
	{
		if(_rows[i][col] != INT_MAX)
		{
			sum += _rows[i][col];
			cell++;
		}
	}
	avg = (double)sum/(double)cell;
	cout << "The average of data in column #" << col << " is " << fixed << setprecision(1) << avg << "." << endl;
}
void Table::add(vector<string> input)
{
	
	_rows.push_back(Row(col_t));
	for(int i = 0; i < col_t; i++)
	{
		if(input[i] != "-")
		{
			_rows[row_t][i] = stoi(input[i]);
		}		
	}
	row_t++;
}
