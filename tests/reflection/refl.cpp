#include <catch2/catch_test_macros.hpp>
#include "core/meta/reflection/refl.h"

using namespace Zafkiel::Reflection;

TEST_CASE("refl")
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

    vec3 v;
    v.x = 1.0f;
    v.y = 2.0f;
    v.z = 3.0f;
    Camera cam1;
    cam1.fov = 45.0f;
    cam1.position = vec3{0.0f, 0.0f, 0.0f};
    cam1.lookAt = vec3{1.0f, 1.0f, 1.0f};
    cam1.data = {1, 1, 2, 3, 4};
    cam1.type = ProjectionType::Ortho;
    for (auto &[a, prop] : GetProperties(cam1))
    {
        if (prop->GetTypeInfo() == GetType("vec3"))
        {
            auto &vec = a.As<vec3>();
            vec.x += 10.0f;
        }
        if (prop->GetTypeInfo()->GetName() == "std::vector<Int32>")
        {
            const List *listType = prop->GetTypeInfo()->As<List>();
            for (int i = 0; i < listType->GetSize(a); i++)
            {
                Any enumNum = listType->GetElem(i, a);
                enumNum.As<int>()++;
            }
        }
        if (prop->GetTypeInfo() == GetType<ProjectionType>())
        {
            const Enum *enumType = prop->GetTypeInfo()->As<Enum>();
            REQUIRE(enumType->GetValue(a) == 1);
            enumType->SetValue(a, 0);
        }
    }
    REQUIRE(cam1.lookAt.x == 11.0f);
    REQUIRE(cam1.lookAt.y == 1.0f);
    REQUIRE(cam1.lookAt.z == 1.0f);

    REQUIRE(cam1.data[1] == 2);
    REQUIRE(cam1.type == ProjectionType::Perspective);

    const Camera &cam2 = cam1;
    for (const auto &[a, prop] : GetProperties(cam2))
    {
        if (prop->GetTypeInfo() == GetType<float>())
        {
            float num = a.As<float>();
            REQUIRE(num == 45.0f);
        }
        if (prop->GetTypeInfo() == GetType<std::vector<int>>())
        {
            const List *listType = prop->GetTypeInfo()->As<List>();
            REQUIRE(listType->GetElem(0, a).As<int>() == 2);
        }
    }
}