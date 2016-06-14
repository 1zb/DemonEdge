// http://codereview.stackexchange.com/questions/36251/c-delegate-implementation-with-member-functions

#ifndef _DELEGATE_HPP_
#define _DELEGATE_HPP_

#include <functional>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <iostream>

namespace dota {

template <typename Object>
class Delegate {
 private:
  class Base {
   public:
    virtual ~Base() {}
  };

  template <typename... BindArgs>
  class Derived : public Base {
   private:
    using Handler = std::function<void(BindArgs...)>;
    Handler handler_;
   public:
    explicit Derived(Handler handler) : handler_(handler) {}

    void Call(BindArgs&&... args) {
      handler_(args...);
    }
  };

  using Vector = std::vector<std::unique_ptr<Base>>;
  using Map = std::map<Object, Vector>;

  Map functions_;

 public:
  using Cookie = typename Vector::const_iterator;

  template <typename... BindArgs>
  void Connect(Object obj, std::function<void(BindArgs...)>&& func) {
    functions_[obj].emplace_back(new Derived<BindArgs...>(func));
    // std::cout << functions_.at(obj).size() << std::endl;
  }

  template <typename... Args>
  void Call(Object obj, Args&&... args) {
    for (auto&& handler : functions_[obj]) {
      auto func = static_cast<Derived<Args...>*>(handler.get());
      func->Call(std::forward<Args>(args)...);
    }
  }
};
}  // namespace dota
#endif  // SRC_DEMONEDGE_DELEGATE_HPP_
