#include <catch2/catch_test_macros.hpp>
#include "reflection/refl.h"
#include "reflection/register.h"

using namespace Zafkiel::Reflection;

TEST_CASE("refl")
{
    struct vec3
    {
        float x, y, z;
    };
    struct Camera
    {
        float fov;
        vec3 position;
        vec3 lookAt;
    };
    Register<vec3>("vec3")
        .AddProperty(&vec3::x, "x")
        .AddProperty(&vec3::y, "y")
        .AddProperty(&vec3::z, "z");

    Register<Camera>("Camera")
        .AddProperty(&Camera::fov, "fov")
        .AddProperty(&Camera::position, "position")
        .AddProperty(&Camera::lookAt, "lookAt");

    vec3 v;
    v.x = 1.0f;
    v.y = 2.0f;
    v.z = 3.0f;
    Camera cam;
    cam.fov = 45.0f;
    cam.position = vec3{0.0f, 0.0f, 0.0f};
    cam.lookAt = vec3{1.0f, 1.0f, 1.0f};
    {
        for (auto &[a, prop] : GetProperties(cam))
        {
            if (prop->GetTypeInfo() == GetType("vec3"))
            {
                auto &vec = RemoveRef<vec3>(a);
                vec.x += 10.0f;
            }
        }
    }
    REQUIRE(cam.lookAt.x == 11.0f);
    REQUIRE(cam.lookAt.y == 1.0f);
    REQUIRE(cam.lookAt.z == 1.0f);
}