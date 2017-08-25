#ifndef MANDELBROT_THREAD_POOL_H
#define MANDELBROT_THREAD_POOL_H

#include <vector>

using std::vector;

class thread_pool
{
public:


private:
    vector<void()> callback;
    vector<
    int threads;
    bool pool_done;
};

#endif