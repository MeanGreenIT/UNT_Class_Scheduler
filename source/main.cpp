// Building on top of the example code from DearImgui so we can not waste time reinventing the wheel
// ImGui - standalone example application for SDL2 + OpenGL
// If you are new to ImGui, see documentation at the top of libs/imgui/imgui.cpp
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

#include "imgui.h"
#include "imgui_impl_sdl_gl3.h"

#include <mysql.h>

#include <iostream>
#include <string>
#include <stdio.h>
#include <GL/gl3w.h>    // This is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <SDL.h>

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_Window *window = SDL_CreateWindow("Schedule Assistant", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 1024, SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    gl3wInit();

    // Setup ImGui binding
    ImGui_ImplSdlGL3_Init(window);

    // Setup style
    ImGui::StyleColorsDark();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //Show Window Variables
    bool show_main_window = true;


    //Varibles
    int window_height = current.h;
    int window_width = current.w;
    static char db_username[32] = "";
    static char db_password[32] = "";
    static char db_host[32] = "";
    static char db_database[32] = "";
    static char db_table[32] = "";
    bool pullSQL = false;

    //Table Setup
    static ImGui::ListView lv;
    int column_total = 11;
    //ListViewHeader("Lablel","ToolTip", TYPE, precision, starting width, prefix, suffix, sorted, editable);
    lv.headers.push_back(ImGui::ListViewHeader("ID", "PrimaryKey",ImGui::ListView::HT_INT, -1, 20));
    lv.headers.push_back(ImGui::ListViewHeader("Catalog", "Student Catalog Number",ImGui::ListView::HT_INT, -1, 30,"","",true));
    lv.headers.push_back(ImGui::ListViewHeader("Course", "",ImGui::ListView::HT_STRING, -1, 100));
    lv.headers.push_back(ImGui::ListViewHeader("Enroll Cap", "",ImGui::ListView::HT_INT, -1, 20));
    lv.headers.push_back(ImGui::ListViewHeader("Enroll Total", "",ImGui::ListView::HT_INT, -1, 20));
    lv.headers.push_back(ImGui::ListViewHeader("Wait Listed", "",ImGui::ListView::HT_INT, -1, 20));
    lv.headers.push_back(ImGui::ListViewHeader("DOTW", "Day of the Week",ImGui::ListView::HT_STRING, -1, 20));
    lv.headers.push_back(ImGui::ListViewHeader("StartTime", "",ImGui::ListView::HT_STRING, -1, 50));
    lv.headers.push_back(ImGui::ListViewHeader("EndTime", "",ImGui::ListView::HT_STRING, -1, 50));
    lv.headers.push_back(ImGui::ListViewHeader("RoomID", "FacilID",ImGui::ListView::HT_STRING, -1, 50));
    lv.headers.push_back(ImGui::ListViewHeader("Professor", "",ImGui::ListView::HT_STRING, -1, 200));

    // Main loop
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
        }
        ImGui_ImplSdlGL3_NewFrame(window);

        //Display Main Window
        if(show_main_window)
        {
            window_height = current.h;
            window_width = current.w;
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(window_width,window_height), ImGuiCond_Always);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);    //Disable Window Rounding
            ImGui::Begin("MainMenu", &show_main_window, ImGuiWindowFlags_NoTitleBar);
            //First Row, Username/Password
            {
                ImGui::PushItemWidth(256);
                ImGui::Text("Username:");
                ImGui::SameLine();
                ImGui::InputText("##Username", db_username, IM_ARRAYSIZE(db_username));
                ImGui::SameLine();
                ImGui::Text("Password:");
                ImGui::SameLine();
                ImGui::InputText("##Password", db_password, IM_ARRAYSIZE(db_password), ImGuiInputTextFlags_Password);
                ImGui::PopItemWidth();
            }
            //Second Row
            {
                ImGui::PushItemWidth(256);
                ImGui::Text("IP:");
                ImGui::SameLine();
                ImGui::InputText("##hostname", db_host, IM_ARRAYSIZE(db_host));
                ImGui::SameLine();
                ImGui::Text("Database:");
                ImGui::SameLine();
                ImGui::InputText("##database", db_database, IM_ARRAYSIZE(db_database));
                ImGui::SameLine();
                ImGui::Text("Table:");
                ImGui::SameLine();
                ImGui::InputText("##table", db_table, IM_ARRAYSIZE(db_table));
                ImGui::PopItemWidth();
            }
            //Third Row
            {
                if(ImGui::Button("Pull"))
                {
                    pullSQL = true;
                }
            }

            if(pullSQL)
            {
                MYSQL *con = mysql_init(NULL);
                if(con == NULL)
                {
                    ImGui::Text("%s\n", mysql_error(con));
                }
                if(mysql_real_connect(con, db_host, db_username, db_password, db_database, 0, NULL, 0) == NULL)
                {
                    ImGui::Text("%s\n", mysql_error(con));
                    mysql_close(con);
                }
                char queryStatement[80];
                strcpy(queryStatement, "SELECT * FROM ");
                strcat(queryStatement, db_table);
                if (mysql_query(con, queryStatement)) 
                {
                    mysql_close(con);
                }
                else
                {
                    MYSQL_RES *result = mysql_store_result(con);
                    if (result == NULL) 
                    {
                        mysql_close(con);
                    }
                    int num_fields = mysql_num_fields(result);

                    MYSQL_ROW row;
                    while ((row = mysql_fetch_row(result))) 
                    { 
                      for(int i = 0; i < num_fields; i++) 
                      { 
                          ImGui::Text("%s ", row[i] ? row[i] : "NULL"); 
                      } 
                          ImGui::Text("\n"); 
                    }

                    mysql_free_result(result);
                    mysql_close(con);
                }
                pullSQL = false;

            }

            //Display Data
            ImGui::Text("testing");
            /*if(lv.headers.size()==0)*/{
                

                class MyListViewItem : public ImGui::ListView::ItemBase {
                public:

                    // Fields and their pointers (MANDATORY!)
                    int ID;
                    int catalog;
                    char course[1024];
                    int enrollCap;
                    int enrollTotal;
                    int waitTotal;
                    char dotw[16];
                    char startTime[16];
                    char endTime[16];
                    char RoomID[128];
                    char Professor[256];
                    const void* getDataPtr(size_t column) const {
                        switch (column) {
                        case 0: return (const void*) &ID;
                        case 1: return (const void*) &catalog;
                        case 2: return (const void*) course;
                        case 3: return (const void*) &enrollCap;
                        case 4: return (const void*) &enrollTotal;
                        case 5: return (const void*) &waitTotal;
                        case 6: return (const void*) dotw;
                        case 7: return (const void*) startTime;
                        case 8: return (const void*) endTime;
                        case 9: return (const void*) RoomID;
                        case 10: return (const void*) Professor;
                        }
                        return NULL;
                    }
                        // Please note that we can easily try to speed up this method by adding a new field like:
                        // const void* fieldPointers[number of fields];    // and assigning them in our ctr
                        // Then here we can just use:
                        // IM_ASSERT(column<number of fields);
                        // return fieldPointers[column]

                    // (Optional) ctr for setting values faster later
                    MyListViewItem(int _id,int _catalog,const char* _course, int _enrollCap, int _enrollTotal, int _waitTotal, const char* _dotw, const char* _startTime, const char* _endTime, const char* _roomID, const char* _professor)
                        : ID(_id), catalog(_catalog), enrollCap(_enrollCap), enrollTotal(_enrollTotal), waitTotal(_waitTotal) {
                        IM_ASSERT(_course && strlen(_course)<1024);
                        IM_ASSERT(_dotw && strlen(_dotw)<16);
                        IM_ASSERT(_startTime && strlen(_startTime)<16);
                        IM_ASSERT(_endTime && strlen(_endTime)<16);
                        IM_ASSERT(_roomID && strlen(_roomID)<128);
                        IM_ASSERT(_professor && strlen(_professor)<256);
                        strncpy(course,_course,sizeof(course));
                        strncpy(dotw,_dotw,sizeof(dotw));
                        strncpy(startTime,_startTime,sizeof(startTime));
                        strncpy(endTime,_endTime,sizeof(endTime));
                        strncpy(RoomID,_roomID,sizeof(RoomID));
                        strncpy(Professor,_professor,sizeof(Professor));
                    }
                    virtual ~MyListViewItem() {}
                };

                lv.items.resize(2);
                MyListViewItem* item;
                item = (MyListViewItem*) ImGui::MemAlloc(sizeof(MyListViewItem));   // MANDATORY (ImGuiListView::~ImGuiListView() will delete these with ImGui::MemFree(...))
                IMIMPL_PLACEMENT_NEW(item) MyListViewItem(1,1030,"COMPUTER SCIENCE I", 285,89,3,"MWF","15:00:00","15:50:00", "ESSC 255", "Shrestha Pradhumna Lal");
                lv.items[0] = item;
                item = (MyListViewItem*) ImGui::MemAlloc(sizeof(MyListViewItem));   // MANDATORY (ImGuiListView::~ImGuiListView() will delete these with ImGui::MemFree(...))
                IMIMPL_PLACEMENT_NEW(item) MyListViewItem(2,1030,"COMPUTER SCIENCE I", 130,130,9,"MWF","10:30:00","11:20:00", "NTDP B185", "Thompson Sr. Mark Anothony");
                lv.items[1] = item;
                static int maxListViewHeight=200;                             // optional: by default is -1 = as height as needed
                ImGui::SliderInt("ListView Height (-1=full)",&maxListViewHeight,-1,500);// Just Testing "maxListViewHeight" here:

                lv.render((float)maxListViewHeight);//(float)maxListViewHeight,&optionalColumnReorder,-1);   // This method returns true when the selectedRow is changed by the user (however when selectedRow gets changed because of sorting it still returns false, because the pointed row-item does not change)
                ImGui::Text("testing");
            }

            ImGui::End();
            ImGui::PopStyleVar();
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