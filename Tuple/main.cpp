#include <iostream>
#include <string>


template <typename T, typename U>
struct CompareType {
    static constexpr bool X = false;
};

template <typename T>
struct CompareType<T, T> {
    static constexpr bool X = true;
};

template <typename... Args>
class Tuple;

// partial specializations
//! Why we need this for actual comparison operators to work?
template<>
struct Tuple <> {
    //! Do we need this and what does this mean?
    // Tuple (const Tuple<>& other) = default;

    bool operator < (const Tuple<>& other) const  {
        return false;
    }
    bool operator > (const Tuple<>& other) const  {
        return false;
    }

    bool operator == (const Tuple<>& other) const {
        return true;
    }

    bool operator >= (const Tuple<>& other) const {
        return true;
    }
    bool operator <= (const Tuple<>& other) const {
        return true;
    }

    static constexpr int size_ = 0;

    friend std::ostream& operator << (std::ostream& out, const Tuple<>& t) {
        return out;
    }    
};


template <typename Head, typename... Tail>
class Tuple<Head, Tail...> {
  public:  
     Tuple(const Head& head, const Tail&... tail) : head_(head), tail_(tail...) {
     }

    //! is it copy constructor? Do we need to delete here something?  No
    Tuple <Head, Tail...>& operator = (const Tuple <Head, Tail...>& other) {
      //! addressof or &? Adressof because & can be overloaded
      if (this != std::addressof(other)) {
          std::cout << "copying" << std::endl;
          head_ = other.head_;
          tail_ = other.tail_;
      }
      return *this;
    }     

    bool operator < (const Tuple<Head, Tail...>& other) const {
        return head_ < other.head_ || tail_ < other.tail_;
    }

    bool operator > (const Tuple<Head, Tail...>& other) const {
        return head_ > other.head_ || tail_ > other.tail_;
    }    

    bool operator == (const Tuple<Head, Tail...>& other) const  {
        return !(*this < other) && !(*this > other);
    }

    bool operator >= (const Tuple<Head, Tail...>& other) const {
        return !(*this < other);
    }

    bool operator <= (const Tuple<Head, Tail...>& other) const {
        return !(*this > other);
    }

    friend std::ostream& operator << (std::ostream& out, const Tuple<Head, Tail...>& t) {
        out << t.head_ << " " << t.tail_;
        return out;
    }

  Head head_;
  Tuple<Tail...> tail_;
  typedef Head Type;
  static constexpr int size_ = sizeof...(Tail) + 1;
};

//! <index> here is like argument to function get()? What is the difference between doing this and doing usual way? We need to know the end during compilation
template <int index, typename... Args>
auto& get(Tuple<Args...>& tuple) {
  if constexpr (index == 0) {
    return tuple.head_;
  } else {
    return get<index - 1>(tuple.tail_);
  }
}


template <typename T, typename... Args>
auto& get(Tuple<Args...>& tuple) {
  //! here "Type" is alias for variable of the same type as Head or it is the type of variable Head itself? Type is the type because Head is the type not variable
  if constexpr (CompareType<T, typename Tuple<Args...>::Type>::X) {
    return tuple.head_;
  } else {
    return get<T>(tuple.tail_);
  }
}

template <typename Head, typename... Tail>
Tuple<Head, Tail...> makeTuple(const Head& head, const Tail&... tail) {
    return Tuple<Head, Tail...> (head, tail...);
}

namespace Helper {
    
template <typename Tuple1, int... Indexes1>
struct Concat {
  
  template <typename Tuple2, int... Indexes2>
  static auto unpack(Tuple1 t1, Tuple2 t2) {
    if constexpr (sizeof...(Indexes1) == Tuple1::size_ && sizeof...(Indexes2) == Tuple2::size_) {
      return  makeTuple(get<Indexes1>(t1)..., get<Indexes2>(t2)...);
    } else if constexpr (sizeof...(Indexes1) < Tuple1::size_) {
      return Concat<Tuple1, Indexes1..., sizeof...(Indexes1)>::template unpack<Tuple2>(t1, t2);
    } else {
      return  Concat<Tuple1, Indexes1...>::template unpack<Tuple2, Indexes2..., sizeof...(Indexes2)>(t1, t2);
    }
  }
};
}

template <typename Tuple1, typename Tuple2>
auto Concat(Tuple1 t1, Tuple2 t2) {
  return Helper::Concat<Tuple1>::template unpack<Tuple2>(t1, t2);
}


template <typename HeadTuple, typename... TailTuples>
auto TupleCat(HeadTuple head, TailTuples... tails) {
    if constexpr (sizeof...(TailTuples) == 0) {
      return head;
    } else {
      return Concat(head, TupleCat(tails...));
    }
}


template <typename Head, typename... Tail>
void print(const Head& head, const Tail&... tail) {
  std::cout << head << " " << std::endl;
  if constexpr (sizeof...(tail) != 0) {
     print(tail...);
  }
}

int main() {
//   print(1,2,3,4,"sssr");
  //std::cout << max(1,2,3,4,41,1,100,2);
  Tuple<int, int, int> t(1,2,3);
//   std::cout << get<0>(t) << std::endl;
//   get<0>(t) = 15;
  // std::cout << get<0>(t) << std::endl;
//   std::cout << get<1>(t) << std::endl;
//   std::cout << get<2>(t) << std::endl;

//   Tuple<int, char, double> t(1,'x',1.);
//   std::cout << get<char>(t) << std::endl;

  Tuple<int, int, int> const t1(1, 5, 5);
  // Tuple<int, int, int> t2(3, 2, 1);
  Tuple<int, int, double> t2(2, 4, 3.1);

  Tuple<double> t3(10.5);
  // int x = 0;  
  // x = t1 >= t2 ? 1 : 0;
  // std::cout << x << std::endl;  

  // it woks even without assignment operator
  // t1 = t2;
  // std::cout << get<0>(t1) << std::endl;

  // Tuple<int, int, int> t3 = makeTuple(10, 11, 12);
  // std::cout << get<0>(t3) << std::endl;

  // std::cout << (t1 < t2) << std::endl; 

  auto x = TupleCat(t1, t2, t3);

  std::cout << x << std::endl;

  return 0;
}

/* HW 8
do refactor

*/