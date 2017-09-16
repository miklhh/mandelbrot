#include <iostream>

#include "thread_pool.h"

/* The mandelbrot threadpool structure. */
struct mandelbrot_threadpool 
{
    int n_threads;
    std::vector<std::thread> workers;
    std::mutex worker_busy_mutex;
    std::vector<bool> worker_busy;
    std::mutex jobs_mutex;
    std::queue<segment_t> jobs;
    std::function<void(segment_t)> render_segment;
};

/* Callback function for the thread pool threads. */
static void mandelbrot_thread_pool_thread_callback(
    mandelbrot_threadpool* threadpool,
    int thread_index
)
{
    /* Test if there is any job to do. Someone might have snatched our job. */
    std::unique_lock<std::mutex> worker_busy_lock(threadpool->worker_busy_mutex);
    std::unique_lock<std::mutex> jobs_lock(threadpool->jobs_mutex);
    if (threadpool->jobs.empty())
    {
        /* Another thread has snated out work. Return. */
        threadpool->worker_busy.at(thread_index) = false;
        return;
    }
    else
    {
        /* Get the job that needs to be done and start working the jobs.*/
        segment_t current_segment = threadpool->jobs.front();
        threadpool->jobs.pop();
        jobs_lock.unlock();
        worker_busy_lock.unlock();
        while (1) 
        {
            /* Render the current segment and try to get a new job.*/
            threadpool->render_segment(current_segment);
            jobs_lock.lock();
            if (threadpool->jobs.empty())
            {
                /* No more jobs. Return this thread. */
                jobs_lock.unlock();
                worker_busy_lock.lock();
                threadpool->worker_busy.at(thread_index) = false;
                worker_busy_lock.unlock();
                std::cout << "Thread Returning." << std::endl;     // DEBUGING.
                return;
            }
            else
            {
                /* More jobs to be done! */
                current_segment = threadpool->jobs.front();
                threadpool->jobs.pop();
                jobs_lock.unlock();
            }
        }
    }
}

/* Internal function for launching a thread. */
static void mandelbrot_thread_pool_launch(mandelbrot_threadpool* threadpool)
{
    /* Try to find a free thread. */
    std::unique_lock<std::mutex> worker_busy_lock(threadpool->jobs_mutex);
    int index_next_free_thread = -1;
    for (int i = 0; i < threadpool->n_threads; i++)
    {
        if (!threadpool->worker_busy.at(i)) 
        { 
            index_next_free_thread = i;
            break;
        }
    }
    /* Act acordingly if thread has been found. */
    if (index_next_free_thread != -1)
    {
        /* A free thread has been found. Launch it. */
        threadpool->worker_busy.at(index_next_free_thread) = true;
        threadpool->workers.at(index_next_free_thread) = std::thread(
            mandelbrot_thread_pool_thread_callback,
            threadpool,
            index_next_free_thread);
        threadpool->workers.at(index_next_free_thread).detach();
    }
}

/* Initialize a mandelbrot thread pool. */
mandelbrot_threadpool* mandelbrot_thread_pool_create(
    int threads,
    std::function<void(segment_t)> render_segment_callback)
{
    /* Allocate the threadpool and set it up. */
    mandelbrot_threadpool* threadpool = new mandelbrot_threadpool;
    if (!threadpool == NULL) 
    {
        /* Create the proper amount of thread objects. */
        for (int i = 0; i < threads; i++)
        {
            /* No need to lock mutex since no other threads are using resources. */
            threadpool->workers.push_back(std::thread());
            threadpool->worker_busy.push_back(false);
        }
        /* Set threadpool data. */
        threadpool->render_segment = render_segment_callback;
        threadpool->n_threads = threads;
        return threadpool;
    }
    else 
    { 
        return NULL; 
    }
}

/* Add a job to the thread pool. */
void mandelbrot_thread_pool_add_job(
    mandelbrot_threadpool* threadpool, 
    segment_t segment)
{
    /* Add the job to the job queue and launch the thread pool. */
    std::unique_lock<std::mutex> jobs_lock(threadpool->jobs_mutex);
    threadpool->jobs.push(segment);
    jobs_lock.unlock();
    mandelbrot_thread_pool_launch(threadpool);
}

/* Destroy a threadpool. */
int mandelbrot_thread_pool_destroy(mandelbrot_threadpool * threadpool)
{
    std::cout << "Killing threads." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    for (int i = 0; i < threadpool->n_threads; i++)
    {
        threadpool->workers.at(i).~thread();
    }
    delete threadpool;
    return 0;
}

/* Returns the amount of active workers. */
int mandelbrot_thread_pool_get_active_workers(mandelbrot_threadpool * threadpool)
{
    std::lock_guard<std::mutex> lock_worker_busy(threadpool->worker_busy_mutex);
    int active_workers = 0;
    for (int i = 0; i < threadpool->n_threads; i++)
    {
        if (threadpool->worker_busy.at(i)) { active_workers++; }
    }
    return active_workers;
}

