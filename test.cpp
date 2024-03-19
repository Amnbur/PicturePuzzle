#include "SDL.h"
#include "SDL_image.h"
#include "SDL_surface.h"
#include "SDL_render.h"

#include "processthreadsapi.h"
#include "windows.h"
#include <iostream>
#include "normal_header.hpp"
#include "D:\programs\reverse\eigen\Eigen\Eigen"

#define SLEEPTIME 2000
//  如果两个周期没有接收到Event即关闭程序

const char* bgImgFileName = "./src/img/countryside.png";

int gotMessage = 1;
int gotmessageLastValue = 1;
int ContinueMainLoop = 1;

SDL_HitTestResult  DefaultHitCallback(SDL_Window* win, const SDL_Point* area, void* data){
    return SDL_HITTEST_DRAGGABLE;
}


using namespace std;
DWORD WINAPI wakeuper(LPVOID lpParam);
void waked(int signum);

int test();
int eigenTest();

# undef main
int main(int argc, char* argv[]){
    // return test();
    return eigenTest();
    signal(SIGILL,waked);

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_AUDIO|SDL_INIT_GAMECONTROLLER)<0)
        return -1;         
	atexit(SDL_Quit);

    SDL_Window *MainWindow = SDL_CreateWindow(
        "Title",                           // 窗口title
        SDL_WINDOWPOS_UNDEFINED,           // x
        SDL_WINDOWPOS_UNDEFINED,           // y
        640,                               // 宽
        360,                               // 高
        SDL_WINDOW_OPENGL                  // flags - see below
    );if (MainWindow == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return -1;
    }
    SDL_SetWindowHitTest(MainWindow,DefaultHitCallback,NULL);
    SDL_Event MainWindowEvent;
    unsigned EventCount = 0;
    
    
    SDL_Renderer *MainRenderer = SDL_CreateRenderer(MainWindow,-1,SDL_RENDERER_ACCELERATED);
    SDL_RenderClear(MainRenderer);

    SDL_Surface* bgImg = IMG_Load(bgImgFileName);
    SDL_Rect bgImgBox = {0,0,bgImg->w,bgImg->h};
    SDL_Surface* MainSurface = SDL_GetWindowSurface(MainWindow);
    SDL_Texture* bgImgTexture = SDL_CreateTextureFromSurface(MainRenderer,bgImg);

    SDL_FillRect(MainSurface,nullptr,SDL_MapRGB(MainSurface->format, 0, 255, 210));
    SDL_UpdateWindowSurface(MainWindow);

    
    SDL_RenderCopy(MainRenderer,bgImgTexture,nullptr,&bgImgBox);
    SDL_RenderPresent(MainRenderer);
    bgImgBox.w = MainSurface->w/2;
    bgImgBox.h = MainSurface->h/2;
    SDL_RenderCopy(MainRenderer,bgImgTexture,&bgImgBox,&bgImgBox);
    SDL_RenderPresent(MainRenderer);

    SDL_Rect tmp = {0,0,192,108};


    SDL_BlitScaled(bgImg,nullptr,MainSurface,nullptr);
    // SDL_UpdateWindowSurface(MainWindow);
    
	SDL_FreeSurface(bgImg);

    DWORD threadID;
    HANDLE ThreadWakeuper = CreateThread(NULL,0,wakeuper,0,0,&threadID);
    if (ThreadWakeuper == INVALID_HANDLE_VALUE) {
        printf("创建线程失败");
        goto DestroyAndQuit;
    }

    while (ContinueMainLoop){
        SDL_UpdateWindowSurface(MainWindow);
        if (SDL_PollEvent(&MainWindowEvent)){
            gotMessage = 1;
            ++EventCount;

            switch(MainWindowEvent.type){
            case SDL_QUIT://退出事件
                SDL_Log("quit");
                goto DestroyAndQuit;
                break;
            default:
                ;
                // SDL_Log("event type:%d\nevent count:%u", MainWindowEvent.type,EventCount);
                // SDL_Log("%d,%d,%d\n",gotMessage,gotmessageLastValue,!(gotMessage|gotmessageLastValue));
            }
        }
        Sleep(20);
    }

DestroyAndQuit:
    SDL_DestroyWindow(MainWindow);
    return 0;
}

