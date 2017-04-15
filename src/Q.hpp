//
// Q.hpp
//
// Linked queue implementation using templates.
//
// Created by Daniel Kozitza
//

#ifndef _Q
#define _Q

#include <cassert>
#include <cstdlib>

using namespace tools;

template<class T>
class Q;

template<class T>
ostream& operator<<(ostream& out, Q<T>& q);

template<class T>
class Q {
   private:
      struct Node {
         T item;
         Node* next;
      };
      Node*  first;
      Node*  last;
      size_t q_size;
      Node* pub_ptr;

      Node* get_node(const T& item, Node* next);

   	// keep track of node and index for [] access assuming it will be
     	// integral. Don't increment the access ptr if the requested index
      // is the same as the last call.
   	Node*  access_ptr;
      size_t access_i;

   public:
      Q();
      Q(const Q& src);
      void operator=(const Q& src);
      void init(const Q& src);

      // operator[] will sometimes increment access_ptr
      // accessing the same index twice will not increment access_ptr
      // big O for random access is i
      // for integral access from first to last it is constant
      // from random to last it is i the first time then constant the rest
      T& operator[](size_t i);
      ~Q();
      void clear();
      void enq(const T& item);
      T deq();
      void deq_discard();
      void delete_item(size_t i);
      void transfer_item(size_t i, Q<T>& q2);

      // set the item pointer to the first item
      void reset_item_ptr();
      T* get_item_ptr();
      // this will increment the item pointer until it is NULL
      void increment_item_ptr();

      size_t size();

      friend ostream& operator<<<T>(ostream& out, Q<T>& q);
};

//
// The template implementation is here so that this code will be generated and
// compiled with the client code. Also there will be no Q.cpp file to be
// mistakenly compiled and linked.
//

template<class T>
Q<T>::Q() {
   first      = NULL;
   last       = NULL;
   pub_ptr    = NULL;
   access_ptr = NULL;
   access_i     = 0;
   q_size     = 0;
}

template<class T>
Q<T>::Q(const Q& src) {
   first      = NULL;
   last       = NULL;
   pub_ptr    = NULL;
   access_ptr = NULL;
   access_i     = 0;
   q_size     = 0;
   init(src);
}

template<class T>
void Q<T>::operator=(const Q& src) {
   if (this == &src)
      return;
   clear();
   init(src);
}

template<class T>
void Q<T>::init(const Q& src) {
   Node *p;
   q_size = 0;

   if (src.first == NULL) {
      first = NULL;
      last  = NULL;
   }
   else {
      first = get_node(src.first->item, NULL);
      q_size++;
      last = first;
      p = src.first->next;
      while (p != NULL) {
         last->next = get_node(p->item, NULL);
         last = last->next;
         q_size++;
         p = p->next;
      }
   }
}

template<class T>
Q<T>::~Q() {
   clear();
}

template<class T>
void Q<T>::clear() {
   Node* tmp;
   while (first != NULL) {
      tmp = first;
      first = first->next;
      delete tmp;
   }
   last = NULL;
   q_size = 0;
}

template<class T>
void Q<T>::enq(const T& item) {
   if (last == NULL) {
      first = get_node(item, NULL);
      last = first;
      pub_ptr = first;
      access_ptr = first;
   }
   else {
      last->next = get_node(item, NULL);
      last = last->next;
   }
   q_size++;
}

template<class T>
T Q<T>::deq() {
   assert(first != NULL);

   T tmp_item = first->item;
   Node* tmp_node = first;

   first = first->next;
   if (first == NULL)
      last = NULL;

   delete tmp_node;
   q_size--;

   pub_ptr = first;

   if (access_i == 0)
      access_ptr = first;

   if (access_i > 0)
      access_i--;

   return tmp_item;
}

// deq without copying the item
//
template<class T>
void Q<T>::deq_discard() {
   assert(first != NULL);

   Node* tmp_node = first;

   first = first->next;
   if (first == NULL)
      last = NULL;

   delete tmp_node;
   q_size--;

   pub_ptr = first;
   if (access_i == 0)
      access_ptr = first;

   if (access_i > 0)
      access_i--;
}

