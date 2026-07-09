#include "editor/EditorLayer.h"
#include "editor/Resource/EditorAssetManager.h"
#include "editor/UILayer.h"
#include "Core/Application/Application.h"
using namespace Zafkiel;

int main()
{
    auto &app = Application::Instance();
    app.PushLayer(CreateRef<UILayer>());
    app.Run();
    return 0;
}
