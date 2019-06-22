//
// Created by Роман Агеев on 2019-06-18.
//

#ifndef VECTOR_VECTOR_HPP
#define VECTOR_VECTOR_HPP

#include <memory>
#include <variant>
template<typename T>
struct vector_iterator {
 private:
  template<typename> friend struct vector;
  template<typename> friend struct const_vector_iterator;
  explicit vector_iterator(T* in) : pointer_(in) {}

  T* pointer_ = nullptr;

 public:
  typedef std::random_access_iterator_tag iterator_category;
  typedef size_t difference_type;
  typedef T value_type;
  typedef T* pointer;
  typedef T& reference;

  vector_iterator() = default;
  vector_iterator(vector_iterator const&) = default;
  vector_iterator& operator=(vector_iterator const&) = default;

  reference operator[](size_t i) {
    return pointer_[i];
  }

  reference operator*() const {
    return *pointer_;
  }

  pointer operator->() const {
    return pointer_;
  }

  vector_iterator& operator++() {
    ++pointer_;
    return *this;
  }

  const vector_iterator operator++(int) {
    vector_iterator tmp(*this);
    ++*this;
    return tmp;
  }

  vector_iterator& operator--() {
    --pointer_;
    return *this;
  }

  const vector_iterator operator--(int) {
    vector_iterator tmp(*this);
    --*this;
    return tmp;
  }

  vector_iterator& operator+=(size_t i) {
    pointer_ += i;
    return *this;
  }

  vector_iterator& operator-=(size_t i) {
    pointer_ -= i;
    return *this;
  }

  friend difference_type operator-(vector_iterator const& in1, vector_iterator const& in2) {
    return in1.pointer_ - in2.pointer_;
  }

  friend vector_iterator operator+(vector_iterator in, size_t i) {
    in += i;
    return in;
  }

  friend vector_iterator operator-(vector_iterator in, size_t i) {
    in -= i;
    return in;
  }

  friend vector_iterator operator+(size_t i, vector_iterator in) {
    in += i;
    return in;
  }

  friend bool operator<(vector_iterator const& in1, vector_iterator const& in2) {
    return in1.pointer_ < in2.pointer_;
  }
  friend bool operator>(vector_iterator const& in1, vector_iterator const& in2) {
    return in2 < in1;
  }
  friend bool operator<=(vector_iterator const& in1, vector_iterator const& in2) {
    return (in1 < in2) || (in1 == in2);
  }
  friend bool operator>=(vector_iterator const& in1, vector_iterator const& in2) {
    return (in1 > in2) || (in1 == in2);
  }
  friend bool operator==(vector_iterator const& in1, vector_iterator const& in2) {
    return in1.pointer_ == in2.pointer_;
  }
  friend bool operator!=(vector_iterator const& in1, vector_iterator const& in2) {
    return !(in1 == in2);
  }
};

template<typename T>
struct const_vector_iterator {
 private:
  template<typename> friend class vector;

  explicit const_vector_iterator(T const * p) : pointer_(p) {}

  T const * pointer_ = nullptr;
 public:
  typedef std::random_access_iterator_tag iterator_category;
  typedef size_t difference_type;
  typedef T value_type;
  typedef T const * pointer;
  typedef T const & reference;
  const_vector_iterator() = default;
  const_vector_iterator(const_vector_iterator const&) = default;
  const_vector_iterator(vector_iterator<T> const&in) : pointer_(in.pointer_) {}
  const_vector_iterator& operator=(const_vector_iterator const&) = default;

  reference operator[](size_t i) {
    return pointer_[i];
  }

  reference operator*() const {
    return *pointer_;
  }

  pointer operator->() const {
    return pointer_;
  }

  const_vector_iterator& operator++() {
    ++pointer_;
    return *this;
  }

  const const_vector_iterator operator++(int) {
    vector_iterator tmp(*this);
    ++*this;
    return tmp;
  }

  const_vector_iterator& operator--() {
    --pointer_;
    return *this;
  }

  const const_vector_iterator operator--(int) {
    const_vector_iterator tmp(*this);
    --*this;
    return tmp;
  }

  const_vector_iterator& operator+=(size_t i) {
    pointer_ += i;
    return *this;
  }

  const_vector_iterator& operator-=(size_t i) {
    pointer_ -= i;
    return *this;
  }

  friend difference_type operator-(const_vector_iterator const& in1, const_vector_iterator const& in2) {
    return in1.pointer_ - in2.pointer_;
  }

  friend const_vector_iterator operator+(const_vector_iterator in, size_t i) {
    in += i;
    return in;
  }

  friend const_vector_iterator operator-(const_vector_iterator in, size_t i) {
    in -= i;
    return in;
  }

  friend const_vector_iterator operator+(size_t i, const_vector_iterator in) {
    in += i;
    return in;
  }

