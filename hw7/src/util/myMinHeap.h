/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const 
	{ 
		if(_data.size())
			return _data[0];
		else
			return Data();
	}
   void insert(const Data& d) 
	{
		size_t start = size();
		_data.push_back(d);
		while(start > 0)
		{
			size_t parent = (start-1)/2;
			if(_data[parent] < d)
				break;
			_data[start] = _data[parent];
			start = parent;			
		}	
		_data[start] = d;
	}
   void delMin() { delData(0); }
   void delData(size_t i)
	{
		if(i >= size())
			return;
		size_t start = i;
		while(start > 0)
		{
			swap(_data[start], _data[(start-1)/2]);
			start = (start-1)/2;
		}
		size_t child = 2*start+1;
		while(child <= size()-1)
		{
			if(child < size()-1)//has right child
			{
				if(_data[child+1] < _data[child])
					++child;
			}
			if(_data[size()-1] < _data[child])
				break;
			_data[start] = _data[child];
			start = child;
			child = 2*start+1;
		}
		_data[start] = _data.back();
		_data.pop_back();	
    }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
