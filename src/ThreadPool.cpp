#include "ThreadPool.hpp"
#include "MySQLConnectionPool.hpp"
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

/**
 * @brief        Constructs a ThreadPool with a given number of worker threads.
 *
 * @details      create `maxThreads` number of threads, each of which waits for
 *               and executes tasks from a shared queue. Threads will continue
 *               running until the thread pool is destroyed.
 *
 * @param[in]    maxThreads    The maximum number of worker threads to create.
 */
ThreadPool::ThreadPool(int maxThreads) : stop(false) {
    for (int i = 0; i < maxThreads; ++i) {
        workers.emplace_back([this]() {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this]() {
                        return stop || !tasks.empty();
                    });

                    if (stop && tasks.empty()) return;

                    task = std::move(tasks.front());
                    tasks.pop();
                }

                task(); // Execute task
            }
        });
    }
}



/**
 * @brief        Destructor for the ThreadPool.
 *
 * @details      Signals all worker threads to stop and waits for them to join.
 *               Ensures graceful shutdown of the thread pool and cleanup of resources.
 */
ThreadPool::~ThreadPool() {
    stop = true;
    condition.notify_all();
    for (auto& thread : workers) {
        if (thread.joinable()) thread.join();
    }
}

    
