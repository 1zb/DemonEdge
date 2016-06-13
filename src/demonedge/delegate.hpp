// http://codereview.stackexchange.com/questions/36251/c-delegate-implementation-with-member-functions

#ifndef _DELEGATE_HPP_
#define _DELEGATE_HPP_

#include <functional>
#include <vector>
// #include <map>
#include <unordered_map>

namespace dota {

template <typename Object, typename... Params>
class Delegate {
 private:
  // typedef std::function<void(Params...)> Handler;
  using Handler = std::function<void(Params...)>;
  // typedef std::vector<Handler> Vector;
  using Vector = std::vector<Handler>;
  using Map = std::unordered_map<Object, Vector>;
  // Vector functions_;
  Map functions_;
 public:
  // typedef typename Vector::const_iterator cookie;
  using Cookie = typename Vector::const_iterator;

  // cookie connect(Handler&& func) {
  //   functions_.push_back(std::move(func));
  //   return functions_.begin() + functions_.size() - 1;
  // }
  Cookie Connect(Object obj, Handler&& func) {
    functions_.at(obj).push_back(std::move(func));
    return functions_.at(obj).begin() + functions_.at(obj).size() - 1;
  }

  template <typename... BindArgs, typename Sfinae = typename std::enable_if<(sizeof...(BindArgs)>1), void>::type>
  Cookie Connect(Object obj, BindArgs&&... args) {
    return Connect(obj, Handler(std::bind(std::forward<BindArgs>(args)...)));
  }

  // void disconnect(Cookie which) {
  //   functions_.erase(which);
  // }

  // template <typename ... Args> void operator()(Args...args) {
  //   for(auto const& handler : functions_)
  //     handler(args...);
  // }
  template <typename... Args>
  void Call(Object obj, Args... args) {
    for (auto const& handler : functions_.at(obj))
      handler(args...);
  }
};
}
#endif