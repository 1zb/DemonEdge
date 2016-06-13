#ifndef _CALLBACK_HPP_
#define _CALLBACK_HPP_

#include <functional>
#include <map>
#include <vector>
#include <utility>
#include <iostream>

#include <google/protobuf/message.h>

#include "delegate.hpp"

namespace dota {

enum class Event {
  OnCDemoFileHeader,
  OnCDemoFileInfo,
};


class Callbacks {
 private:
 public:
  
}
// struct Function {
//   virtual ~Function() = default;
// };

// template <typename BindArg, typename Object=void>
// struct Callback : public Function {
//   using Handler = std::function<void(const Object&, BindArg)>;
//   Handler handler_;
//   Callback(Handler handler) : handler_(handler) {}
// };

// template <typename BindArg> 
// struct Callback<BindArg, void> : public Function {
//   using Handler = std::function<void(BindArg)>;
//   Handler handler_;
//   Callback(Handler handler) : handler_(handler) {}
// };

// class Callbacks {
//  private:

//   using FunctionPointer = std::unique_ptr<Function>;
//   using Map = std::map<Event, std::vector<FunctionPointer>>;

//   Map callbacks_;
//  public:
//   Callbacks() = default;

//   void Connect(const Event& e, FunctionPointer&& fp) {
//     callbacks_[e].push_back(std::move(fp));
//   }

//   template<typename... Args>
//   void Call(const Event& e, Args&&... args) const {
//     for (auto const& fp : callbacks_.at(e)) {
//       const Function& base = *fp;
//       const std::function<void(Args...)>& fun = static_cast<const Callback<Args...>&>(base).handler_;
//       fun(std::forward<Args>(args)...);
//     }
//   }
// };
// template <typename MessageType>
// class Callback {
//  public:
//   using Handler = std::function<void(MessageType)>;
//   using Vector = std::vector<Handler>;
//   using Cookie = typename Vector::const_iterator;

//   Callback() = default;

//   Cookie connect(Handler&& func) {
//     functions_.push_back(std::move(func));
//     return functions_.begin() + functions_.size() - 1;
//   }

//   template <typename... BindArgs, typename Sfinae = typename std::enable_if<(sizeof...(BindArgs)>1), void>::type>
//   Cookie connect(BindArgs&&... args) {
//     return connect(Handler(std::bind(std::forward<BindArgs>(args)...)));
//   }

//   void disconnect(Cookie which) {
//     functions_.erase(which);
//   }
//  private:
//   Vector functions_;
// };

}
#endif