#include "ThreadPool.hpp"
#include "MySQLConnectionPool.hpp"
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>


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
    
        void ThreadPool::enqueue(std::function<void()> task) {
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                tasks.push(std::move(task));
            }
            condition.notify_one();
        }


    
        ThreadPool::~ThreadPool() {
            stop = true;
            condition.notify_all();
            for (auto& thread : workers) {
                if (thread.joinable()) thread.join();
            }
        }

    
