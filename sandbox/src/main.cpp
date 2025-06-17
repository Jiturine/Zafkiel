#include "main.h"
#include <any>
#include "ECS/world.h"
using namespace Zafkiel;

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
    World world;
    auto entity = world.SpawnEntity<Name>(Name{1});
    entity.AddComponent<Shader>(Shader{2});
    for (auto e : world.Query<Name, Shader>())
    {
        std::cout << e.GetComponent<Name>().id << std::endl;
    }
    for (auto e : world.Query<Name>())
    {
        std::cout << e.GetComponent<Name>().id << std::endl;
    }
    world.SetResource(Shader{5});
    auto &shader = world.GetResource<Shader>();
    std::cout << shader.vs << std::endl;
    return 0;
}