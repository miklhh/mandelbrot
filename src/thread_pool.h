/*
 * Threadpool for drawing segments of the mandelbrot set.
 * 
 * Function explanations:
 *
 * (1) mandelbrot_threadpool* mandelbrot_thread_pool_create(
 *          int threads,
 *          std::function<void(segment_t)> render_segment);
 *     --
 *     Function for creating a threadpool. Returns a pointer to the thred 
 *     pool on success. Each initialized threadpool should
 *     by its initializer accordingly use mandelbrot_threadpool_destroy to 
 *     destroy the threadpool and return it's resourcese to prevent 
 *     memoryleaks. The threadpool should only be initialized once. 
 *     Initilizing a thread pool than one time causes undefined behaviour.
 *     --
 *
 * (2) int mandelbrot_thread_pool_destroy(mandelbrot_threadpool* threadpool);
 *     --
 *     Destroy a mandelbrot threadpool. This function releases all the memory
 *     that is held by a mandelbrot threadpool object.
 *     --
 *
 * (3) int mandelbrot_thread_pool_add_job(segment_t segment);
 *     --
 *     Add a job to the threadpool. This job will automaticly launch if any
 *     worker is free, otherwise it will be put in a queue.
 *     --
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
    std::mutex worker_busy_mutex;
    std::vector<bool> worker_busy;
    std::mutex jobs_mutex;
    std::queue<segment_t> jobs;
    std::function<void(segment_t)> render_segment;
};

/* Functions */
void mandelbrot_thread_pool_add_job(mandelbrot_threadpool* threadpool, segment_t segment);
int mandelbrot_thread_pool_destroy(mandelbrot_threadpool* threadpool);
int mandelbrot_thread_pool_get_active_workers(mandelbrot_threadpool* threadpool);
mandelbrot_threadpool* mandelbrot_thread_pool_create(
    int threads,
    std::function<void(segment_t)> render_segment);

#endif