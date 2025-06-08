#include "main.h"
using namespace Zafkiel;

size_t cnt_size = 0;

void *operator new(size_t size)
{
    std::cout << "Allocating " << size << " bytes\n";
    cnt_size += size;
    return malloc(size);
}

void operator delete(void *ptr, size_t size) noexcept
{
    std::cout << "Deallocating " << size << " bytes\n";
    cnt_size -= size;
    free(ptr);
}

int main(int argc, char **argv)
{
    {
        using namespace Reflection;
        ReflectionGenerate::RegisterReflectionInfo();
        vec3 v;
        v.x = 1.0f;
        v.y = 2.0f;
        v.z = 3.0f;
        Camera cam;
        cam.fov = 45.0f;
        cam.position = vec3{0.0f, 0.0f, 0.0f};
        cam.lookAt = vec3{1.0f, 1.0f, 1.0f};
        std::cout << cnt_size << std::endl;
        std::cout << "-------------" << std::endl;
        {
            for (auto &[a, prop] : GetProperties(cam))
            {
                std::cout << prop->GetTypeInfo()->GetName() << std::endl;
                if (prop->GetTypeInfo() == GetType<vec3>())
                {
                    auto &v = RemoveRef<vec3>(a);
                    std::cout << "Value: " << v.x << " " << v.y << " " << v.z << std::endl;
                    v.x++;
                }
            }
        }
        std::cout << cam.lookAt.x << " " << cam.lookAt.y << " " << cam.lookAt.z << std::endl;
        std::cout << cnt_size << std::endl;
        std::cout << "-------------" << std::endl;
    }

    return 0;
}