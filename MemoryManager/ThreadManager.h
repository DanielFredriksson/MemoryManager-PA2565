#ifndef THREAD_MANAGER
#define THREAD_MANAGER

#include <thread>
#include <condition_variable>
#include <vector>
#include <queue>
#include <mutex>

class ThreadManager 
{
private:
	// The threads doing jobs
	std::vector<std::thread> m_threads;
	std::vector<std::thread::id> m_threadIDs;

	std::condition_variable m_cond;

	bool m_initialized = false;
	bool m_shutdown = false;

	std::queue<std::function<void(void)>> m_jobs;

	std::mutex m_mutex;

private:
	void threadEntry(unsigned int threadID);

public:
	ThreadManager();
	~ThreadManager();

	static ThreadManager& getInstance() 
	{
		static ThreadManager instance;

		return instance;
	}

	void init(unsigned int numThreads = 1);
	int getThreadID(std::thread::id thread);
	void appendJob(std::function<void(void)> func);

	size_t getNumThreads();


};

#endif //THREAD_MANAGER
