//
// Created by Роман Агеев on 2019-06-18.
//

#ifndef VECTOR_VECTOR_HPP
#define VECTOR_VECTOR_HPP

#include <memory>
#include <variant>

template<typename T>
class vector {
 private:
  struct helper {
    size_t capacity = 0;
    size_t size = 0;
    size_t counter = 0;
    T *get_ptr() {
      return reinterpret_cast<T *>(&counter + 1);
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
    return tmp;
  }

  template<typename ...Args>
  void construct(T *pointer, Args const &... args) {
    new(pointer) T(args...);
  }

  void construct(T *pointer, T *copy) {
    assert(copy != nullptr);
    new(pointer) T(*copy);
  }

  void destruct(T *pointer) {
    pointer->~T();
  }

  void dealloc(helper *in) {
    operator delete(static_cast<void *>(in));
  }
  // typedef std::allocator_traits<Allocator> allocator_traits;
  // typedef typename allocator_traits::template rebind_alloc<uint8_t> rebinded_allocator;
  typedef std::variant<helper *, T> variant;

  variant data_;

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

  T *get_data_ptr() const noexcept {
    assert(get_helper() != nullptr);
    return get_helper()->get_ptr();
  }

  void detach() {
    if ((data_.index() == 0) && (get_helper() != nullptr) && (get_helper()->counter > 0)) {
      helper *new_data = alloc(get_capacity());
      size_t i = 0;
      try {
        for (; i < get_size(); ++i) {
          construct(new_data->get_ptr() + i, get_data_ptr() + i);
        }
      } catch (...) {
        for (size_t j = 0; j < i; j++) {
          destruct(new_data->get_ptr() + j);
        }
        dealloc(new_data);
        throw;
      }
      --get_helper()->counter;
      new_data->size = get_size();
      new_data->capacity = get_capacity();
      data_ = new_data;
    }
  }

  void push_back_long(T const &in) {
    if (get_size() < get_capacity()) {
      construct(get_data_ptr() + get_size(), in);
      ++get_helper()->size;
    } else {

    }
  }
 public:
  // typedef std::reverse_iterator<iterator> reverse_iterator;
  // typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  static_assert((sizeof(data_)) <= (sizeof(void *) + std::max(sizeof(T), sizeof(void *))));

  vector() noexcept = default;

  ~vector() noexcept {
    if (!small()) {
      if (get_helper()->counter == 0) {
        for (size_t j = 0; j < get_size(); ++j) {
          destruct(get_data_ptr() + j);
        }
        dealloc(get_helper());
      } else {
        --get_helper()->counter;
      }
    }
  }

  bool small() const noexcept {
    return (data_.index() == 1) || (get_helper() == nullptr);
  }

  bool empty() const noexcept {
    return ((data_.index() == 0) && ((get_helper() == nullptr) || (get_helper()->size == 0)));
  }

  size_t size() const noexcept {
    if (data_.index() == 0) {
      return get_helper() == nullptr ? 0 : get_size();
    }
    return 1;
  }

  void push_back(T const &in) {
    detach();
    if (empty()) {
      data_ = in;
    } else {
      if (small()) {
        helper *new_data = alloc(10000);
        new_data->size = 2;
        try {
          construct(new_data->get_ptr(), std::get<1>(data_));
        } catch (...) {
          dealloc(new_data);
          throw;
        }
        try {
          construct(new_data->get_ptr() + 1, in);
        } catch (...) {
          destruct(new_data->get_ptr());
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

  void clear() {

  }

  void erase(...) {

  }

  void insert(...) {

  }

  bool unique() const noexcept {
    return data_.index() == 1 || (get_helper() == nullptr || get_helper()->counter == 0);
  }

  T* begin() const {
    return get_data_ptr();
  }

  size_t use_count() {
    return small() ? 0 : get_helper()->counter;
  }

  T* end() const {
    return get_data_ptr() + size();
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

  void pop_back() {
    assert(size() > 0);
    if (small()) {
      data_ = nullptr;
    } else {
      destruct(get_data_ptr() + size() - 1);
      --get_helper()->size;
    }
  }

  vector(vector const &in) : vector() {
    if (&in != this) {
      if (in.small()) {
        if (!in.empty()) {
          push_back(in[0]);
        }
      } else {
        data_ = in.get_helper();
        ++get_helper()->counter;
      }
    }
  }
};

#endif //VECTOR_VECTOR_HPP
