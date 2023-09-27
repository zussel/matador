#include <type_traits>
#include <string>
#include <iostream>
#include <vector>
#include <list>

template < typename Type >
struct container_item
{
public:

  Type value;
  size_t size{};
};

template < typename Type, template < class ... > class Container >
struct container_iterator_traits;

template < typename Type >
struct container_iterator_traits<Type, std::vector>
{
  using self = container_iterator_traits<Type, std::vector>;
  using difference_type = std::ptrdiff_t;
  using value_type = Type;
  using pointer = Type*;
  using reference = Type&;
  using iterator_category = std::random_access_iterator_tag;
  using container_item_type = container_item<Type>;
  using container_type = std::vector<container_item_type>;
  using container_iterator_type = typename container_type::iterator;
};

template < typename Type >
struct container_iterator_traits<Type, std::list>
{
  using self = container_iterator_traits<Type, std::vector>;
  using difference_type = std::ptrdiff_t;
  using value_type = Type;
  using pointer = Type*;
  using reference = Type&;
  using iterator_category = std::bidirectional_iterator_tag;
  using container_item_type = container_item<Type>;
  using container_type = std::list<container_item_type>;
  using container_iterator_type = typename container_type::iterator;
};

template < typename Type, template < class ... > class Container >
class container_iterator
{
public:
  using self_type = container_iterator<Type, Container>;
  using iterator_traits = container_iterator_traits<Type, Container>;
  using iterator_category = typename iterator_traits::iterator_category;
  using difference_type = typename iterator_traits::difference_type;
  using value_type = typename iterator_traits::value_type;
  using pointer = typename iterator_traits::pointer;
  using reference = typename iterator_traits::reference;
  using container_iterator_type = typename iterator_traits::container_iterator_type;

  explicit container_iterator(container_iterator_type iter) : iter_(std::move(iter)) {}

  reference operator*() const { return iter_->value; }
  pointer operator->() { return &iter_->value; }
  self_type& operator++() { ++iter_; return *this; }
  self_type operator++(int) { self_type tmp = *this; ++(*this); return tmp; }
  friend bool operator== (const self_type& a, const self_type& b) { return a.iter_ == b.iter_; };
  friend bool operator!= (const self_type& a, const self_type& b) { return a.iter_ != b.iter_; };

private:
  container_iterator_type iter_;
};


template < typename Type, template < class ... > class Container >
class container
{
public:
  using iterator = container_iterator<Type, Container>;
  using container_type = typename iterator::iterator_traits::container_type;

  template < typename U = Type >
  explicit container(size_t size) : size_(size)
  {
    static_assert(std::is_same<U, std::string>::value, "Size is only needed for std::string");
  }

  template < typename U = Type >
  container()
  {
    static_assert(!std::is_same<U, std::string>::value, "std::string needs a size");
  }

  iterator begin() { return iterator(container_.begin()); }
  iterator end()   { return iterator(container_.end()); }

  template < template < class ... > class C = Container >
  void push_front(const Type &value) {
    static_assert(!std::is_same<C<container_item<Type>>, std::vector<container_item<Type>>>::value, "No push_front in vector");
    container_.push_front({value, size_});
  }

  void push_back(const Type &value) {
    container_.push_back({value, size_});
  }

private:
  size_t size_{};
  container_type container_;
};

int main()
{
  container<std::string, std::vector> c1(255);
  c1.push_back("hallo");
  c1.push_back("welt");

  for (const auto& item : c1) {
    std::cout << "item " << item << "\n";
  }

  container<long, std::list> c2;

//  c1.push_front("hallo"); won't compile
  c2.push_front(7);
  c2.push_front(8);
  c2.push_back(9);

  for (const auto& item : c2) {
    std::cout << "item " << item << "\n";
  }

}