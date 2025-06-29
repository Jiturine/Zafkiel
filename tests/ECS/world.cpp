#include <catch2/catch_test_macros.hpp>
#include <ECS/world.h>

using namespace Zafkiel;

TEST_CASE("world")
{
    struct Person
    {
        int age;
        std::string name;
    };
    struct Movement
    {
        float x, y, z;
    };
    World world;
    for (int i = 0; i < 100; i++)
    {
        auto entity = world.SpawnEntity(
            Person{1, std::format("<{}>", i)},
            Movement{1.0f * i, 2.0f * i, 3.0f * i});
        if (i % 2 == 0)
        {
            entity.RemoveComponent<Movement>();
        }
        if (i % 4 == 0)
        {
            entity.AddComponent<Movement>({1.0f * i, 2.0f * i, 3.0f * i});
        }
    }
    int cnt = 0;
    for (auto entity : world.Query<Person, Movement>())
    {
        cnt++;
    }
    REQUIRE(cnt == 75);
}