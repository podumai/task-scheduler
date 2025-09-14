#include <pthread.h>
#include <signal.h>
#include <ucontext.h>
#include <unistd.h>

#include <barrier>
#include <boost/asio.hpp>
#include <boost/coroutine2/all.hpp>
#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <boost/smart_ptr/make_local_shared_array.hpp>
#include <boost/type_index.hpp>
#include <chrono>
#include <coroutine>
#include <engine/task_scheduler/task_scheduler.hpp>
#include <exception>
#include <future>
#include <iostream>
#include <iterator>
#include <list>
#include <mutex>
#include <optional>
#include <print>
#include <ranges>
#include <semaphore>
#include <span>
#include <thread>

std::barrier bar{2};

auto main(
  [[maybe_unused]] int argc,  //
  [[maybe_unused]] char* argv[]
) -> int
{
  auto scheduler{engine::TaskScheduler::Clone()};
  scheduler->PushTask(
    [](boost::context::fiber&& f) -> boost::context::fiber&&
    {
      for (int i{}; i < 10; ++i)
      {
        std::println("Fiber[1:{}]: Hello, world!", i);
        f = std::move(f).resume();
      }
      return std::move(f);
    }
  );
  scheduler->PushTask(
    [](boost::context::fiber&& f) -> boost::context::fiber&&
    {
      for (int i{}; i < 10; ++i)
      {
        std::println("Fiber[2:{}]: Hello, world!", i);
        f = std::move(f).resume();
      }
      return std::move(f);
    }
  );
  scheduler->PushTask(
    [](boost::context::fiber&& f) -> boost::context::fiber&&
    {
      for (int i{}; i < 10; ++i)
      {
        std::println("Fiber[3:{}]: Hello, world!", i);
        f = std::move(f).resume();
      }
      return std::move(f);
    }
  );
  std::this_thread::sleep_for(std::chrono::nanoseconds(20'500));
  return 0;
}
