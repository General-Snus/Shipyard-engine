#pragma once
#define  NOMINMAX
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread> 
#include "SingletonTemplate.h" 

class ThreadPool : public Singleton<ThreadPool>
{
	friend class ThreadWorker;
	friend class Singleton<ThreadPool>;
public:
	void Init(unsigned int pool_size = std::thread::hardware_concurrency() - 1)
	{
		static const unsigned int max_threads = std::thread::hardware_concurrency();
		numThreads = pool_size == 0 ? max_threads - 1 : (std::min)(max_threads - 1,pool_size);

		threads.reserve(numThreads);
		for (unsigned int i = 0; i < numThreads; ++i)
		{
			threads.emplace_back(std::bind(ThreadWorker(this)));
		}
	}

	void Destroy()
	{
		{
			std::lock_guard<std::mutex> lk(conditional_mutex);
			ShouldClose = true;
			conditional_var.notify_all();
		}

		for (auto& thread : threads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}
	}

	ThreadPool(ThreadPool const&) = delete;
	ThreadPool(ThreadPool&&) = delete;
	ThreadPool& operator=(ThreadPool const&) = delete;
	ThreadPool& operator=(ThreadPool&&) = delete;
	~ThreadPool() { Destroy(); };

	template<typename F,typename... Args>
	auto SubmitWork(F&& f,Args&&... args) -> std::future<decltype(f(args...))>
	{
		std::packaged_task<decltype(f(args...))()> task(std::forward<F>(f),std::forward<Args>(args)...);

		auto future = task.get_future();
		auto wrap =
			[task = std::move(task)]() mutable
			{
				task();
			};

		{
			std::lock_guard<std::mutex> lock(conditional_mutex);
			workerQueue.push(std::move(wrap));
			conditional_var.notify_one();
		}

		return future;
	}

	int QueueSize()
	{
		std::lock_guard<std::mutex> lock(conditional_mutex);
		return static_cast<unsigned int>(workerQueue.size());
	}


	bool ShouldClose{};
private:
	std::vector<std::thread> threads;
	ThreadPool() = default;
	std::queue<std::move_only_function<void()>> workerQueue;
	std::condition_variable conditional_var;
	std::mutex conditional_mutex;
	unsigned numThreads{};
	unsigned activeThreads = 0;

	class ThreadWorker
	{
	public:
		ThreadWorker(ThreadPool* pool) : pool(pool) {}
		ThreadPool* pool;

		void operator()()
		{
			std::unique_lock<std::mutex> lock(pool->conditional_mutex);
			while (!pool->ShouldClose || (!pool->workerQueue.empty() && pool->ShouldClose))
			{
				pool->activeThreads--;
				pool->conditional_var.wait(lock,[this]
					{
						return pool->ShouldClose || !pool->workerQueue.empty();
					});
				pool->activeThreads++;

				if (!pool->workerQueue.empty())
				{
					auto func = std::move(pool->workerQueue.front());
					pool->workerQueue.pop();
					lock.unlock();
					func();
					lock.lock();
				}
			}
		}
	};
};