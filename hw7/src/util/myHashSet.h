/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
		//constructors and destructor
		iterator(const HashSet<Data>* _set = 0, size_t _b = 0, size_t _s = 0):_current_set((HashSet<Data>*)_set), _bucket(_b), _slot(_s){}
		iterator(const iterator& i):_current_set(i._current_set), _bucket(i._bucket),_slot(i._slot){}
		~iterator(){}

		//functions
		const Data& operator *() const { return (*_current_set)[_bucket][_slot];}
   		Data& operator *() { return (*_current_set)[_bucket][_slot];}
		iterator& operator ++()
		{
			if(_slot + 1 >= (*_current_set)[_bucket].size())
			{
				++_bucket;
				while(_bucket < _current_set->_numBuckets)
				{
					if((*_current_set)[_bucket].size())
						break;
					else
						++_bucket;
				}
				_slot = 0;
			}
			else
				_slot ++;
			return *this;
		}
		iterator operator ++(int)
		{
			iterator temp = iterator(*this);
			++(*this);
			return temp;
		}
 		iterator& operator --()
		{
			if(_slot == 0)
			{
				--_bucket;
				while(_bucket >= 0)
				{
					if((*_current_set)[_bucket].size())
						break;
					else
						--_bucket;
				}
				_slot = (*_current_set)[_bucket].size()-1;
			}
			else
				_slot --;
			return *this;

		}
		iterator operator --(int)
		{
			iterator temp = iterator(*this);
			++(*this);
			return temp;
		} 
		iterator& operator = (const iterator& copy)
		{
			_current_set = copy._current_set;
			_bucket = copy._bucket;
			_slot = copy._slot;
			return *this;
		}
		bool operator == (const iterator& copy) const
		{
			return(	_current_set == copy._current_set && _bucket == copy._bucket && _slot == copy._slot);
		}
		bool operator != (const iterator& copy) const
		{
			return !((*this) == copy);
		}
		private:
		HashSet<Data>*	_current_set;
		size_t	_bucket;
		size_t	_slot;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b];}
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const 
	{
		for(size_t i = 0; i < _numBuckets; i++)
		{
			if(!_buckets[i].empty())
				return iterator(this,i,0);
		} 
		return end();
	}
   // Pass the end
   iterator end() const 
	{
		return iterator(this,_numBuckets, 0); 
	}
   // return true if no valid data
   bool empty() const 
	{
		for(size_t i = 0; i < _numBuckets; i++)
		{
			if(_buckets[i].size())
				return false;
		} 
		return true; 
	}
   // number of valid data
   size_t size() const 
	{ 
		size_t s = 0; 
		for(size_t i = 0; i < _numBuckets; i++)
		{
			s+=_buckets[i].size();
		}
		return s; 
	}

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const 
	{
		size_t _b = bucketNum(d); 
		return (find(_buckets[_b].begin(),_buckets[_b].end(),d)!=_buckets[_b].end()); 
	}

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const 
	{
		size_t _b = bucketNum(d);
		for(size_t i = 0; i < _buckets[_b].size(); i++)
		{
			if(_buckets[_b][i] == d)
			{
				d = _buckets[_b][i];
				return true;
			}
		}
		return false; 
	}

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) 
	{
		size_t _b = bucketNum(d);
		for(size_t i = 0; i < _buckets[_b].size(); i++)
		{
			if(_buckets[_b][i] == d)
			{
				 _buckets[_b][i] = d;
				return true;
			}
		}
		return false; 
 	}

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d)
	 {
		if(check(d))
			return false;
		_buckets[bucketNum(d)].push_back(d);
		cout << "Task node inserted: " << d << endl; 
		return true; 
	 }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) 
	{
		size_t _b = bucketNum(d);
		for(size_t i = 0; i < _buckets[_b].size(); i++)
		{
			if(_buckets[_b][i] == d)
			{
				_buckets[_b].erase(_buckets[_b].begin()+i);
				return true;
			}
		} 
		return false; 
	}

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
