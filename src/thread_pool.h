/*
 * Threadpool for drawing segments of the mandelbrot set.
 * 
 * Function explanations:
 * (1) int mandelbrot_threadpool_init(
 *          mandelbrot_threadpool* threadpool,
 *          int threads,
 *          std::function<void(segment_t)> render_segment);
 *      Function for initializing a mandelbrot threadpool object. From the 
 *      moment this functinocal has been made (and return succes == 0) the
 *      threadpool is ready to take jobs. Each initialized threadpools should
 *      by its initializer accordingly use mandelbrot_threadpool_destroy to 
 *      destroy the threadpool and return it's resourcese to prevent memoryleaks.
 *
 * (2) int mandelbrot_threadpool_destroy(mandelbrot_threadpool* threadpool);
 *      Destroy a mandelbrot threadpool. This function releases all the memory
 *      that is held by a mandelbrot threadpool object.
 *
 * (3) int mandelbrot_threadpool_add_job(segment_t segment)
 *      Add a job to the threadpool. This job will automaticly launch if any
 *      worker is free, otherwise it will be put in a queue.
 */



#ifndef MANDELBROT_THREAD_POOL_H
#define MANDELBROT_THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>

#include "types.h"

struct mandelbrot_threadpool
{
    int n_threads;
    std::vector<std::thread> workers;
    std::mutex busy_mutex;
    std::vector<bool> busy;
    std::mutex jobs_mutex;
    std::queue<segment_t> jobs;
    std::function<void(segment_t)> render_segment;
};

/* Functions */
int mandelbrot_threadpool_add_job(mandelbrot_threadpool* threadpool, segment_t segment);
int mandelbrot_threadpool_destroy(mandelbrot_threadpool* threadpool);
int mandelbrot_threadpool_init(
    mandelbrot_threadpool* threadpool,
    int threads,
    std::function<void(segment_t)> render_segment);

#endif