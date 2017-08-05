/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//

template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d, BSTreeNode<T>* parent = 0, BSTreeNode<T>* left = 0, BSTreeNode<T>* right = 0):
      _data(d), _parent(parent), _left(left), _right(right) {}

   T              _data;
   BSTreeNode<T>* _parent;
   BSTreeNode<T>*  _left;
   BSTreeNode<T>*  _right;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
   public:
   BSTree() {
      _root = 0;
	  _end = new BSTreeNode<T> (T());
   }
   ~BSTree() { clear();delete _end; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class BSTree;

   public:
      iterator(BSTreeNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () 
		{ 	
			if(_node->_right)
			{
				_node = _node ->_right;
				while(_node->_left)	
					_node = _node->_left;
				return (*this);							
			}
			BSTreeNode<T> *run = _node;
			while(run->_parent)
			{
				if(run->_parent->_left == run)
				{
					_node = run->_parent;
					return *this;
				}
				run = run->_parent;
			}
			assert(run->_parent == 0);
			return *this;
	   }
      iterator operator ++ (int) { iterator tmp(*this);
									++(*this);									
									return tmp; }
      iterator& operator -- () 
		{ 
			if(_node->_left)
			{
				_node = _node ->_left;
				while(_node->_right)	
					_node = _node->_right;
				return (*this);							
			}
			BSTreeNode<T> *run = _node;
			while(run->_parent)
			{
				if(run->_parent->_right == run)
				{
					_node = run->_parent;
					return *this;
				}
				run = run->_parent;
			}
			assert(run->_parent == 0);
			return *this;
		}	
      iterator operator -- (int) { iterator tmp(*this);
									--(*this);
									return tmp; }

      iterator& operator = (const iterator& i) { _node = i._node;
												return *this; }

      bool operator != (const iterator& i) const { return (_node != i._node); }
      bool operator == (const iterator& i) const { return (_node == i._node); }

   private:
      BSTreeNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const 
	{ 
		if(!_root)
			return _end;
		BSTreeNode<T> *run = _root;
		while(run->_left)
		{
			run = run->_left;
		}
		return iterator(run);
    }
   iterator end() const { return iterator(_end); }
   bool empty() const { return(!_root); }
   size_t size() const 
   {  
		size_t size = 0;
		for(iterator it = begin(); it != end(); ++it)
			size ++;
		return size;
   }
   void insert(const T& x)
	 {
		if(!_root)
		{
			_root = new BSTreeNode<T>(x);
			_root->_right = _end;
			_end->_parent = _root;
		}
		else
		{
			BSTreeNode<T> *run = _root;
			BSTreeNode<T> *prev = run;
			while(1)
			{
				prev = run;
				run = (x <= run->_data) ? run->_left : run->_right;
				if(!run || run == _end)
					break;
			}
			if(prev->_data >= x)
			{
				prev->_left = new BSTreeNode<T>(x);
				prev->_left->_parent = prev;
			}
			else
			{
				BSTreeNode<T> * _new = new BSTreeNode<T>(x);
				prev->_right = _new;
				_new->_parent = prev;
				if(run == _end)
				{
					_new->_right = _end;
					_end->_parent = _new;
				}				
			}
		}
     }
   void pop_front() { erase(begin());}
   void pop_back() 
	{ 
		BSTreeNode<T> *run = _root;
		while(run->_right != _end)
		{
			run = run->_right;
		}
		erase(iterator(run));
	}

   // return false if nothing to erase
   bool erase(iterator pos) 
	{ 
		if(empty())
			return false;
		if(!(pos._node->_left) && !(pos._node->_right))
		{
			assert(pos._node->_parent);
			if(pos._node->_parent->_left == pos._node)
				 pos._node->_parent->_left = 0 ;
			else
				 pos._node->_parent->_right = 0;
			delete pos._node;
			pos._node = 0;
			return true;		
		}
		else if(!(pos._node->_left))
		{
			if(pos._node == _root)
			{
				if(pos._node->_right == _end)
				{
					delete _root;
					_root = 0;
					return true;
				}
				_root = _root->_right;
				_root->_parent = 0;
			}
			else
			{
				assert(pos._node->_parent);
				if(pos._node->_parent->_left == pos._node)
				{
					pos._node->_parent->_left = pos._node->_right;
					pos._node->_right->_parent = pos._node->_parent;
				}
				else
				{
					pos._node->_parent->_right = pos._node->_right;
					pos._node->_right->_parent = pos._node->_parent;
				}
			}
			delete pos._node;
			pos._node = 0;
			return true;
		}
		else if(!(pos._node->_right))
		{
	
			assert(pos._node->_parent);
			if(pos._node->_parent->_left == pos._node)
			{
				pos._node->_parent->_left = pos._node->_left;
				pos._node->_left->_parent = pos._node->_parent;
			}
			else
			{
				pos._node->_parent->_right = pos._node->_left;
				pos._node->_left->_parent = pos._node->_parent;
			}
			delete pos._node;
			pos._node = 0;
			return true;
		}
		else
		{
			iterator prev = begin();
			while(prev != pos)
				prev ++;
			prev --;
			*pos = *prev;
			return erase(prev);		
		}
    }
   bool erase(const T& x) 
	{
		if(empty())
			return false;
		BSTreeNode<T> *run = _root;
		while(run && run != _end)
		{
			if(run->_data == x)
			{
				return erase(iterator(run));
			}
			else
				run = (x < run->_data) ? run->_left : run->_right;
		}
		return false;
    }
	void sort() {};
	void print() {};
   void clear() 
	{
		while(!empty())
			pop_front();
    }
private:
   BSTreeNode<T>*  _root;
   BSTreeNode<T>*  _end;    
};

#endif // BST_H
