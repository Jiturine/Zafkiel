#pragma once
#include "Function/Scene/Entity.h"
#include "Resource/Asset.h"

namespace Zafkiel 
{

struct SelectionContext
{
    SelectionContext() : type(Type::None) { }
    enum class Type
    {
        None, Entity, Asset
    } type;
    union 
    {
        Entity entity;
        AssetHandle asset;
    };
};

class EditorContext final
{
  public:
    EditorContext() = default;
    ~EditorContext() = default;
    static void Init()
    {
        instance = new EditorContext;
    }
    static void Destroy()
    {
        delete instance;
        instance = nullptr;
    }
    
    static SelectionContext &GetSelectionContext() { return instance->selectionContext; }
    static void SetSelectedEntity(Entity entity) { instance->SetSelectedEntityImpl(entity); }

  private:
    SelectionContext selectionContext;
    
    void SetSelectedEntityImpl(Entity entity);

    inline static EditorContext *instance;
};

}