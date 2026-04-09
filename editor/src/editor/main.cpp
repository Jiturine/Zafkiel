#include "editor/EditorLayer.h"
#include "editor/Resource/EditorAssetManager.h"
#include "editor/TestLayer.h"
#include "editor/FontLayer.h"
#include "Core/Application/Application.h"
using namespace Zafkiel;

int main()
{
    auto& app = Application::Create();
    app.PushLayer(CreateRef<TestLayer>());
    app.Run();
    return 0;
}
