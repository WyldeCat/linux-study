#include <cstdio>
#include <memory>
#include <vector>

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

  my_alloc<int>::set_offset(a);
  std::vector<int,my_alloc<int> > v;
  v.push_back(1);
  v.push_back(2);
  printf("a: %d\n", a);
  printf("b: %d\n", b);
  printf("v.begin() : %d, *v.begin() : %d\n", v.begin(), *v.begin());
  printf("++v.begin() : %d, *(++v.begin()) : %d\n",++v.begin(), *(++v.begin()));
  return 0;
}
