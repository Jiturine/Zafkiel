#include "editor/editor_layer.h"
#include "editor/resource/editor_asset_manager.h"
#include "editor/test_layer.h"
#include "core/application/application.h"
using namespace Zafkiel;

int main()
{
    auto& app = Application::Create();
    app.PushLayer(CreateRef<TestLayer>());
    app.Run();
    return 0;
}
