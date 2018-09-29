#include <SDL.h>
#include <fstream>
#include <stdio.h>
#include <GL/gl3w.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

const uint32_t SDL_BLACK = 0x00000000;
const uint32_t SDL_WHITE = 0x00FFFFFF;

struct pgm
{
    const char* magic = "P2";
    const int32_t max = 255;
    int32_t width, height;
    uint32_t* image;

    pgm()
    {
        width = 0;
        height = 0;
        image = nullptr;
    }
};

void readPgm(std::ifstream& stream, pgm& image)
{
    int32_t max;
    std::string magic;
    stream >> magic >> image.width >> image.height >> max;

    int32_t pixel;
    int32_t width = image.width;
    int32_t height = image.height;
    image.image = new uint32_t[image.width*image.height];

    int32_t i = 0;
    while(height--)
    {
        while(width--)
        {
            stream >> pixel;
            image.image[i] = pixel == 0 ? SDL_BLACK: SDL_WHITE;
            i++;
        }
        width = image.width;
    }
}

int main(int argc, char** argv)
{
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) return -1;

    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_Window* window = SDL_CreateWindow("MedBench", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_BORDERLESS);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    if(gl3wInit() != 0) return 1;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImGui::StyleColorsDark();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    pgm texture;
    std::ifstream imageData;
    imageData.open("Logo.pgm");
    readPgm(imageData, texture);
    imageData.close();

    GLuint textureID = 0;
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.image);
    glGenerateMipmap(GL_TEXTURE_2D);

    bool done = false;
    while(!done)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_QUIT)
                done = true;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        static float f = 0.0f;

        ImGui::Begin("Edit", nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse);
        ImGui::Text("This is some useful text.");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);   
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        if(ImGui::Button("Button"))
        {
            done = true;
        }

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Begin("Frames", nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse);
        ImGui::ImageButton((void*)(intptr_t)textureID, ImVec2(128,64));
        ImGui::End();

        ImGui::Render();
        SDL_GL_MakeCurrent(window, gl_context);
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
        SDL_Delay(10);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
