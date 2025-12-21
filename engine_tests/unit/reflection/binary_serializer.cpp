#include <catch2/catch_test_macros.hpp>
#include "core/meta/reflection/refl.h"
#include "core/meta/serializer/binary_serializer.h"

using namespace Zafkiel::Reflection;
using namespace Zafkiel;

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

TEST_CASE("Binary Serializer/Deserializer Works correctly", "[reflection]")
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

    SECTION("Custom Type")
    {
        Camera cam
        {
            .fov = 45.0f,
            .position = vec3(0, 0, 0),
            .lookAt = vec3(1, 1, 1),
            .data = { 1, 2, 3},
            .type = ProjectionType::Perspective
        };

        BinarySerializer serializer;
        serializer.Value(cam);
        auto binaryData = serializer.GetOutput();
        
        BinaryDeserializer deserializer(binaryData.data(), binaryData.size());
        auto result = deserializer.As<Camera>();

        REQUIRE(result.fov == 45.0f);
        REQUIRE(result.position.x == 0);
        REQUIRE(result.lookAt.y == 1);
        REQUIRE(result.data[2] == 3);
        REQUIRE(result.type == ProjectionType::Perspective);       
    }

    SECTION("Access List Element")
    {
        std::vector<vec3> positons
        {
            {1.0f, 0.0f, 1.0f},
            {2.0f, 3.0f, 4.0f},
            {-1.0f, -2.0f, -3.0f}
        };
        
        BinarySerializer serializer;
        serializer.Value(positons);
        auto binaryData = serializer.GetOutput();

        BinaryDeserializer deserializer(binaryData.data(), binaryData.size());
        REQUIRE(deserializer.IsSeq());
        REQUIRE(deserializer[1].As<vec3>().y == 3.0f);
        REQUIRE(deserializer[2].As<vec3>().z == -3.0f);
    }

    SECTION("Access Map Item By StringKey")
    {
        std::unordered_map<std::string, vec3> params;
        params["position1"] = {1.0f, 2.0f, 3.0f};
        params["position2"] = {4.0f, 5.0f, 6.0f};
        params["position3"] = {7.0f, 8.0f, 9.0f};
         
        BinarySerializer serializer;
        serializer.Value(params);
        auto binaryData = serializer.GetOutput();

        BinaryDeserializer deserializer(binaryData.data(), binaryData.size());
        REQUIRE(deserializer["position3"].As<vec3>().z == 9.0f);
        REQUIRE(deserializer["position1"].As<vec3>().y == 2.0f);
    }

    SECTION("Traverse List")
    {
         std::vector<vec3> positons
        {
            {1.0f, 0.0f, 1.0f},
            {2.0f, 3.0f, 4.0f},
            {-1.0f, -2.0f, -3.0f}
        };
        
        BinarySerializer serializer;
        serializer.Value(positons);
        auto binaryData = serializer.GetOutput();

        BinaryDeserializer deserializer(binaryData.data(), binaryData.size());
        int sum = 0;
        for (auto elem : deserializer.SeqElems())
        {
            sum += elem.As<vec3>().x;   
        }
        REQUIRE(sum == 2.0f);
    }
    
    SECTION("Traverse Map")
    {  
        std::unordered_map<std::string, vec3> params;
        params["position1"] = {1.0f, 2.0f, 3.0f};
        params["position2"] = {4.0f, 5.0f, 6.0f};
        params["position3"] = {7.0f, 8.0f, 9.0f};
         
        BinarySerializer serializer;
        serializer.Value(params);
        auto binaryData = serializer.GetOutput();

        BinaryDeserializer deserializer(binaryData.data(), binaryData.size());

        for (auto [key, value] : deserializer.MapItems())
        {
            if (key.As<std::string>() == "position1")
            {
                REQUIRE(value.As<vec3>().x == 1.0f);
            }
            if (key.As<std::string>() == "position2")
            {
                REQUIRE(value.As<vec3>().z == 6.0f);
            }
        }
    }
    
    SECTION("Complex Structure")
    {
        BinarySerializer serializer;
        serializer.BeginMap();
        serializer.Key("Vertices").BeginSeq();
        serializer.BeginBatchSeq().Value(vec3(1.0f, 1.0f, 1.0f)).Value(vec3(1.0f, 1.0f, 1.0f)).Value(vec2(1.0f, 1.0f)).EndBatchSeq();
        serializer.BeginBatchSeq().Value(vec3(1.0f, 1.0f, 1.0f)).Value(vec3(1.0f, 1.0f, 1.0f)).Value(vec2(1.0f, 1.0f)).EndBatchSeq();
        serializer.EndSeq();
        serializer.EndMap();
        
        auto binaryData = serializer.GetOutput();
        BinaryDeserializer deserializer(binaryData.data(), binaryData.size());

        for (auto vertexData : deserializer["Vertices"].SeqElems())
        {
            vec3 pos = vertexData[0].As<vec3>();
            REQUIRE(pos.x == 1.0f);
        }
    }

    SECTION("Packed Data (e.g. Mesh Vertices & Indices")
    {
        BinarySerializer serializer;
        std::vector<uint32_t> indices;
        for (uint32_t i = 0; i < 10000; i++)
        {
            indices.push_back(i);
        }
        serializer.WritePack(indices.data(), indices.size() * sizeof (uint32_t));
        
        auto data = serializer.GetOutput();
        BinaryDeserializer deserializer(data.data(), data.size());
        
        auto [dataPtr, size] = deserializer.Unpack();
        std::vector<uint32_t> unpackedIndices(reinterpret_cast<const uint32_t *>(dataPtr), 
            reinterpret_cast<const uint32_t *>(dataPtr + size));

        REQUIRE(unpackedIndices[2000] == 2000);
    }
}


}