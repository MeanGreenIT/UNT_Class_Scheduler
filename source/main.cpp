#include "imgui.h"
#include "imgui_impl_sdl_gl3.h"
#include "window_setup.hpp"

#include <mysql.h>
#include <GL/gl3w.h>
#include <SDL.h>

#include <iostream>
#include <stdio.h>
#include <vector>

namespace class_scheduler{
  static const int DEFAULT_WINDOW_X = 1280;
  static const int DEFAULT_WINDOW_Y = 720;
}

//https://eliasdaler.github.io/using-imgui-with-sfml-pt2/#using-imgui-with-stl
namespace ImGui
{
  static auto vector_getter = [](void* vec, int idx, const char** out_text)
  {
      auto& vector = *static_cast<std::vector<std::string>*>(vec);
      if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
      *out_text = vector.at(idx).c_str();
      return true;
  };
  
  bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
  {
      if (values.empty()) { return false; }
      return Combo(label, currIndex, vector_getter,
          static_cast<void*>(&values), values.size());
  }
  
  bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
  {
      if (values.empty()) { return false; }
      return ListBox(label, currIndex, vector_getter,
          static_cast<void*>(&values), values.size());
  }
 
}

int main(int, char**){
  // Setup SDL
  if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){
    printf("Error: %s\n", SDL_GetError());
    return -1;
  }

  // Setup Window
  setupWindow();
  SDL_Window *window = SDL_CreateWindow(
    "Class Scheduler Assistant",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    class_scheduler::DEFAULT_WINDOW_X,
    class_scheduler::DEFAULT_WINDOW_Y,
    SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE
  );
  SDL_GLContext glcontext = SDL_GL_CreateContext(window);
  gl3wInit();

  // Setup ImGui binding
  ImGui_ImplSdlGL3_Init(window);

  // Setup Style
  ImGui::StyleColorsDark();
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = 0.0f;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Show Window Variables that determine if a window should be drawn
  bool showCSVMenu = true;
  bool showOptionsMenu = true;

  // Option Variables
  static char dbUsername[32] = "";
  static char dbPassword[32] = "";
  static char dbHost[32] = "";
  static char dbDatabase[32] = "";
  static char dbTable[32] = "";

  //File Browsing Variables
  const char* startingFolder = "./";
  const char* optionalFileExtensionFilterString = ".csv";

  // Trigger Variables
  bool pullSQL = false;
  bool updateData = false;

  // SDL Variables that change because of client actions

  // Main Loop
  bool done = false;
  while(!done){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
      ImGui_ImplSdlGL3_ProcessEvent(&event);
      if(event.type == SDL_QUIT){
        done = true;
      }
    }
    ImGui_ImplSdlGL3_NewFrame(window);

    // Main Code Goes Here
    if(showOptionsMenu){
      ImGui::Begin("Server Options", &showOptionsMenu);
      //First Row, Username/Password
      {
          ImGui::PushItemWidth(256);
          ImGui::Text("Username:");
          ImGui::SameLine();
          ImGui::InputText("##Username", dbUsername, IM_ARRAYSIZE(dbUsername));
          ImGui::SameLine();
          ImGui::Text("Password:");
          ImGui::SameLine();
          ImGui::InputText("##Password", dbPassword, IM_ARRAYSIZE(dbPassword), ImGuiInputTextFlags_Password);
          ImGui::PopItemWidth();
      }
      //Second Row
      {
          ImGui::PushItemWidth(256);
          ImGui::Text("IP:");
          ImGui::SameLine();
          ImGui::InputText("##hostname", dbHost, IM_ARRAYSIZE(dbHost));
          ImGui::SameLine();
          ImGui::Text("Database:");
          ImGui::SameLine();
          ImGui::InputText("##database", dbDatabase, IM_ARRAYSIZE(dbDatabase));
          ImGui::SameLine();
          ImGui::Text("Table:");
          ImGui::SameLine();
          ImGui::InputText("##table", dbTable, IM_ARRAYSIZE(dbTable));
          ImGui::PopItemWidth();
      }
      //Third Row
      {
          if(ImGui::Button("Pull From DB"))
          {
              pullSQL = true;
          }
      }
      ImGui::End();
    }
    if(showCSVMenu){
      ImGui::Begin("CSV Options", &showOptionsMenu);
      //File Browser
      ImGui::Text("Please choose a CSV file: ");
      ImGui::SameLine();
      const bool browseButtonPressed = ImGui::Button("...");
      static ImGuiFs::Dialog fsInstance;
      const char* chosenPath = fsInstance.chooseFileDialog(browseButtonPressed,startingFolder,optionalFileExtensionFilterString);
      if (strlen(chosenPath)>0) {
          // A path (chosenPath) has been chosen right now. However we can retrieve it later using: fsInstance.getChosenPath()
      }
      /* if (strlen(fsInstance.getChosenPath())>0){
          ImGui::Text("Chosen File: \"%s\"",fsInstance.getChosenPath());
          ImGui::SameLine();
          if(ImGui::Button("Replace Current Table with CSV"))
          {
              std::string csvFilePath = fsInstance.getChosenPath();
              SQLData = ParseCSV(csvFilePath);
              updateData = true;
          }
      } */
      ImGui::End();
    }
    
    // Rendering
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    SDL_GL_SwapWindow(window);
  }

  // Cleanup
  ImGui_ImplSdlGL3_Shutdown();
  SDL_GL_DeleteContext(glcontext);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}