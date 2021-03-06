/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next ;
								return *(this); }
      iterator operator ++ (int) { iterator tmp(*this);
									++(*this);									
									return tmp; }
      iterator& operator -- () { _node = _node->_prev;
								return *(this); }
      iterator operator -- (int) { iterator tmp(*this);
									--(*this);
									return tmp; }

      iterator& operator = (const iterator& i) { _node = i._node;
												return *(this); }

      bool operator != (const iterator& i) const { return (_node != i._node); }
      bool operator == (const iterator& i) const { return (_node == i._node); }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head); }
   iterator end() const { return iterator(_head->_prev); }
   bool empty() const { return(begin() == end()); }
   size_t size() const 
	{  
		size_t size = 0;
		for(iterator it = begin(); it != end(); ++it)
			size++;	
		return size;
    }
   void push_back(const T& x)
	 {
		if(empty())
		{
			DListNode<T>* tmp = new DListNode<T>(x, end()._node->_prev, end()._node);
			_head = tmp;
			end()._node->_prev = end()._node->_next = _head;
			return;
		}
		DListNode<T>* tmp = new DListNode<T>(x, end()._node->_prev, end()._node);
		tmp->_prev->_next = tmp;
		tmp->_next->_prev = tmp;
     }
   void pop_front() { erase(begin());}
   void pop_back() { erase(iterator(end()._node->_prev));}

   // return false if nothing to erase
   bool erase(iterator pos) 
	{ 
		if(empty())
			return false;
		if(pos == begin())
			_head = _head->_next;
		pos._node->_prev->_next = pos._node->_next;
		pos._node->_next->_prev = pos._node->_prev;
		delete pos._node;
		pos._node = 0;
		return true;
    }
   bool erase(const T& x) 
	{
		if(empty()) 
			return false;
		for(iterator it = begin(); it != end(); ++it)
		{
			if(it._node->_data == x)
				return erase(it);
		}
		return false;
    }

   void clear() 
	{
		while(!empty())
			pop_back();
    }  // delete all nodes except for the dummy node

   void sort() const 
	{
		if(!empty())
		{
			for(iterator it(begin()._node->_next); it != end(); ++it)
			{
				T tmp = *it;
				iterator comp(it);
				while(comp != end())
				{
					if(*comp > tmp)
					{
						comp._node->_next->_data = *comp;
						*comp = tmp;
					}	
					--comp;	
				}
			}
		}
    }

private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
