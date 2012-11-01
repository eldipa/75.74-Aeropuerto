#ifndef BOUNDED_QUEUE_H_
#define BOUNDED_QUEUE_H_

#include <iostream>
#include <stdexcept>

template <class T, int N>
class BoundedQueue {
public:
   BoundedQueue() : 
      tail(0), size(0) {
   }

   void offer( const T& new_element ) {
      if ( size == N )
         throw std::runtime_error("BoundedQueue");
      elements[ (tail+size)%N ] = new_element;
      size++;
   }

   T take() {
      if( size == 0 )
         throw std::runtime_error("BoundedQueue");
      T& return_element = elements[ tail ];
      tail = (tail+1)%N;
      size--;
      return return_element;
   }

   unsigned int get_size() const {
      return size;
   }

   bool is_full() const {
      return (get_size()==N);
   }

   bool is_empty() const {
      return (get_size()==0);
   }

   //template <class T, int length>
   friend std::ostream& operator<<( std::ostream& stream, const BoundedQueue<T,N> q ) {
      for( unsigned int i=0; i<q.size; i++ ) {
         stream << "|" << q.elements[ (q.tail+i)%N ];
      }
      stream << "| " << "(" << q.get_size() << " elements)";
      return stream;
   }

private:
   T elements[N];
   unsigned int tail;
   unsigned int size;

};

#endif
