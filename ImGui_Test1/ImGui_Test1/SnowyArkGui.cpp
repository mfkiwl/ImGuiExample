#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include"ImGui/imgui.h"
#ifndef IMGUI_DISABLE

#include <ctype.h>          // toupper
#include <limits.h>         // INT_MIN, INT_MAX
#include <math.h>           // sqrtf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>          // vsnprintf, sscanf, printf
#include <stdlib.h>         // NULL, malloc, free, atoi
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>         // intptr_t
#else
#include <stdint.h>         // intptr_t
#endif

//Setting
ImVec4 triangleColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

// Forward Declarations
static void ShowSnowyArkMainMenuBar();
static void ShowSnowyArkHelpWindow(bool* p_open);
static void ShowSnowyArkOverlay(bool* p_open);


void ShowSnowyArkWindow(bool* p_open)
{
    // Exceptionally add an extra assert here for people confused about initial Dear ImGui setup
    // Most ImGui functions would normally just crash if the context is missing.
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");

    //Other Windows
    static bool showSnowyArkMainMenuBar = true;
    static bool showSnowyArkHelpWindow = false;
    static bool showSnowyArkOverlay = true;

    if (showSnowyArkMainMenuBar)          ShowSnowyArkMainMenuBar();
    if (showSnowyArkHelpWindow)           ShowSnowyArkHelpWindow(&showSnowyArkHelpWindow);
    if (showSnowyArkOverlay)              ShowSnowyArkOverlay(&showSnowyArkOverlay);

    // We specify a default position/size in case there's no data in the .ini file.
    // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
    ImGui::SetNextWindowPos(ImVec2(540, 18), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(260, 200), ImGuiCond_FirstUseEver);

    //create window
    if (!ImGui::Begin("SnowyArk", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove))
    {
        ImGui::End();
        return;
    }
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            ImGui::MenuItem("Open","Ctrl+W");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Windows"))
        {
            ImGui::MenuItem("Main Menu Bar", NULL, &showSnowyArkMainMenuBar);
            ImGui::MenuItem("Simple Overlay", NULL, &showSnowyArkOverlay);
            ImGui::Separator();
            ImGui::MenuItem("Help", NULL, &showSnowyArkHelpWindow);

            ImGui::EndMenu();
        }
        

        ImGui::EndMenuBar();
    }
    ImGui::ColorEdit3("Triangle Color", (float*)&triangleColor);
    //Leave a fixed amount of width for labels (by passing a negative value), the rest goes to widgets.
    //...What Fuck?
    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);


    ImGui::End();
}

static void ShowSnowyArkMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Flie"))
        {
            if (ImGui::BeginMenu("New"))
            {
                ImGui::MenuItem("Flie");
                ImGui::MenuItem("Folder");

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Open")) {}
            if (ImGui::MenuItem("Quit", "Alt+F4")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

static void ShowSnowyArkHelpWindow(bool* p_open)
{
    ImGui::SetNextWindowSize(ImVec2(400, 150), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Help", p_open))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Help Documentation");
    ImGui::Separator();
    ImGui::BulletText("SnowyArk is a soft renderer based on OpenGL");


    ImGui::End();
}

void ShowSnowyArkOverlay(bool* p_open)
{
    const float DISTANCE = 18.0f;
    static int corner = 0;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags overlayWindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (corner != -1)
    {
        overlayWindowFlags |= ImGuiWindowFlags_NoMove;
        ImVec2 overlayPos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE - 18 : DISTANCE - 18, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
        ImVec2 overlayPosPivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(overlayPos, ImGuiCond_Always, overlayPosPivot);
    }
    ImGui::SetNextWindowBgAlpha(0.35f);// Transparent background
    if (ImGui::Begin("Overlay", p_open, overlayWindowFlags))
    {
        ImGui::Text("This is a Overlay Demo.");
        ImGui::Separator();
        if (ImGui::IsMousePosValid())
        {
            ImGui::Text("Mouse Position:(%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        }else
        {
            ImGui::Text("Mouse Position:<invalid>");
        }
        ImGui::Text("FPS:%.1f", ImGui::GetIO().Framerate);
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (p_open && ImGui::MenuItem("Close")) *p_open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}




#endif // #ifndef IMGUI_DISABLE