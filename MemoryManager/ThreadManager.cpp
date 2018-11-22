#include "ThreadManager.h"

/*
	Private Functions
*/

void ThreadManager::threadEntry(unsigned int threadID)
{
	// Continously run the worker thread
	while (1) {
		
		std::function<void(void)> job;
		
		{ // Mutex locked scope
			std::unique_lock<std::mutex> lock(m_mutex);
			
			m_cond.wait(lock);

			if (m_shutdown && m_jobs.empty())
				return;

			// Get the next job and then pop it
			job = std::move(m_jobs.front());
			m_jobs.pop();
		}
		// Do the job without lock
		job();
	}
}


/*
	Public Functions
*/

ThreadManager::ThreadManager()
{
}

ThreadManager::~ThreadManager()
{
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_shutdown = true;
		// Notify all threads to let them shut down
		m_cond.notify_all();
	}

	for (auto& t : m_threads)
		t.join();

}

void ThreadManager::init(unsigned int numThreads)
{
	for (unsigned int i = 0; i < numThreads; i++) {
		m_threads.emplace_back(std::bind(&ThreadManager::threadEntry, this, i));
		m_threadIDs.emplace_back(m_threads.back().get_id());
	}

	m_shutdown = false;
	m_initialized = true;
}

int ThreadManager::getThreadID(std::thread::id threadID)
{
	for (unsigned int i = 0; i < m_threadIDs.size(); i++)
		if (m_threadIDs[i] == threadID) return i;
	return -1;
}

std::vector<std::thread::id> ThreadManager::getThreadIDs()
{
	return m_threadIDs;
}

void ThreadManager::appendJob(std::function<void(void)> func)
{
	std::unique_lock<std::mutex> lock(m_mutex);

	m_jobs.emplace(func);
	// Notifies one thread and lets it do it do a job
	m_cond.notify_one();
}

size_t ThreadManager::getNumThreads() {
	return m_threads.size();
}