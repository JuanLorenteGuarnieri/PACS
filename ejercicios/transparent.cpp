#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <algorithm>
#include <execution>
#include <future>
#include <chrono>


struct Pixel
{
    u_int8_t red, green, blue, alpha = 255;
};

struct alpha_image
{
    int width;
    int height;
    std::vector<Pixel> data;

    alpha_image(int w, int h) : width(w), height(h), data(w * h) 
    {
        // Build rng
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0, 1.0);

        for (int i = 0; i < w * h; i++)
        {
            data[i] = {
                (u_int8_t)(dis(gen) * 255),
                (u_int8_t)(dis(gen) * 255),
                (u_int8_t)(dis(gen) * 255),
                (u_int8_t)(dis(gen) * 255)
            };
        }
    }

    Pixel operator()(int w, int h) const
    {
        return data[h * width + w];
    }

    Pixel& operator()(int w, int h)
    {
        return data[h * width + w];
    }
};


Pixel combine(const Pixel &p1, const Pixel &p2)
{
    float alpha_f = p1.alpha / 255.0f;
    float alpha_b = p2.alpha / 255.0f;

    float r = alpha_f*p1.red + (1-alpha_f)*alpha_b*p2.red;
    float g = alpha_f*p1.green + (1-alpha_f)*alpha_b*p2.green;
    float b = alpha_f*p1.blue + (1-alpha_f)*alpha_b*p2.blue;

    Pixel result {
            (u_int8_t)r,
            (u_int8_t)g,
            (u_int8_t)b,
            255};

    return result;
}

alpha_image alpha_over_operator(const alpha_image& f, const alpha_image& b)
{
    alpha_image result(f.width, f.height);

    for (int i = 0; i < f.width; i++)
    {
        for (int j = 0; j < f.height; j++)
        {
            Pixel pf = f(i, j);
            Pixel pb = b(i, j);

            result(i, j) = combine(pf, pb);
        }
    }

    return result;
}

alpha_image parallel_alpha_over_operator(const alpha_image& f, const alpha_image& b)
{
    alpha_image result(f.width, f.height);

    auto process_batch = [&](int idx, int nThreads)
    {
        int batch_size = f.height / nThreads;
        int start = idx * batch_size;
        int end = (idx == nThreads - 1) ? f.height : start + batch_size;

        for (int row = start; row < end; row++)
        {
            for (int col = 0; col < f.width; col++)
            {
                Pixel pf = f(col, row);
                Pixel pb = b(col, row);

                result(col, row) = combine(pf, pb);
            }
        }
    };   // Each thread processes contiguous rows to better use cache

    std::vector<std::thread> threads;
    int nThreads = std::thread::hardware_concurrency();

    for (int i = 0; i < nThreads; i++) {
        threads.push_back(std::thread(process_batch, i, nThreads));
    }
    for (auto &t : threads) {
        t.join();
    }

    return result;
}

alpha_image future_alpha_over_operator(const alpha_image& f, const alpha_image& b)
{
    alpha_image result(f.width, f.height);

    auto process_row = [&](int row)
    {
        for (int col = 0; col < f.width; col++)
        {
            Pixel pf = f(col, row);
            Pixel pb = b(col, row);

            result(col, row) = combine(pf, pb);
        }
    };

    std::vector<std::future<void>> futures;
    for (int row = 0; row < f.height; row++) {
        futures.push_back(std::async(std::launch::async, process_row, row));
    }

    for (auto &f : futures)
    {
        f.get();
    }

    return result;
}

bool check_result (const alpha_image &a, 
                    const alpha_image &b,
                    const alpha_image &result)
{
    for (int i = 0; i < a.width; i++)
    {
        for (int j = 0; j < a.height; j++)
        {
            Pixel p = combine(a(i, j), b(i, j));

            if (p.red != result(i, j).red ||
                p.green != result(i, j).green ||
                p.blue != result(i, j).blue ||
                p.alpha != result(i, j).alpha)
            {
                return false;
            }
        }
    }

    return true;
}

int main()
{
    alpha_image f(10000, 10000);
    alpha_image b(10000, 10000);

    std::cout << "Running sequential alpha over operator..." << std::endl;

    auto init = std::chrono::high_resolution_clock::now();
    alpha_image result = parallel_alpha_over_operator(f, b);
    auto end = std::chrono::high_resolution_clock::now();

    if (check_result(f, b, result))
    {
        std::cout << "Success in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - init).count() << " ms" << std::endl;
    }
    else
    {
        std::cout << "Fail!" << std::endl;
    }

    return 0;
}