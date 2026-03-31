// Implementation of sjtu::any_ptr per assignment spec.
#ifndef SRC_HPP
#define SRC_HPP

#include <stdexcept>
#include <initializer_list>

namespace sjtu {

class any_ptr {
 private:
  struct control_block {
    void *ptr;
    void (*deleter)(void *);
    const void *type_tag;
    std::size_t refcnt;
  };

  control_block *ctrl_ = nullptr;

  template <class T>
  static const void *type_tag_of() {
    static int tag;
    return &tag;
  }

  void retain() noexcept {
    if (ctrl_) ++ctrl_->refcnt;
  }

  void release() noexcept {
    if (!ctrl_) return;
    if (--ctrl_->refcnt == 0) {
      if (ctrl_->deleter && ctrl_->ptr) ctrl_->deleter(ctrl_->ptr);
      delete ctrl_;
    }
    ctrl_ = nullptr;
  }

 public:
  any_ptr() {}

  any_ptr(const any_ptr &other) : ctrl_(other.ctrl_) { retain(); }

  template <class T>
  explicit any_ptr(T *ptr) {
    if (ptr) {
      ctrl_ = new control_block{
          ptr,
          [](void *p) {
            delete static_cast<T *>(p);
          },
          type_tag_of<T>(),
          1};
    }
  }

  ~any_ptr() { release(); }

  any_ptr &operator=(const any_ptr &other) {
    if (this == &other) return *this;
    // Retain new first to handle self-assignment chains safely
    control_block *new_ctrl = other.ctrl_;
    if (new_ctrl) ++new_ctrl->refcnt;
    release();
    ctrl_ = new_ctrl;
    return *this;
  }

  template <class T>
  any_ptr &operator=(T *ptr) {
    // Take ownership of raw pointer
    release();
    if (ptr) {
      ctrl_ = new control_block{
          ptr,
          [](void *p) {
            delete static_cast<T *>(p);
          },
          type_tag_of<T>(),
          1};
    }
    return *this;
  }

  template <class T>
  T &unwrap() {
    if (!ctrl_ || ctrl_->type_tag != type_tag_of<T>()) throw std::bad_cast();
    return *static_cast<T *>(ctrl_->ptr);
  }

  template <class T>
  const T &unwrap() const {
    if (!ctrl_ || ctrl_->type_tag != type_tag_of<T>()) throw std::bad_cast();
    return *static_cast<const T *>(ctrl_->ptr);
  }
};

template <class T>
any_ptr make_any_ptr(const T &t) {
  return any_ptr(new T(t));
}

template <class T, class... Args>
any_ptr make_any_ptr(Args &&...args) {
  return any_ptr(new T(std::forward<Args>(args)...));
}

template <class T>
any_ptr make_any_ptr(std::initializer_list<typename T::value_type> il) {
  return any_ptr(new T(il));
}

}  // namespace sjtu

#endif

