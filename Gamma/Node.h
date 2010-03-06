#ifndef GAMMA_NODE_H_INC
#define GAMMA_NODE_H_INC

/*	Gamma - Generic processing library
	See COPYRIGHT file for authors and license information */

#include <iostream>
#include <stdio.h>

using std::cout;

#define TEM template <class T>

namespace gam{

/// A doubly-linked node.
template <class T>
class Node2{
public:
	Node2();
	Node2(bool zeroLinks);
	virtual ~Node2();
	
	T * nodeL;					///< Reference to left node.
	T * nodeR;					///< Reference to right node.
	
	void nodeInsertL(T & node);	///< Insert myself to left of node.
	void nodeInsertR(T & node);	///< Insert myself to right of node.
	void nodeRemove();			///< Remove myself from linked list.
	
	const Node2<T>& leftmost() const {
		Node2<T> * t = nodeL;
		Node2<T> * n = nodeL;
		while(t){ n = t; t = t->nodeL; }
		return n ? *n : *this;
	}
	
	void print(const char * append = "\n", FILE * fp = stdout) const;
	void printAll(const char * append = "\n", FILE * fp = stdout) const;
};


/// A quadruply-linked node.
template <class T>
class Node4{
public:
	Node4();
	virtual ~Node4();

	T * parent;
	T * child;
	T * right;
	T * left;

	void add(T * node);			///< Add node to my children.
	
	//! Remove myself from parent. Does not delete instance.
	
	//! This method should always be called from the instance's destructor.
	//!
	void remove();				

	void setAsFirstChild();		///< Make myself the first child
	void setAsLastChild();		///< Make myself the last child

	//! Returns next node using preorder traversal.
	
	//! If the next node is 'terminal', returns 0.
	//!
	T * nextPreorder(const T * terminal) const;

	//T * nextBreadth(T * terminate);		// Returns next node using breadth first traversal.
										// If the next node is 'terminate', returns 0.

	virtual void print(const char * append = "");			///< Print my value.
	void printDescendents(const char * append = "");		///< Print indented outline of my descendents.
};


// Implementation_______________________________________________________________

// Node2

TEM Node2<T>::Node2()
:	nodeL(0), nodeR(0)
{
}

TEM Node2<T>::Node2(bool zeroLinks){
	if(zeroLinks){
		nodeL = 0;
		nodeR = 0;
	}
}

TEM Node2<T>::~Node2(){
	nodeRemove();
}

TEM void Node2<T>::nodeInsertL(T & node){
	nodeL = node.nodeL;
	nodeR = &node;
	if(nodeL) nodeL->nodeR = (T *)this;
	node.nodeL = (T *)this;
}

TEM void Node2<T>::nodeInsertR(T & node){
	nodeR = node.nodeR;
	nodeL = &node;
	if(nodeR) nodeR->nodeL = (T *)this;
	node.nodeR = (T *)this;
}

TEM void Node2<T>::nodeRemove(){
	// connect neighbors
	if(nodeL){ nodeL->nodeR = nodeR; }
	if(nodeR){ nodeR->nodeL = nodeL; }
	nodeL = 0;
	nodeR = 0;
}



TEM void Node2<T>::print(const char * append, FILE * fp) const {
	fprintf(fp, "%p %p %p%s", nodeL, this, nodeR, append);
}

TEM void Node2<T>::printAll(const char * append, FILE * fp) const {

	Node2<T> const * n = &leftmost();
	
	while(n){
		fprintf(fp, n == this ? "(%p) " : " %p  ", n);
		n = n->nodeR;
	}
	fprintf(fp, append);
}

// Node4

TEM Node4<T>::Node4(){
	parent = 0;
	child = 0;
	right = 0;
	left = 0;
}

TEM Node4<T>::~Node4(){
}


TEM void Node4<T>::add(T * node){
	node->parent = (T *)this;
	
	if(child){
		T * last = child;
		while(last->right) last = last->right;
		
		last->right = node;
		node->left = last;
	}
	else{
		child = node;
	}
}


TEM void Node4<T>::remove(){

	// connect neighbor siblings
	if(left)	left->right = right;
	else		if(parent) parent->child = right;
	
	if(right)	right->left = left;
}



TEM void Node4<T>::setAsFirstChild(){

	if(left){	// i.e. not first child
	
		// connect neighbor siblings
		left->right = right;
		if(right) right->left = left;
		
		// insert in first child location
		parent->child->left = (T *)this;
		right = parent->child;
		left = 0;
		
		// update parent's link
		parent->child = (T *)this;
	}
}


TEM void Node4<T>::setAsLastChild(){

	if(right){	// i.e. not last child
	
		// connect neighbor siblings
		if(left) left->right = right;
		else			parent->child = right;
		right->left = left;
		
		// insert in last child location
		T * last = right;
		while(last->right) last = last->right;

		last->right = (T *)this;
		right = 0;
		left = last;
	}
}


TEM T * Node4<T>::nextPreorder(const T * terminal) const {
	
	T * node = (T *)this;
	
	if(node->child){
		node = node->child;
	}
	else if(node->right){
		node = node->right;
	}
	else{
		while( node != terminal && node->right == 0 ){
			node = node->parent;
		}

		if( node != terminal && node->right ){
			node = node->right;
		}
	}
	
	if(node == terminal) return 0;
	
	return node;
}


//template <class T>
//T * Node4<T>::nextBreadth(T * terminate){
//	
//	T * node = (T *)this;
//	
//	if(node->right){
//		node = node->right;
//	}
//	else if(node->parent && node->parent->child){
//	
//	}
//	
//	if(node == terminate) return 0;
//	
//	return node;
//}


TEM void Node4<T>::print(const char * append){
	cout << this << append;
}


TEM void Node4<T>::printDescendents(const char * append){

	T * node = (T *)this;

	do{
		T * p = node->parent;
		while(p != this->parent){ printf("  "); p = p->parent; }
		node->print("\n");
		node = node->nextPreorder((T *)this);
	}while(node);
	
	printf("%s",append);
}


// Node4 helper sub-classes

//#define TREE_OBJECT(name) name : public Node4<name>
//
//class TREE_OBJECT(TreeString){
//public:
//	TreeString(const char * str = "");			// from C string
//	TreeString(const char * str, int length);	// from substring
//
//	string * value;
//
//	void print(char * append){ cout << *value << append; }
//};
//
//inline TreeString::TreeString(const char * str){
//	value = new string(str);
//}
//
//inline TreeString::TreeString(const char * str, int length){
//	value = new string(str, length);
//}
//
//#undef TREE_OBJECT

} // end namespace gam

#undef TEM
#endif

