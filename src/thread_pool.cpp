#include <iostream>

#include "thread_pool.h"


/* The callback function for the threads in the pool.*/
static void mandelbrot_threadpool_thread_callback(
    mandelbrot_threadpool* threadpool,
    int thread_index)
{
    /* Test if threr is any jobs in the queue. Someone might have snatched it
     * real fast while leaving. */
    std::cout << "Locking 1." << std::endl;
    std::lock_guard<std::mutex> lock_job(threadpool->jobs_mutex);
    std::cout << "Passt lock 1." << std::endl;

    if (threadpool->jobs.empty())
    {
        /* Someone else took this threads job. Return this thread to the 
         * thread pool. */
        lock_job.~lock_guard();
        std::lock_guard<std::mutex> lock_busy(threadpool->busy_mutex);
        threadpool->busy.at(thread_index) = false;
        std::cout << "Returning." << std::endl;
        return;
    }

    /* Variable to keep track if this is the first job for this thread. 
     * and to keep track of it's current segment. */
    bool first_loop_iteration = true;
    segment_t current_segment = threadpool->jobs.front();
    threadpool->jobs.pop();
    lock_job.~lock_guard();
    std::cout << "Looping." << std::endl;
    /* Loop as long as there is job to do. */
    for (;;)
    {
        /* Draw the segment. */
        std::cout << "Rendering!" << std::endl;
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
    }
}

/* Internal function used to launch a thread, if there is any thread free to use. */
static void mandelbrot_threadpool_launch(mandelbrot_threadpool* threadpool)
{
    /* Lock the jobs mutex so no-one can snatch a possible job. */
    std::lock_guard<std::mutex> lock_jobs(threadpool->jobs_mutex);
    if (threadpool->jobs.empty()) 
    {
        return; 
    }
    
    else
    {
        /* Try to find a free thread. */
        std::lock_guard<std::mutex> lock_busy(threadpool->busy_mutex);
        int index_next_free_thread = -1;
        int i = 0;
        while (i < threadpool->n_threads)
        {
            if (!threadpool->busy.at(i))
            {
                index_next_free_thread = i;
                break;
            }
            i++;
        }

        /* If there is a free thread. */
        if (index_next_free_thread != -1)
        {
            system("pause");
            /* A free thread in the pool found. Make it busy and unlock the
             * guard so the thread can use the guard. */
            threadpool->busy.at(index_next_free_thread) = true;
            lock_jobs.~lock_guard();

            /* Launch the thread. */
            std::cout << "Creating thread." << std::endl;
            threadpool->workers.at(index_next_free_thread) = std::thread(
                mandelbrot_threadpool_thread_callback,
                threadpool,
                index_next_free_thread);
            threadpool->workers.at(index_next_free_thread).detach();
            
            /* Our job here is done. */
            return;
        }

        /* If no free thread was found. */
        else
        {
            return;
        }
    }
}

mandelbrot_threadpool* mandelbrot_threadpool_init(
        int threads,
        std::function<void(segment_t)> render_segment_callback)
{
    /* Allocate the threadpool. */
    mandelbrot_threadpool* threadpool = new mandelbrot_threadpool;

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

        /* Set threadpool data. */
        threadpool->render_segment = render_segment_callback;
        threadpool->n_threads = threads;

        /* Threadpool successfully created return accordingly. */
        return threadpool;
    }
    else 
    { 
        return NULL; 
    }
}



void mandelbrot_threadpool_add_job(
    mandelbrot_threadpool* threadpool, 
    segment_t segment)
{
    /* Add the job to the job queue and launch the thread pool. */
    std::lock_guard<std::mutex> lock(threadpool->jobs_mutex);
    threadpool->jobs.push(segment);
    lock.~lock_guard();
    mandelbrot_threadpool_launch(threadpool);
}

/* Destroy a threadpool. */
int mandelbrot_threadpool_destroy(mandelbrot_threadpool * threadpool)
{
    for (int i = 0; i < threadpool->n_threads; i++)
    {
        threadpool->workers.at(i).~thread();
    }
    delete threadpool;
    return 0;
}

