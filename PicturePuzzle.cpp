#include "SDL.h"
#include "SDL_image.h"
#include "SDL_surface.h"
#include "SDL_render.h"
// #include "processthreadsapi.h"
// #include "D:\programs\reverse\eigen\Eigen\Eigen"
#include <vector>
#include "synchapi.h"
#include "normal_header.hpp"


const char* DefaultPuzzleImage = "./src/img/countryside.png";
const char *imgPath = "./src/img/wildness.png";

SDL_Surface* imgSf;
SDL_Window* wd;
SDL_Surface* sf;
SDL_Renderer* rd;
SDL_Texture* imgT;
SDL_Event evt;
SDL_EventFilter evtFlt;

bool ContinueMainLoop = true;
bool MouseDown = false;

void MouseDownHandler();
void MouseUpHandler();
void MouseMotionHandler();

void ZeroMmry(void* p, int size);
void ClearRect(SDL_Rect* r);
/*
    abandoned class
    minimized program method used
*/
class Puzzle{
    char* ImagePath;
    bool ImageLoaded = false;
    
    unsigned xGrid = 3;
    unsigned yGrid = 3;

    SDL_Surface* Image = nullptr;
    std::vector<SDL_Rect*> grid;
    SDL_Window* PuzzleWindow = nullptr;

public:
    static const int DefaultWindowSize[4];
    static const SDL_WindowFlags DefaultWindowFlag;

public:
    Puzzle(const char* mainImage = DefaultPuzzleImage):ImagePath((char*)mainImage){}
    
    bool CheckImageLoaded(){return ImageLoaded;}

    bool LoadImage(char* PuzzleImagePath = nullptr){
        if (PuzzleImagePath != nullptr)
            ImagePath = PuzzleImagePath;
        Image = IMG_Load(ImagePath);
        if (CheckPointer(Image)){
            return ImageLoaded = true;
        }return false;
    }

    void setGrid(unsigned x,unsigned y){
        xGrid = x;  
        yGrid = y;
    }
    
    bool setGrid(){
        if (!CheckPointer(Image)){return false;}
        
    }

    bool SetWindow(SDL_Window* window = nullptr,int* size = (int*) DefaultWindowSize,SDL_WindowFlags flag = DefaultWindowFlag){
        if (!(CheckPointer(window)|CheckPointer(PuzzleWindow))){
            PuzzleWindow = SDL_CreateWindow("title",DefaultPuzzleImage[0],DefaultPuzzleImage[1],DefaultPuzzleImage[2],DefaultPuzzleImage[3],0);
            return true;
        }
        return false;
    }
};

const int Puzzle::DefaultWindowSize[4] = {1,2,3,4};
const SDL_WindowFlags Puzzle::DefaultWindowFlag = SDL_WindowFlags::SDL_WINDOW_OPENGL;


int main_puzzle();

# undef main
int main(){
    return main_puzzle();
}
int main_puzzle(){
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_GAMECONTROLLER|SDL_INIT_SENSOR|SDL_INIT_EVENTS)<0)
        exit(0);
    // const char *imgPath = "./src/img/countryside.png";


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
    // SDL_FillRect(sf,nullptr,SDL_MapRGB(sf->format, 0, 255, 210));
    // SDL_UpdateWindowSurface(wd);
    // AlertPointer(sf);
    
    //  split
    SDL_SetRenderDrawColor(rd,0,255,210,0);
    SDL_RenderFillRect(rd,nullptr);
    SDL_SetRenderDrawColor(rd,255,128,0,0);
    SDL_RenderFillRect(rd,&rdGrid);

    SDL_RenderCopy(rd,imgT,&imgGrid,&wdBox);
    SDL_RenderPresent(rd);

MAIN_LOOP:
    ContinueMainLoop = true;
    while (ContinueMainLoop){
        //  check events
        if (SDL_PollEvent(&evt)){
            switch(evt.type){
            case SDL_QUIT:
                goto TEST_END;
            case SDL_MOUSEBUTTONDOWN:
                MouseDownHandler();
                break;
            case SDL_MOUSEBUTTONUP:
                MouseUpHandler();
                break;
            case SDL_MOUSEMOTION:
                
            default:
                break;
            }
        }
        SDL_RenderPresent(rd);
    }

TEST_END:
    SDL_DestroyWindow(wd);    
    return 0;
}



void MouseDownHandler(){
    MouseDown = true;
    
}
void MouseUpHandler(){
    MouseDown = false;
}
void MouseMotionHandler(){
    if (MouseDown){
        
    }
}



void ZeroMmry(void* p, int size){
    memset(p,0,size);
}
void ClearRect(SDL_Rect* r){
    ZeroMmry(r,sizeof(SDL_Rect));
}