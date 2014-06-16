#ifndef TYPETUPLE_H
#define TYPETUPLE_H

// Base case, Tuple<>
template< typename... Ts >
class TypeTuple {
public:
};

// recursive inheritance :D
template< typename T, typename... Ts >
class TypeTuple< T, Ts... > : private TypeTuple< Ts... > {
public:
   // we implement get() by checking wether element type match with
   // request type
   template< typename t >
   typename std::enable_if< std::is_same< t, T >::value, t& >::type
   get() {
     return element;
   }

   // above is not enough since it only check this class's element type,
   // we can check the rest of the tuple by inspecting its the parent classes.
   template< typename t >
   typename std::enable_if< !( std::is_same< t, T >::value ), t& >::type
   get() {
       return TypeTuple<Ts...>::template get<t>();  // call its parent's get()
                                                // ::template to shut compiler up
   }

private:
    T element;
};

#endif // TYPETUPLE_H
