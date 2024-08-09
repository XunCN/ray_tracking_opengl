#include <iostream>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "texture.h"

void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << '\n';
}

#ifdef DEBUG
void gl_debug_output(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam)
{
    // ignore unimportant error/warning
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cerr << "---------------" << std::endl;
    std::cerr << "Debug message (" << id << "): " << message << std::endl;

    switch(source)
    {
    case GL_DEBUG_SOURCE_API:             std::cerr << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cerr << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cerr << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cerr << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cerr << "Source: Other"; break;
    } std::cerr << std::endl;

    switch(type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cerr << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cerr << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cerr << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cerr << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cerr << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cerr << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cerr << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cerr << "Type: Other"; break;
    } std::cerr << std::endl;

    switch(severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cerr << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cerr << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cerr << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "Severity: notification"; break;
    } std::cerr << std::endl;
    std::cerr << std::endl;
}
#endif  // DEBUG

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_W && action == GLFW_PRESS  // Ctrl + W to exit
        && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

GLFWwindow* init(const char* window_name, unsigned width, unsigned height)
{
    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit())
        return nullptr;

    const char* glsl_version = "#version 450";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, window_name, NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        return nullptr;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    // glfwSwapInterval(1); // Enable vsync

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    GLenum err = glewInit();
    if(GLEW_OK != err)
    {
        std::cerr << "glew init error: " << glewGetErrorString(err) << '\n';
        return nullptr;
    }

#ifdef DEBUG
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if(flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_output, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    else
    {
        std::cout << "OpeGL debugger set failed\n";
    }
#endif // DEBUG

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    io.ConfigViewportsNoAutoMerge = true;
    io.ConfigViewportsNoTaskBarIcon = true;
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return window;
}

void clean(GLFWwindow* window)
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void print_env_info()
{
    std::cout << "GLEW versison: " << glewGetString(GLEW_VERSION) << '\n';
    std::cout << "OpenGL vendor: " << glGetString(GL_VENDOR) << '\n';
    std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << '\n';
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';

    int compute_shader_config[3] = {0};
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &compute_shader_config[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &compute_shader_config[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &compute_shader_config[2]);
    std::cout << "max global work group counts x: " << compute_shader_config[0]
        << " y: " << compute_shader_config[1] << " z: " << compute_shader_config[2] << '\n';
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &compute_shader_config[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &compute_shader_config[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &compute_shader_config[2]);
    std::cout << "max local work group size x: " << compute_shader_config[0]
        << " y: " << compute_shader_config[1] << " z: " << compute_shader_config[2] << '\n';
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &compute_shader_config[0]);
    std::cout << "max work group invovations: " << compute_shader_config[0] << '\n';
}

int main(void)
{
    int window_width = 1000;
    int window_height = 800;
    int texture_width = 600;
    float zoom_level = 1.0f;  // zoom level for texture size
    float aspect_ratio = 16.0f / 9.0f;
    int texture_height = texture_width / aspect_ratio;

    GLFWwindow* window = init("ray tracking", window_width, window_height);
    if(!window)
    {
        std::cerr << "Init failed\n";
        return EXIT_FAILURE;
    }

    print_env_info();

    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simkai.ttf",
20.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    IM_ASSERT(font != nullptr);

    std::shared_ptr<float> p_texture(new float[texture_width * texture_height * 3], [](float* p) {delete[] p; });
    for(int i = 0; i < texture_width * texture_height * 3;)
    {
        p_texture.get()[i++] = 0.1f;
        p_texture.get()[i++] = 0.8f;
        p_texture.get()[i++] = 0.3f;
    }

    Texture picture(texture_width, texture_height, Texture::ChannelType::RGB, Texture::DataType::FLOAT, p_texture.get());


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if(glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // set dock space for main window
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus | ImGuiDockNodeFlags_NoDockingOverCentralNode;

        ImGui::Begin("DOCK_BASE", nullptr, window_flags);
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        ImGui::End();  // DOCK_BASE
        ImGui::PopStyleVar(3);

        // split view is not show as public api yet, drag the windows to arrange them
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_Once);
        ImGui::Begin("base viewer port", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_HorizontalScrollbar);

        ImVec2 region = ImGui::GetContentRegionAvail();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        int texture_show_width = texture_width * zoom_level;
        int texture_show_height = texture_show_width / aspect_ratio;
        pos = ImVec2(pos.x + (region.x - texture_show_width) / 2,
            pos.y + (region.y - texture_show_height) / 2);

        ImGui::SetCursorScreenPos(ImVec2(pos.x - .0f, pos.y - .0f));
        ImGui::Image((void*)picture.get_id(), ImVec2(texture_show_width, texture_show_height));

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRect(ImVec2(pos.x, pos.y),
            ImVec2(pos.x + texture_show_width, pos.y + texture_show_height),
            ImU32(IM_COL32(160, 30, 30, 255)), 0.0f, 0, 3.0f);

        ImGui::End();  // base viewer port

        ImGui::Begin(u8"渲染配置");

        ImGui::SeparatorText(u8"渲染数据");
        ImGui::Text(u8"渲染图像大小：%d X %d", texture_width, texture_height);
        ImGui::Text(u8"显示图像大小：%d X %d", texture_show_width, texture_show_height);
        ImGui::Dummy(ImGui::GetItemRectSize());  // keep an item sized empty space

        ImGui::SeparatorText(u8"配置项");
        ImGui::SliderFloat(u8"缩放", &zoom_level, 0.25, 8.0f);
        ImGui::SameLine();
        if(ImGui::Button("reset##zoom_level")) zoom_level = 1.0f;

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y - ImGui::GetItemRectSize().y * 3);
        ImGui::Separator();
        static bool show_demo_window = false;
        ImGui::Checkbox(u8"显示Dear ImGui演示窗口", &show_demo_window);
        if(show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::End();  // 渲染配置

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    clean(window);

    return 0;
}

