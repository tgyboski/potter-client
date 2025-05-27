Application::Application()
{
    // Inicializa o WebView2Manager
    WebView2Manager::getInstance().initialize(static_cast<WIN32Window&>(g_window).getWindowHandle());
} 