DWORD WINAPI wakeuper(LPVOID lpParam){
    gotmessageLastValue = 1;
Looper:
    gotmessageLastValue = gotMessage;
    gotMessage = 0;
    Sleep(SLEEPTIME);
    if (!(gotMessage|gotmessageLastValue))
        raise(SIGILL);
    else
        goto Looper;
    return 0;
}
void waked(int signum){
    SDL_Log("got signal:%d",signum);
    ContinueMainLoop = 0;
}
// #include "GL/gl.h"
// #include "GL/glext.h"
// #include "GL/glu.h"
// #include "D:\\programs\\code\\glfw\\include\\GLFW\\glfw3.h"

// int test(){
//     system("chcp 65001");
//     PrintVersion();
//     return 0;
// }

// GLvoid PrintVersion(){   
//     const GLubyte* name = glGetString(GL_VENDOR);            //返回负责当前OpenGL实现厂商的名字
//     const GLubyte* biaoshifu = glGetString(GL_RENDERER);    //返回一个渲染器标识符，通常是个硬件平台
//     const GLubyte* OpenGLVersion = glGetString(GL_VERSION);    //返回当前OpenGL实现的版本号
//     const GLubyte* glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);//返回着色预压编译器版本号
//     const GLubyte* gluVersion = gluGetString(GLU_VERSION);    //返回当前GLU工具库版本
//     printf("OpenGL实现厂商的名字：%s\n", name);
//     printf("渲染器标识符：%s\n", biaoshifu);
//     printf("OpenGL实现的版本号：%s\n", OpenGLVersion);
//     printf("OpenGL着色语言版本：%s\n", glsl);
//     printf("GLU工具库版本：%s\n", gluVersion);
// }


// int main(){
//     SDL_Window *window = SDL_CreateWindow("TEST",100,100,192,108,SDL_WINDOW_INPUT_GRABBED);
//     return 0;
// }

// enum NoteType{z,s,x,d,c,f,v};
// NoteType noteX[8] = {z,s,x,d,c,f,v};


int test(){
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)<0)
        exit(0);
    // const char *imgPath = "./src/img/countryside.png";
    const char *imgPath = "./src/img/wildness.png";
    
    SDL_Surface* imgSf;
    SDL_Window* wd;
    SDL_Surface* sf;
    SDL_Renderer* rd;
    SDL_Texture* imgT;

    //  create img
    imgSf = IMG_Load(imgPath);
    AlertPointer(imgSf);
    SDL_Rect imgBox = {0,0,imgSf->w,imgSf->h};
    SDL_Rect imgGrid = {0,0,imgSf->w / 3, imgSf->h /3};
    SDL_Rect rdGrid = {0,0,imgGrid.w+12,imgGrid.h+12};

    //  set & config window
    wd = SDL_CreateWindow("t",0,0,imgBox.w+36,imgBox.h+36,SDL_WindowFlags::SDL_WINDOW_OPENGL);
    SDL_Rect wdBox = {6,6,imgGrid.w,imgGrid.h};
    AlertPointer(wd);
    
    //  renderer
    rd = SDL_CreateRenderer(wd,-1,SDL_RENDERER_ACCELERATED);
    AlertPointer(rd);
    SDL_RenderClear(rd);
    imgT = SDL_CreateTextureFromSurface(rd,imgSf);
    AlertPointer(imgT);

    sf = SDL_GetWindowSurface(wd);
    SDL_FillRect(sf,nullptr,SDL_MapRGB(sf->format, 0, 255, 210));
    SDL_UpdateWindowSurface(wd);
    Sleep(1000);
    // AlertPointer(sf);
    
    //  split
    SDL_SetRenderDrawColor(rd,0,255,210,0);
    SDL_RenderFillRect(rd,nullptr);
    SDL_SetRenderDrawColor(rd,255,128,0,0);
    SDL_RenderFillRect(rd,&rdGrid);

    SDL_RenderCopy(rd,imgT,&imgGrid,&wdBox);
    SDL_RenderPresent(rd);
    Sleep(2000);

TEST_END:
    SDL_DestroyWindow(wd);
    Eigen::Matrix3<int> a;
    return 0;
}

int eigenTest(){
    Eigen::Matrix<int,-1,-1> mt;
    mt.resize(4,4);
    for (int i=0;i<4;++i){
        int j=0;
        for (auto it = mt.block<1,4>(i,0).begin();it!=mt.block<1,4>(i,0).end();++it){
            *it = i*4+j++;
            ++it;
        }
    }

    for (auto id = 0;id<4;++id){
        for (auto it = mt.block<1,4>(id,0).begin();it!=mt.block<1,4>(id,0).end();++it){
            std::cout << *it << ' ';
        }
    }
    return 0;
}