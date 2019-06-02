#include "edgelist.h"
#include <iostream>
#include <cstdlib>

namespace mylist {
EdgeList::EdgeList() {
	head = NULL; tail = NULL;
	numOutEdges = 0;
}

void EdgeList::addEdge(vertexid_t vid,char label) {
	EdgeNode *p = (EdgeNode*)malloc(sizeof(EdgeNode));
	p->vid = vid;
	p->label = label;
	p->next = NULL;
	if(head == NULL)
		head = tail = p;	
	else {	
		tail->next = p;
		tail = p;
	}
	++numOutEdges;
}

void EdgeList::clear() {
	if(head) {
		EdgeNode *p,*next;
		p = head;
		while(p) {
			next = p->next;
			free(p);
			p = next;
		}
		numOutEdges = 0;
		head = NULL;
	}	
}

void EdgeList::print() {
	if(!head)
		cout << "empty EdgeList!" << endl;
	else {
		cout << "EdgeList: " << endl;
		EdgeNode *cur = head;
		while(cur) {
			cout << "(" << cur->vid << "," << (int)cur->label << ")" << " -> ";
			cur = cur->next;
		}
		cout << "end" << endl;
	}
}

bool EdgeList::checkDuple() {
	if(!head || !head->next)
		return false;
	else {
		EdgeNode *pre = head;
		EdgeNode *cur = pre->next;
		while(cur) {
			if(pre->vid == cur->vid && pre->label == cur->label)
				return true;
			pre = cur;
			cur = cur->next;
		}
		return false;
	}
}

}

