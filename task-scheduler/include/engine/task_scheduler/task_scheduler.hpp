/**
 * @file task_scheduler.hpp
 * @author podumai kirillsm05@gmail.com
 * @details This is only the test example that I wrote to
 *          examine the user-level threads scheduling
 *          (very simple approach) using "first come, first served" strategy.
 */

#pragma once

#include <boost/context/fiber.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <memory>
#include <mutex>
#include <thread>

/**
 * @namespace engine
 */
namespace engine
{

/**
 * @class TaskScheduler
 * @brief Class that schedules fibers.
 * @details TaskScheduler provides "first come, first served" scheduling policy.
 */
class TaskScheduler final
{
 private:
  /**
   * @private
   * @brief Default constructor for TaskScheduler.
   * @details Constructor initalizes task queue and
   *          makes worker thread that executes tasks
   *          in iterative order.
   */
  TaskScheduler() : tasks_{1000}
  { }

 public:
  TaskScheduler(const TaskScheduler&) = delete ("Only one instance of TaskSchedular can be created.");
  TaskScheduler(TaskScheduler&&) = delete ("Only one instance of TaskSchedular can be created.");
  auto operator=(const TaskScheduler&) -> TaskScheduler = delete ("Only one instance of TaskSchedular can be created.");
  auto operator=(TaskScheduler&&) -> TaskScheduler& = delete ("Only one instance of TaskSchedular can be created.");

  /**
   * @public
   * @brief Get TaskScheduler instance.
   * @details Clone is fully thread safe.
   *          It manages the creation of TaskScheduler object
   *          and provides an instance to manipulate with.
   *
   * @return const std::shared_ptr<TaskScheduler>
   */
  [[nodiscard]] static auto Clone() -> const std::shared_ptr<TaskScheduler>
  {
    std::lock_guard lg{init_mutex_};
    static std::shared_ptr<TaskScheduler> scheduler;
    std::call_once(
      init_flag_,
      [](std::shared_ptr<TaskScheduler>& scheduler) -> void
      {
        scheduler.reset(new TaskScheduler);
        std::jthread{
          [scheduler](std::stop_token token) -> void
          {
            while (true)
            {
              if (token.stop_requested())
              {
                return;
              }
              std::shared_ptr<fiber> task;
              if (scheduler->tasks_.pop(task) == false)
              {
                std::this_thread::yield();
                continue;
              }
              if (!*task)
              {
                std::this_thread::yield();
                continue;
              }
              *task = std::move(*task).resume();
              scheduler->tasks_.push(task);
            }
          }
        }.detach();
      },
      scheduler
    );
    return scheduler;
  }

  /**
   * @public
   * @brief Posts task to TaskScheduler queue.
   *
   * @tparam Function Generic function type that satisfies boost::context::fiber signature.
   * @param[in] f Function that will be executed in new fiber.
   * 
   * @throws std::runtime_error if push operation failed.
   */
  template<typename Function>
  auto PushTask(
    Function&& f
  ) -> void
  {
    auto task{std::make_shared<boost::context::fiber>(std::forward<Function>(f))};
    if (tasks_.push(task) == false)
    {
      throw std::runtime_error{"TaskScheduler::PushTask() -> task queue limit"};
    }
  }

 private:
  using fiber = typename boost::context::fiber;

 private:
  static inline std::once_flag init_flag_;
  static inline std::mutex init_mutex_;
  boost::lockfree::spsc_queue<std::shared_ptr<fiber>> tasks_;
};

}  // namespace engine
