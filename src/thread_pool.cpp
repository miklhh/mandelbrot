#include <iostream>

#include "thread_pool.h"


static void mandelbrot_threadpool_thread_callback(
    mandelbrot_threadpool* threadpool,
    int thread_index
)
{
    /* Test if there is any job to do. Someone might have snatched our job. */
    if (threadpool->jobs.empty())
    {
        /* Another thread has snated out work. Return. */
        threadpool->worker_busy.at(thread_index) = false;
        return;
    }
    else
    {
        /* Get the job that needs to be done. */
        segment_t current_segment = threadpool->jobs.front();
        threadpool->jobs.pop();

        /* Let the thread iterate as long as there is job to do. */
        while (1)
        {
            /* Render the current segment. */
            threadpool->render_segment(current_segment);

            /* Try to get a new job if available. */
            if (threadpool->jobs.empty())
            {
                /* No more jobs. Return this thread. */
                threadpool->worker_busy.at(thread_index) = false;
                std::cout << "Thread Returning." << std::endl;
                return;
            }
            else
            {
                /* More jobs to be done! */
                current_segment = threadpool->jobs.front();
                threadpool->jobs.pop();
            }
        }
    }
}


static void mandelbrot_threadpool_launch(mandelbrot_threadpool* threadpool)
{
    /* Try to find a free thread. */
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
            mandelbrot_threadpool_thread_callback,
            threadpool,
            index_next_free_thread
        );
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
            threadpool->worker_busy.push_back(false);
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



void mandelbrot_threadpool_add_job(mandelbrot_threadpool* threadpool, segment_t segment)
{
    /* Add the job to the job queue and launch the thread pool. */
    std::unique_lock<std::mutex> jobs_lock(threadpool->jobs_mutex);
    threadpool->jobs.push(segment);
    jobs_lock.unlock();
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

int mandelbrot_threadpool_get_active_workers(mandelbrot_threadpool * threadpool)
{
    int active_workers = 0;
    std::lock_guard<std::mutex> lock_worker_busy(threadpool->worker_busy_mutex);
    for (int i = 0; i < threadpool->n_threads; i++)
    {
        if (threadpool->worker_busy.at(i)) { active_workers++; }
    }
    return active_workers;
}

