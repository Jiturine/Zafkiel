#include "main.h"
#include <any>
#include "ECS/world.h"
#include <chrono>
#include <iostream>

static size_t memory = 0;

void *operator new(size_t size)
{
    memory += size;
    // std::cout << "Allocated memory: " << size << " bytes" << std::endl;
    return malloc(size);
}
void operator delete(void *ptr, size_t size)
{
    memory -= size;
    // std::cout << "Deallocated memory: " << size << " bytes" << std::endl;
    free(ptr);
}

template <typename Func>
void benchmark(const std::string &name, Func &&f, int iterations = 1000)
{
    using namespace std::chrono;
    auto start = high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i)
    {
        f(); // 多次迭代减少误差
    }

    auto end = high_resolution_clock::now();
    auto avg_time = duration_cast<microseconds>(end - start).count() / iterations;

    std::cout << name << ": " << avg_time << " us/iter\n";
}
using namespace Zafkiel;
static int cnt = 0;
struct Name
{
    int id;
};

struct Shader
{
    int vs;
};

int main(int argc, char **argv)
{
    // benchmark("QueryEntity", [&]() {

    // std::cout << memory << std::endl;
    // {
    //     World world;
    //     for (int i = 0; i < 100000; i++)
    //     {
    //         auto entity = world.SpawnEntity<Name>(Name{i});

    //         if (i & 1)
    //         {
    //             entity.AddComponent<Shader>(Shader{i});
    //         }
    //     }

    //     for (auto e : world.Query<Name, Shader>())
    //     {
    //         e.GetComponent<Name>().id++;
    //     }

    //     for (auto e : world.Query<Name>())
    //     {
    //         world.Destroy(e);
    //     }
    // }
    // std::cout << memory << std::endl;
    // });
    World world;
    auto entity = world.SpawnEntity<Name>(Name{1});
    entity.AddComponent<Shader>(Shader{2});
    entity.RemoveComponent<Name>();
    for (auto e : world.Query<Name, Shader>())
    {
        std::cout << e.GetComponent<Name>().id << std::endl;
    }
    for (auto e : world.Query<Shader>())
    {
        std::cout << e.GetComponent<Shader>().vs << std::endl;
    }
    world.SetResource(Shader{5});
    auto &shader = world.GetResource<Shader>();
    std::cout << shader.vs << std::endl;
    return 0;
}