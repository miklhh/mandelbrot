#include <iostream>

#include "thread_pool.h"


/* The callback function for the threads in the pool.*/
static void mandelbrot_threadpool_thread_callback(
    mandelbrot_threadpool* threadpool,
    int thread_index)
{
    /* TEST IF THERE IS ANY JOBS IN THE QUEUE. ANOTHER THREAD MIGHT BE REALLY
     *  REALLY FAST AND TAKE THE LAST JOB. */
    std::lock_guard<std::mutex> lock_job(threadpool->jobs_mutex);
    if (threadpool->jobs.empty())
    {
        /* Someone else took this threads job. Just return it to the threadpool. */
        lock_job.~lock_guard();
        std::lock_guard<std::mutex> lock_busy(threadpool->busy_mutex);
        threadpool->busy[thread_index] = false;
    }

    /* Variable to keep track if this is the first job for this thread. 
     * and to keep track of it's current segment. */
    bool first_loop_iteration = true;
    segment_t current_segment = threadpool->jobs.front();
    threadpool->jobs.pop();
    lock_job.~lock_guard();
    
    /* Loop for whenever there is job to do. */
    for (;;)
    {
        /* Draw the segment. */
        threadpool->render_segment(current_segment);

        /* Try to get a new job and act accordingly. */
        std::lock_guard<std::mutex> lock_job_loop(threadpool->jobs_mutex);
        if (threadpool->jobs.empty())
        {
            lock_job_loop.~lock_guard();
            std::lock_guard<std::mutex> lock_busy_loop(threadpool->busy_mutex);
            threadpool->busy.at(thread_index) = false;
            lock_busy_loop.~lock_guard();
            return;
        }
        else
        {
            current_segment = threadpool->jobs.front();
            threadpool->jobs.pop();
            lock_job_loop.~lock_guard();
        }



        /* If this is not the first iteration, we need to test for a new element. */
        if (!first_loop_iteration)
        {
            /* Test if there are more jobs to do. */
            std::lock_guard<std::mutex> lock_job_loop(threadpool->jobs_mutex);
            if (threadpool->jobs.empty())
            {
                lock_job_loop.~lock_guard();
                return;
            }
            else
            {
                current_segment = threadpool->jobs.front();
                lock_job_loop.~lock_guard();
                first_loop_iteration = false;
            }
        }

        /* Get to work. */
        threadpool->render_segment(current_segment);

        /* Redo the loop to get the next*/
    }
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
                threadpool,
                index_next_free_thread);
            threadpool->workers.at(index_next_free_thread).detach();
            
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

int mandelbrot_threadpool_init(
        mandelbrot_threadpool* threadpool, 
        int threads,
        std::function<void(segment_t)> render_segment_callback)
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
        threadpool->render_segment = render_segment_callback;

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

