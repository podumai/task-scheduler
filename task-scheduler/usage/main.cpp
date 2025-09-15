#include <engine/task_scheduler/task_scheduler.hpp>
#include <future>
#include <print>
#include <thread>

auto main(
  [[maybe_unused]] int argc,  //
  [[maybe_unused]] char* argv[]
) -> int
{
  std::promise<int> p;
  std::future<int> f{p.get_future()};
  auto scheduler{engine::TaskScheduler::Clone()};
  scheduler->PushTask(
    [p = std::move(p)](boost::context::fiber&& f) mutable -> boost::context::fiber&&
    {
      int value{};
      for (int i{}; i < 10; ++i)
      {
        ++value;
        f = std::move(f).resume();
      }
      p.set_value(value);
      return std::move(f);
    }
  );
  std::println("Value:{}", f.get());
  return 0;
}
