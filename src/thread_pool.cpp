#include <iostream>

#include "thread_pool.h"

/* The callback function for the threads in the pool.*/
static void mandelbrot_threadpool_thread_callback(int thread_index)
{
    /* TEST IF THERE IS ANY JOBS IN THE QUEUE. ANOTHER THREAD MIGHT BE REALLY
     *  REALLY FAST AND TAKE THE LAST JOB. */
}

/* Internal function used to launch a thread, if there is any thread free to use. */
static void mandelbrot_threadpool_launch(mandelbrot_threadpool* threadpool)
{
    /* Lock the jobs mutex so no-one can snatch a possible job. */
    std::lock_guard<std::mutex> lock_jobs(threadpool->jobs_mutex);
    if (threadpool->jobs.empty()) { return; }
    else
    {
        /* Try to find a free thread. */
        std::lock_guard<std::mutex> lock_busy(threadpool->busy_mutex);
        int index_next_free_thread;
        int i = 0;
        while (i < threadpool->n_threads)
        {
            if (!threadpool->busy.at(i))
            {
                index_next_free_thread = i;
                break;
            }
        }

        if (index_next_free_thread)
        {
            /* WOHO, there is a free thread. Launch it to do work! */
            threadpool->busy.at(index_next_free_thread) = true;
            lock_jobs.~lock_guard(); // Free the jobs so the new thread can use it.
            threadpool->workers.at(index_next_free_thread) = std::thread(
                mandelbrot_threadpool_thread_callback,
                index_next_free_thread);
            
            /* Our job here is done. */
            return;
        }

        /* Else */
        else
        {
            return;
        }
    }
}

int mandelbrot_threadpool_init(mandelbrot_threadpool* threadpool, int threads)
{
    /* Allocate the threadpool. */
    threadpool = new mandelbrot_threadpool;

    /* Setup the threadpool. */
    if (!threadpool == NULL) 
    {
        /* Create the proper amount of thread objects. */
        for (int i = 0; i < threads; i++)
        {
            /* No need to lock mutex since no new threads are running. */
            threadpool->workers.push_back(std::thread());
            threadpool->busy.push_back(false);
        }

        /* Threadpool successfully created return accordingly. */
        return 0;
    }
    else 
    { 
        return -1; 
    }
}

int mandelbrot_threadpool_destroy(mandelbrot_threadpool * threadpool)
{
    std::cout << "PLEASE IMPLEMENT ME!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(30));
    return 12345678;
}

int mandelbrot_threadpool_add_job(
    mandelbrot_threadpool* threadpool,
    segment_t segment)
{
    /* Add the job to the job queue. */
    std::lock_guard<std::mutex> lock(threadpool->jobs_mutex);
    threadpool->jobs.push(segment);
    lock.~lock_guard();

    mandelbrot_threadpool_launch(threadpool);
    return 0;
}