  friend bool operator<(const_vector_iterator const& in1, const_vector_iterator const& in2) {
    return in1.pointer_ < in2.pointer_;
  }
  friend bool operator>(const_vector_iterator const& in1, const_vector_iterator const& in2) {
    return in2 < in1;
  }
  friend bool operator<=(const_vector_iterator const& in1, const_vector_iterator const& in2) {
    return (in1 < in2) || (in1 == in2);
  }
  friend bool operator>=(const_vector_iterator const& in1, const_vector_iterator const& in2) {
    return (in1 > in2) || (in1 == in2);
  }
  friend bool operator==(const_vector_iterator const& in1, const_vector_iterator const& in2) {
    return in1.pointer_ == in2.pointer_;
  }
  friend bool operator!=(const_vector_iterator const& in1, const_vector_iterator const& in2) {
    return !(in1 == in2);
  }
};

template<typename T>
class vector {
 private:
  struct helper {
    size_t capacity = 0;
    size_t size = 0;
    size_t counter = 0;
    T * ptr = nullptr;
    T *get_ptr() {
      return reinterpret_cast<T *>(&ptr + 1);
    }
  };

  void *alloc_inside(size_t n) {
    return operator new(n);
  }

  helper *alloc(size_t n) {
    auto tmp = static_cast<helper *>(alloc_inside(sizeof(helper) + sizeof(T) * n));
    tmp->capacity = n;
    tmp->size = 0;
    tmp->counter = 0;
    tmp->ptr = tmp->get_ptr();
    return tmp;
  }

  template<typename ...Args>
  void construct(T *pointer, Args const &... args) {
    assert(pointer != nullptr);
    new(pointer) T(args...);
  }

  void construct(T *pointer, T *copy) {
    assert(pointer != nullptr);
    assert(copy != nullptr);
    new(pointer) T(*copy);
  }

  void destruct(T *pointer) noexcept {
    pointer->~T();
  }

  helper * big_safe_copy(size_t n) {
    helper *new_data = alloc(n);
    size_t i = 0;
    try {
      for (; i < std::min(get_size(), n); ++i) {
        construct(new_data->get_ptr() + i, get_data_ptr() + i);
      }
    } catch (...) {
      big_safe_destruct(new_data->get_ptr(), i);
      dealloc(new_data);
      throw;
    }
    new_data->size = get_size();
    return new_data;
  }

  void big_safe_destruct(T* ptr, size_t n) noexcept {
    for (size_t j = 0; j < n; j++) {
      destruct(ptr + j);
    }
  }

  void dealloc(helper *in) {
    operator delete(static_cast<void *>(in));
  }

  void forget_helper() {
    assert(data_.index() == 0);
    big_safe_destruct(get_data_ptr(), get_size());
    dealloc(std::get<0>(data_));
    std::get<0>(data_) = nullptr;
  }

  std::variant<helper *, T> data_;

  helper *get_helper() const noexcept {
    assert(data_.index() == 0);
    return std::get<0>(data_);
  }

  size_t get_capacity() const noexcept {
    assert(get_helper() != nullptr);
    return get_helper()->capacity;
  }

  size_t get_size() const noexcept {
    assert(get_helper() != nullptr);
    return get_helper()->size;
  }

  size_t get_counter() const noexcept {
    assert(get_helper() != nullptr);
    return get_helper()->counter;
  }

  T *get_data_ptr() const noexcept {
    assert(get_helper() != nullptr);
    return get_helper()->ptr;
  }

  void detach() {
    if ((data_.index() == 0) && (get_helper() != nullptr) && (get_helper()->counter > 0)) {
      helper* new_data = big_safe_copy(get_capacity());
      --get_helper()->counter;
      data_ = new_data;
    }
  }

  void push_back_long(T const &in) {
    if (get_size() >= get_capacity()) {
      helper* new_helper = big_safe_copy(get_capacity() * 2);
      try {
        construct(new_helper->get_ptr() + get_size(), in);
      } catch (...) {
        big_safe_destruct(new_helper->get_ptr(), get_size());
        dealloc(new_helper);
        throw;
      }
      forget_helper();
      data_ = new_helper;
    } else {
      construct(get_data_ptr() + get_size(), in);
    }
    ++get_helper()->size;
  }

 public:
  typedef vector_iterator<T>       iterator;
  typedef const_vector_iterator<T> const_iterator;
  typedef std::reverse_iterator<iterator>         reverse_iterator;
  typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

  static_assert((sizeof(data_)) <= (sizeof(void *) + std::max(sizeof(T), sizeof(void *))));

  vector() noexcept = default;

  template<typename I, typename std::enable_if<std::iterator_traits<I>::type, I>::type>
  vector(I i1, I i2) : vector() {
    for(;i1 != i2; ++i1) {
      push_back(*i1);
    }
  }

