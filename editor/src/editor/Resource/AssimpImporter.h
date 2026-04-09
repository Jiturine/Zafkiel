#pragma once
#include "Core/Base/Utils.h"
#include "Function/RHI/RHIResources.h"
#include "Platform/Filesystem/Filesystem.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Zafkiel
{

static mat4 AiMatrixToMat(const aiMatrix4x4 &m)
{
    return mat4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
}

class AssimpNode
{
  public:
    AssimpNode(aiNode *node, const aiScene *scene) : node(node), scene(scene) {}
    mat4 GetLocalTransform() const { return AiMatrixToMat(node->mTransformation); }
    std::string GetName() const { return node->mName.C_Str(); }
    std::vector<AssimpNode> GetChildren() const
    {
        std::vector<AssimpNode> ret;
        for (size_t i = 0; i < node->mNumChildren; i++)
        {
            ret.emplace_back(node->mChildren[i], scene);
        }
        return ret;
    }
    std::vector<std::pair<uint32, aiMesh *>> GetMeshes() const
    {
        std::vector<std::pair<uint32, aiMesh *>> ret;
        for (size_t i = 0; i < node->mNumMeshes; i++)
        {
            ret.emplace_back(node->mMeshes[i], scene->mMeshes[node->mMeshes[i]]);
        }
        return ret;
    }
  private:
    aiNode *node;
    const aiScene *scene;
};

class AssimpTexture
{
  public:
    AssimpTexture() = default;
    AssimpTexture(const std::string &path, const aiScene *scene) : path(path), scene(scene) {}
    Path GetPath() const
    {
        std::string texPathName = path.c_str();
        Utils::Replace(texPathName, '\\', '/');
        return texPathName;
    }

    bool IsEmbedded() const
    {
        const aiTexture* tex = scene->GetEmbeddedTexture(path.c_str());
        if (tex) 
            return true;
        else 
            return false;
        if (!path.empty() && path.c_str()[0] == '*')
            return true;
        else
            return false;
    }

    bool IsCompressed() const
    {
        const aiTexture *texture = scene->GetEmbeddedTexture(path.c_str());
        return texture->mHeight == 0;
    }

    uint32 GetWidth() const 
    { 
        const aiTexture *texture = scene->GetEmbeddedTexture(path.c_str());
        return texture->mWidth; 
    }
    uint32 GetHeight() const 
    {
        const aiTexture *texture = scene->GetEmbeddedTexture(path.c_str());
        return texture->mHeight; 
    }

    const uint8 *GetData() const
    {
        const aiTexture *texture = scene->GetEmbeddedTexture(path.c_str());
        return reinterpret_cast<const uint8*>(texture->pcData);
    }

    uint32 GetDataSize() const
    {
        const aiTexture *texture = scene->GetEmbeddedTexture(path.c_str());
        if (texture->mHeight == 0)
            return texture->mWidth;
        else
            return texture->mWidth * texture->mHeight * 4;
    }

  private:
    std::string path;
    const aiScene *scene;
};

class AssimpMaterial
{
  public:
    AssimpMaterial(aiMaterial *material, const aiScene *scene) : material(material), scene(scene) {}
    bool GetVec3(const char *pKey, unsigned int type, unsigned int idx, vec3 &output) const 
    {
        aiColor3D color3D;
        if (material->Get(pKey, type, idx, color3D) == AI_SUCCESS)
        {
            output = vec3(color3D.r, color3D.g, color3D.b);
            return true;
        }
        else return false;
    }
    bool GetTexture(aiTextureType textureType, AssimpTexture &output) const
    {
        aiString path;
        if (material->GetTexture(textureType, 0, &path) == AI_SUCCESS)
        {
            output = AssimpTexture(path.C_Str(), scene);
            return true;
        }
        else
            return false;
    }
    std::string GetName() const { return material->GetName().C_Str(); }

  private:
    aiMaterial *material;
    const aiScene *scene;
};

class AssimpImporter
{
  public:
    AssimpImporter(const Path &filePath)
    {
        scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Log::Error("Failed to load Model: {}", importer.GetErrorString());
        }
    }
    AssimpNode GetRootNode() const { return {scene->mRootNode, scene}; }

    std::vector<std::pair<uint32, AssimpMaterial>> GetMaterials()
    {
        std::vector<std::pair<uint32, AssimpMaterial>> ret;
        for (size_t i = 0; i < scene->mNumMaterials; i++)
        {
            ret.emplace_back(i, AssimpMaterial(scene->mMaterials[i], scene));
        }
        return ret;
    }

    AssimpTexture GetTexture(size_t index)
    {
        return AssimpTexture(std::format("*{}", index), scene);
    }
  private:
    Assimp::Importer importer;
    const aiScene *scene;
};
}
