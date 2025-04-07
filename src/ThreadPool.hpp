#ifndef ThreadPOOL
#define ThreadPOOL

#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <atomic>
#include <vector>



class ThreadPool {
    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
    
        std::mutex queueMutex;
        std::condition_variable condition;
        std::atomic<bool> stop;
    
    public:
        ThreadPool(int maxThreads);
        template<typename F>
        void enqueue(F&& f) {
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                tasks.emplace(std::forward<F>(f));
            }
            condition.notify_one();
        }
           
        ~ThreadPool();
    };

#endif