  ~vector() noexcept {
    clear();
  }

  bool small() const noexcept {
    return (data_.index() == 1) || (get_helper() == nullptr);
  }

  bool empty() const noexcept {
    return ((data_.index() == 0) && ((get_helper() == nullptr) || (get_size() == 0)));
  }

  size_t size() const noexcept {
    if (data_.index() == 0) {
      return get_helper() == nullptr ? 0 : get_size();
    }
    return 1;
  }

  void push_back(T const &in) {
    detach();
    if (empty() && (get_helper() == nullptr)) {
      try {
        data_ = in;
      } catch (...) {
        data_ = nullptr;
        throw;
      }
    } else {
      if (small()) {
        helper *new_data = alloc(8);
        new_data->size = 2;
        try {
          construct(new_data->get_ptr() + 1, in);
        } catch (...) {
          dealloc(new_data);
          throw;
        }
        try {
          construct(new_data->get_ptr(), std::get<1>(data_));
        } catch (...) {
          destruct(new_data->get_ptr() + 1);
          dealloc(new_data);
          throw;
        }
        data_ = new_data;
      } else {
        push_back_long(in);
      }
    }
  }

  T &operator[](size_t i) {
    detach();
    if (small()) {
      assert(i == 0);
      return std::get<1>(data_);
    } else {
      assert(data_.index() == 0);
      assert(get_helper() != nullptr);
      assert(i < get_size());
      return *(get_data_ptr() + i);
    }
  }

  T const &operator[](size_t i) const {
    if (small()) {
      assert(i == 0);
      return std::get<1>(data_);
    } else {
      assert(data_.index() == 0);
      assert(get_helper() != nullptr);
      assert(i < get_size());
      return *(get_data_ptr() + i);
    }
  }

  void clear() {
    if (!small()) {
      if (get_counter() == 0) {
        forget_helper();
      } else {
        --get_helper()->counter;
      }
    }
    data_ = nullptr;
  }

  bool unique() const noexcept {
    return data_.index() == 1 || (get_helper() == nullptr || get_counter() == 0);
  }

  T* data() {
    detach();
    if (empty()) {
      return nullptr;
    }
    return small() ? &(std::get<1>(data_)) : get_data_ptr();
  }

  const T* data() const {
    if (empty()) {
      return nullptr;
    }
    return small() ? &(std::get<1>(data_)) : get_data_ptr();
  }

  iterator begin()  {
    detach();
    if (empty()) {
      return iterator(nullptr);
    }
    return small() ? iterator(&(std::get<1>(data_))) : iterator(get_data_ptr());
  }

  iterator end() {
    detach();
    if (empty()) {
      return iterator(nullptr);
    }
    return small() ? iterator(&(std::get<1>(data_)) + 1) : iterator(get_data_ptr() + size());
  }

  const_iterator begin() const noexcept {
    if (empty()) {
      return iterator(nullptr);
    }
    return small() ? const_iterator(&(std::get<1>(data_))) : const_iterator(get_data_ptr());
  }

  const_iterator end() const noexcept {
    if (empty()) {
      return iterator(nullptr);
    }
    return small() ? const_iterator(&(std::get<1>(data_)) + 1) : const_iterator(get_data_ptr() + size());
  }

  const_iterator cbegin() const noexcept {
    return begin();
  }

  const_iterator cend() const noexcept {
    return end();
  }

  reverse_iterator rbegin()  {
    detach();
    return reverse_iterator(end());
  }

