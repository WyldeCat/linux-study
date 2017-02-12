#include <cstdio>
#include <memory>
#include <vector>
#include <map>
#include <functional>

#include <stdlib.h>

template<typename T> 
class my_alloc
{
public:
  static void *offset;
public:
  static void set_offset(void *_offset){ offset = _offset; } 

  typedef T value_type;
  typedef size_t size_type;

  typedef T* pointer;
  typedef const T* const_pointer;

  typedef T& reference;
  typedef const T& const_reference;
  
  pointer address(reference r) const { return &r; }
  const_pointer address(const_reference r) const { return &r; }

  my_alloc() noexcept = default;
  ~my_alloc() noexcept {}

  pointer allocate(size_type n)
  {
    void *tmp = offset;
    offset += (n * sizeof(value_type));
    return tmp;
  }
  void deallocate(pointer p, size_type n)
  {
    ::operator delete(p);
  }

  void construct(pointer p, const T& val){ new(p) T(val); }
  void destroy(pointer p){ p->~T(); }


  template<typename U>
  struct rebind {typedef std::allocator<U> other; };
};
template<typename T> void* my_alloc<T>::offset;

template<typename T> bool operator==(const my_alloc<T> &, const my_alloc<T> &){ return true; }
template<typename T> bool operator!=(const my_alloc<T> &, const my_alloc<T> &){ return false; }

int main( )
{
  void *a = malloc(1024 * sizeof(int));
  int b;

  my_alloc<std::pair<int,int>>::set_offset(a);
  std::map<int, int, std::less<int>, my_alloc<std::pair<int,int>>> mp; 

  printf("a : %d\n",a);
  mp.insert(std::make_pair(1,3));
  printf("mp.begin() : %d, mp.begin()->first : %d\n", mp.begin(), mp.begin()->first);
  printf("&mp[1] : %d, mp[1] : %d\n",&mp[1], mp[1]);
  mp.insert(std::make_pair(0,4));
  printf("mp.begin() : %d, mp.begin()->first : %d\n", mp.begin(), mp.begin()->first);
  printf("&mp[1] : %d, mp[1] : %d\n",&mp[1], mp[1]);
  printf("&mp[0] : %d, mp[0] : %d\n",&mp[0], mp[0]);
  mp.insert(std::make_pair(-1,4));
  printf("mp.begin() : %d, mp.begin()->first : %d\n", mp.begin(), mp.begin()->first);
  printf("&mp[1] : %d, mp[1] : %d\n",&mp[1], mp[1]);
  printf("&mp[0] : %d, mp[0] : %d\n",&mp[0], mp[0]);
  printf("&mp[-1] : %d, mp[-1] : %d\n",&mp[-1], mp[-1]);
 
  return 0;
}
