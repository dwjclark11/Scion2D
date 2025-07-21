#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <atomic>

namespace SCION_UTIL
{
class ThreadPool
{
  public:
	explicit ThreadPool( size_t threadCount = std::thread::hardware_concurrency() )
		: m_bStopped{ false }
	{
		for ( size_t i = 0; i < threadCount; ++i )
		{
			// Launch each worker thread
			m_Workers.emplace_back( [ this ] {
				while ( true )
				{
					std::function<void()> task;

					{
						// acquire lock to access the task queue
						std::unique_lock lock( m_QueueMutex );
						// Wait until there's a task or shutdown signal
						m_Condition.wait( lock, [ this ] { return m_bStopped || !m_Tasks.empty(); } );

						// If shutdown and no tasks remain, exit the thread
						if ( m_bStopped && m_Tasks.empty() )
							return;
						// fetch the next task from the queue
						task = std::move( m_Tasks.front() );

						// Then we pop the task off of the queue.
						m_Tasks.pop();
					}
					// Execute the task outside the lock.
					task();
				}
			} );
		}
	}

	// Make thread pool non-copyable
	ThreadPool( const ThreadPool& ) = delete;
	ThreadPool& operator=( const ThreadPool& ) = delete;

	// DTOR: Signals shutdown and joins all threads
	~ThreadPool()
	{
		{
			// Acquire lock before setting the stop flag
			std::lock_guard lock( m_QueueMutex );
			m_bStopped = true;
		}

		// Wake up all worker threads so they can exit
		m_Condition.notify_all();

		// Join all threads to ensure a clean shutdown
		for ( auto& thread : m_Workers )
		{
			if ( thread.joinable() )
				thread.join();
		}
	}

	// Enqueue a task with optional arguments, returns a future to get the result.
	template <typename Func, typename... Args>
	auto Enqueue( Func&& func, Args&&... args ) -> std::future<std::invoke_result_t<Func, Args...>>
	{
		using ReturnType = std::invoke_result_t<Func, Args...>;

		// Wrap the function and its arguments in a packaged task
		auto taskPtr = std::make_shared<std::packaged_task<ReturnType()>>(
			std::bind( std::forward<Func>( func ), std::forward<Args>( args )... ) );

		{
			// Lock before adding task to queue
			std::lock_guard lock( m_QueueMutex );

			// Don't allow enqueing if the pool is stopped
			if ( m_bStopped )
			{
				throw std::runtime_error( "Thread Pool is stopped" );
			}

			// Push the task into the queue, wrapped in a void() lambda
			m_Tasks.emplace( [ taskPtr ]() { (*taskPtr)(); } );
		}

		// Notify one thread that a new task is available
		m_Condition.notify_one();

		// return the future so the caller can wait for the result.
		return taskPtr->get_future();
	}

  private:
	/* Worker thread pool. */
	std::vector<std::thread> m_Workers;
	/* Queue of pending tasks. */
	std::queue<std::function<void()>> m_Tasks;
	/* Mutex for protecting the task queue. */
	std::mutex m_QueueMutex;
	/* Used to notify worker threads of new tasks. */
	std::condition_variable m_Condition;
	/* Indicates if the pool is shutting down. */
	std::atomic<bool> m_bStopped;
};
} // namespace SCION_UTIL

using SharedThreadPool = std::shared_ptr<SCION_UTIL::ThreadPool>;