template<class T>
void Q<T>::delete_item(size_t i) {
   assert(q_size > i);

   if (i == 0) {
      deq_discard();

      return;
   }

   Node* prev_node = first;
   Node* del_node = first->next;

   for (size_t z = 1; z < q_size; ++z) {
      if (z == i) {
         break;
      }
      prev_node = del_node;
      del_node = del_node->next;
   }

   // patch the list
   prev_node->next = del_node->next;

   delete del_node;
   q_size--;

   if (prev_node->next == NULL)
      last = prev_node;

   pub_ptr = first;

   // if current access pointer is deleted replace it with the following
   if (access_i == i) {
      if (prev_node->next != NULL) {
         access_ptr = prev_node->next;
      }
      else { // we deleted the last index
       access_ptr = first;
       access_i = 0;
      }
   }
   if (access_i > i)
      access_i--;
}

// transfer_item
//
// Copy the pointer of the selected node to the second Q and remove it from
// this Q. This does not copy the item and leaves the second Q responsible
// for deleting the node.
//
template<class T>
void Q<T>::transfer_item(size_t i, Q<T>& q2) {
   assert(q_size > i);

   Node* t_node = NULL;

   // remove node i from this queue
   if (i == 0) {
      t_node = first;

      // patch the list
      first = first->next;
      if (first == NULL)
         last = NULL;
   
      // handle public and access pointers
      pub_ptr = first;
      if (access_i == 0)
         access_ptr = first;
   
      if (access_i > 0)
         access_i--;
   }
   else {

      // get t_node to node i
      t_node = first->next;
      Node* prev_node = first;
      for (size_t z = 1; z < q_size; ++z) {
         if (z == i) {
            break;
         }
         prev_node = t_node;
         t_node = t_node->next;
      }

      // patch the list
      prev_node->next = t_node->next;

      // in case we deleted the last node
      if (prev_node->next == NULL)
         last = prev_node;

      // if current access pointer is deleted replace it with the following
      if (access_i == i) {
         if (prev_node->next != NULL) {
            access_ptr = prev_node->next;
         }
         else { // we deleted the last index
          access_ptr = first;
          access_i = 0;
         }
      }
      if (access_i > i)
         access_i--;
   }
   q_size--;

   // remove any link to this Q
   t_node->next = NULL;

   // enque t_node in q2 without copying it
   if (q2.last == NULL) {
      q2.first = t_node;
      q2.last = q2.first;
      q2.pub_ptr = q2.first;
      q2.access_ptr = q2.first;
   }
   else {
      q2.last->next = t_node;
      q2.last = q2.last->next;
   }
   q2.q_size++;
}

template<class T>
size_t Q<T>::size() {
   return q_size;
}

template<class T>
typename Q<T>::Node* Q<T>::get_node(const T& item, Node* next) {
   Node* p = new Node;
   p->item = item;
   p->next = next;
   return p;
}

template<class T>
T* Q<T>::get_item_ptr() {
   return &pub_ptr->item;
}

template<class T>
void Q<T>::increment_item_ptr() {
   if (pub_ptr != NULL)
      pub_ptr = pub_ptr->next;
}

template<class T>
void Q<T>::reset_item_ptr() {
   pub_ptr = first;
}

template<class T>
T& Q<T>::operator[](size_t i) {
   assert(q_size > i);

   if (i == access_i)
      return access_ptr->item;

   if (i == access_i+1 && access_ptr->next != NULL) {
      access_ptr = access_ptr->next;
      access_i++;
      return access_ptr->item;
   }

   // restart from the beginning of the list until we get to i
   access_ptr = first;
   for (size_t z = 0; z < i; ++z) {
      access_ptr = access_ptr->next;
   }
   access_i = i;
   return access_ptr->item;
}

template<class T>
ostream& operator<<(ostream& out, Q<T>& q) {
   typename Q<T>::Node* tmp = q.first;
   if (tmp != NULL) {
      while (tmp->next != NULL) {
         //cout << tmp->item << " ";
         cout << tmp << " ";
         tmp = tmp->next;
      }
      //cout << tmp->item;
      cout << tmp;
   }
   return out;
}

#endif