  reverse_iterator rend() {
    return reverse_iterator(begin());
  }

  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(cend());
  }

  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }

  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

  void erase(const_iterator i1, const_iterator i2) {
    detach();
    if (i2 == end()) {
      for (;i1 != i2; --i2) {
        pop_back();
      }
    } else {
      size_t pl = i1 - cbegin();
      size_t pr = i2 - cbegin();
      size_t size = get_size() - (i2 - i1);
      vector tmp;
      tmp.reserve(size * 2 + 1);
      size_t i = 0;
      try {
        for (;i < pl ;++i) {
          tmp.push_back((*this)[i]);
        }
        i = pr;
        for (;i < get_size(); ++i) {
          tmp.push_back((*this)[i]);
        }
      } catch (...) {
        tmp.clear();
        throw;
      }
      *this = tmp;
    }
  }

  void erase(const_iterator i1) {
    erase(i1, i1 + 1);
  }

  void insert(const_iterator i, const T & in) {
    detach();
    if ((empty()) || (i == end())) {
      push_back(in);
    } else {
      vector tmp;
      tmp.reserve(size() + 1);
      auto it = cbegin();
      try {
        for (;it < i; ++it) {
          tmp.push_back(*it);
        }
        tmp.push_back(in);
        for (;it < cend(); ++it) {
          tmp.push_back(*it);
        }
      } catch (...) {
        tmp.clear();
        throw;
      }
      *this = tmp;
    }
  }

  void pop_back() {
    assert(size() > 0);
    detach();
    if (small()) {
      data_ = nullptr;
    } else {
      destruct(get_data_ptr() + size() - 1);
      --get_helper()->size;
      if(get_size() == 0) {
        clear();
      }
    }
  }

  vector(vector const &in) : vector() {
    if (in.small()) {
      if (!in.empty()) {
        push_back(in[0]);
      }
    } else {
      data_ = in.get_helper();
      ++get_helper()->counter;
    }
  }

  void reserve(size_t n) {
    if (empty()) {
      helper *new_data = alloc(8 + n);
      new_data->size = 0;
      data_ = new_data;
    } else if (small() && (n > 0)) {
      helper *new_data = alloc(8 + n);
      new_data->size = 1;
      try {
        construct(new_data->get_ptr(), std::get<1>(data_));
      } catch (...) {
        dealloc(new_data);
        throw;
      }
      data_ = new_data;
    } else if (n > 0) {
      helper *new_helper = big_safe_copy(get_capacity() + n);
      forget_helper();
      data_ = new_helper;
    }
  }

  T const & back() const noexcept {
    if (small()) {
      return std::get<1>(data_);
    } else {
      return *(get_data_ptr() + size() - 1);
    }
  }

  T & back() {
    detach();
    if (small()) {
      return std::get<1>(data_);
    } else {
      return *(get_data_ptr() + size() - 1);
    }
  }

  T const & front() const noexcept {
    if (small()) {
      return std::get<1>(data_);
    } else {
      return *(get_data_ptr());
    }
  }

  T & front() {
    detach();
    if (small()) {
      return std::get<1>(data_);
    } else {
      return *(get_data_ptr());
    }
  }

  vector & operator=(vector const &in) {
    clear();
    data_ = in.data_;
    if (!in.small()) {
      ++get_helper()->counter;
    }
    return * this;
  }

  void swap(vector &in) {
    if (&in != this) {
      bool d1 = data_.index() == 0;
      bool d2 = in.data_.index() == 0;
      if (d1 && d2) {
        std::swap(data_, in.data_);
      } else if (!d1 && d2) {
        in.swap(*this);
      } else if (d1 && !d2) {
        helper *tmp = get_helper();
        try {
          data_ = in.data_;
        } catch (...) {
          in.data_ = nullptr;
          data_ = tmp;
          throw;
        }
        in.data_ = tmp;
      } else {
        try {
          std::swap(data_, in.data_);
        } catch (...) {
          data_ = nullptr;
          in.data_ = nullptr;
          throw;
        }
      }
    }
  }
  template <typename C>
  friend bool operator==(vector<C> const&, vector<C> const &) noexcept;
  template <typename C>
  friend bool operator!=(vector<C> const&, vector<C> const &) noexcept;
  template <typename C>
  friend bool operator<=(vector<C> const&, vector<C> const &) noexcept;
  template <typename C>
  friend bool operator>=(vector<C> const&, vector<C> const &) noexcept;
  template <typename C>
  friend bool operator<(vector<C> const&, vector<C> const &) noexcept;
  template <typename C>
  friend bool operator>(vector<C> const&, vector<C> const &) noexcept;
};

template <typename C>
void swap(vector<C> &in1, vector<C> &in2) {
  in1.swap(in2);
}

template<typename C>
bool operator==(const vector<C> &in1, const vector<C> &in2) noexcept {
  if (in1.size() != in2.size()) {
    return false;
  }
  for (size_t i = 0; i < in1.size(); ++i) {
    if (in1[i] != in2[i]) {
      return false;
    }
  }
  return true;
}

template<typename C>
bool operator!=(const vector<C> &in1, const vector<C> &in2) noexcept {
  return !(in1 == in2);
}

template<typename C>
bool operator<=(const vector<C> &in1, const vector<C> &in2) noexcept {
  return (in1 < in2) || (in1 == in2);
}

template<typename C>
bool operator>=(const vector<C> &in1, const vector<C> &in2) noexcept {
  return (in1 > in2) || (in1 == in2);
}

template<typename C>
bool operator<(const vector<C> &in1, const vector<C> &in2) noexcept {
  for (size_t i = 0; i < std::min(in1.size(), in2.size()); ++i) {
    if (in1[i] != in2[i]) {
      return (in1[i] < in2[i]);
    }
  }
  return in1.size() < in2.size();
}

template<typename C>
bool operator>(const vector<C> &in1, const vector<C> &in2) noexcept {
  return in2 < in1;
}
#endif //VECTOR_VECTOR_HPP