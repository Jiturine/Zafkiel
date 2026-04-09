#include <catch2/catch_test_macros.hpp>
#include "Core/Meta/Reflection/Refl.h"
#include "Core/Meta/Serializer/BinarySerializer.h"

using namespace Zafkiel;
using namespace Zafkiel::Reflection;

namespace Test
{

struct vec3
{
    float x, y, z;
};
enum ProjectionType
{
    Perspective,
    Ortho
};
struct Camera
{
    float fov;
    vec3 position;
    vec3 lookAt;
    std::vector<int> data;
    ProjectionType type;
};

TEST_CASE("Dynamic Reflection bahaves correctly", "[reflection]")
{
    Register<vec3>("vec3")
        .AddProperty(&vec3::x, "x")
        .AddProperty(&vec3::y, "y")
        .AddProperty(&vec3::z, "z");

    Register<ProjectionType>("ProjectionType")
        .Add(ProjectionType::Perspective, "ProjectionType::Perspective")
        .Add(ProjectionType::Ortho, "ProjectionType::Ortho");

    Register<Camera>("Camera")
        .AddProperty(&Camera::fov, "fov")
        .AddProperty(&Camera::position, "position")
        .AddProperty(&Camera::lookAt, "lookAt")
        .AddProperty(&Camera::data, "data")
        .AddProperty(&Camera::type, "type");
    
    SECTION("Access and modify properties correctly")
    {
        Camera cam
        {
            .fov = 45.0f,
            .position = {5.0f, 4.0f, 3.0f},
            .lookAt = {1.0f, 0.0f, 0.0f},
            .data = {10, 5, 7},
            .type = ProjectionType::Perspective
        };
        for (auto &[a, prop] : GetProperties(cam))
        {
            if (prop->GetTypeInfo() == GetType<vec3>())
            {
                auto &vec = a.As<vec3>();
                vec.x += 10.0f;
            }
            if (prop->GetTypeInfo()->GetName() == "std::vector<Int32>")
            {
                const List *listType = prop->GetTypeInfo()->As<List>();
                for (int i = 0; i < listType->GetSize(a); i++)
                {
                    AnyRef enumNum = listType->GetElem(i, a);
                    enumNum.As<int>()++;
                }
            }
            if (prop->GetTypeInfo() == GetType<ProjectionType>())
            {
                const Enum *enumType = prop->GetTypeInfo()->As<Enum>();
                REQUIRE(enumType->GetValue(a) == 0);
                enumType->SetValue(a, ProjectionType::Ortho);
            }
        }
        REQUIRE(cam.position.x == 15.0f);
        REQUIRE(cam.lookAt.x == 11.0f);
        REQUIRE(cam.data[1] == 6);
        REQUIRE(cam.type == ProjectionType::Ortho);   
    }
    
    
}   
}

