#include "SDL.h"
#include "SDL_image.h"
#include "SDL_surface.h"
#include "SDL_render.h"

#include "synchapi.h"
#include "normal_header.hpp"
#include <vector>


const unsigned sizeX = 5;
const unsigned sizeY = 3;
const unsigned PieceCount = sizeX * sizeY;
SDL_Rect imgPieces[PieceCount];
SDL_Rect mapPieces[PieceCount];
unsigned PieceMatch[PieceCount]={0,1,2,3,4,5,6,8,7,9,10,11,12,13,14};
std::vector<unsigned> MapPiecesToUpdate = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
std::vector<unsigned> SelectedPieces;


const Uint8 SelectingFrameColor[4] = {237,77,16,233};
const char* DefaultPuzzleImage = "./src/img/countryside.png";
// const char *imgPath = "./src/img/wildness.png";
const char *imgPath = "./src/img/countryside.png";

SDL_Surface* imgSf;
SDL_Window* wd;
SDL_Surface* sf;
SDL_Renderer* rd;
SDL_Texture* imgT;
SDL_Event evt;
SDL_EventFilter evtFlt;

bool ContinueMainLoop = true;
bool MouseDown = false;
bool FrameSize = 10;
int MouseX = 0;
int MouseY = 0;
bool MouseMovedBeforeRelease = false;
Uint32 FramColor = 0xff8888;

void sdl_renderDrawThickRect(SDL_Renderer *renderer, const SDL_Rect *rect,Uint8 width, Uint32 color);
void SetRectFrame(SDL_Renderer *renderer, const SDL_Rect *rect);

void SplitMap(int w,int h,SDL_Rect* rects,int xCount,int yCount);
bool CheckPieceMatch(unsigned* matchs,unsigned count);

bool CheckPointInRect(int x,int y,SDL_Rect* rect);
unsigned CheckMouseFocus();

void MouseDownHandler();
void MouseUpHandler();
void MouseMotionHandler();


void ClearRect(SDL_Rect* r);
/*
    abandoned class
    minimized program method used
*/
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
    // SDL_Rect rdGrid = {0,0,imgGrid.w+12,imgGrid.h+12};   //  abandoned

    //  set & config window
    int ta = imgBox.w , tb = imgBox.h, tc = 0;
    while (ta>960||tb>540){
        if (++tc > 5) return 0;
        ta /= 2;    tb /= 2;
    }
    wd = SDL_CreateWindow("t",30,30,ta,tb,SDL_WindowFlags::SDL_WINDOW_OPENGL);
    SDL_Rect wdBox = {0,0,imgGrid.w,imgGrid.h};
    AlertPointer(wd);
    
    //  renderer
    rd = SDL_CreateRenderer(wd,-1,SDL_RENDERER_ACCELERATED);
    AlertPointer(rd);
    SDL_RenderClear(rd);
    imgT = SDL_CreateTextureFromSurface(rd,imgSf);
    AlertPointer(imgT);

    sf = SDL_GetWindowSurface(wd);
    AlertPointer(sf);
    
    //  split
    SplitMap(imgSf->w,imgSf->h,imgPieces,sizeX,sizeY);
    SplitMap(sf->w,sf->h,mapPieces,sizeX,sizeY);


    SDL_SetRenderDrawBlendMode(rd,SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rd,255,255,255,255);
    SDL_RenderFillRect(rd,nullptr); //  白底
    SDL_SetRenderDrawColor(rd,0,0,0,0x80);  
    SDL_RenderFillRect(rd,nullptr);//  黑色蒙版
    //  得到灰底，rgba显示正常

    // SDL_SetRenderDrawColor(rd,0,0,0,255);
    // SDL_RenderDrawRect(rd,nullptr); //  black outline
    SDL_SetRenderDrawColor(rd,255,128,0,0);
    SDL_RenderPresent(rd);
    // SelectedPieces.push_back(3);

MAIN_LOOP:
    ContinueMainLoop = true;

    while (ContinueMainLoop){

        //  更新屏幕
        for (auto i: MapPiecesToUpdate){
            SDL_RenderCopy(rd,imgT, imgPieces+PieceMatch[i] , mapPieces+i);
        }   MapPiecesToUpdate.clear();

        //  接着处理焦点事件
        SDL_SetRenderDrawColor(rd,SelectingFrameColor[0],SelectingFrameColor[1],SelectingFrameColor[2],SelectingFrameColor[3]);
        for (auto i:SelectedPieces){
            SDL_RenderDrawRect(rd,mapPieces+i);
        }

        //  check events
        if (SDL_PollEvent(&evt)){
            switch(evt.type){
            
            case SDL_QUIT:
                SDL_Log("Quit");
                goto FUNCTION_QUIT;
            
            case SDL_MOUSEBUTTONDOWN:
                MouseDownHandler();
                break;
            case SDL_MOUSEBUTTONUP:
                MouseUpHandler();
                break;
            case SDL_MOUSEMOTION:
                MouseMotionHandler();
                break;
            
            default:
                break;
            }
        }

        //  不论如何都刷新屏幕
        SDL_RenderPresent(rd);

        if (CheckPieceMatch(PieceMatch,PieceCount)){
            SelectedPieces.clear();
            ContinueMainLoop = false;
            for (auto i = 0;i<PieceCount;++i){
                SDL_RenderCopy(rd,imgT, imgPieces+PieceMatch[i] , mapPieces+i);
            }
            SDL_RenderPresent(rd);
            Sleep(500);
            system("win.pyw");	//	win
        }
    }

FUNCTION_QUIT:
    SDL_DestroyWindow(wd);    
    return 0;
}

// void sdl_renderDrawThickRect(SDL_Renderer *renderer, const SDL_Rect *rect,Uint8 width, Uint32 color){
//     int xa = rect->x,
//         xb = rect->x + rect->w,
//         ya = rect->y,
//         yb = rect->y + rect->h;
//     thickLineColor(renderer,xa,ya,xb,ya,width,color);   //  upline
//     thickLineColor(renderer,xa,ya,xa,yb,width,color);   //  leftline
//     thickLineColor(renderer,xb,ya,xb,yb,width,color);   //  rightline
//     thickLineColor(renderer,xa,yb,xb,yb,width,color);   //  underline
// }
// void SetRectFrame(SDL_Renderer *renderer, const SDL_Rect *rect){
//     sdl_renderDrawThickRect(renderer,rect,FrameSize,FramColor);
// }


void SplitMap(int w,int h,SDL_Rect* rects,int xCount,int yCount){
    int x = w/xCount;
    int y = h/yCount;
    for (auto i =0 ;i<yCount;++i){
        for (auto j=0;j<xCount;++j){
            rects->x = j*x;
            rects->y = i*y;
            rects->w = x;
            rects->h = y;
            ++ rects;
        }
    }
}

bool CheckPieceMatch(unsigned* matchs,unsigned count){
    for (unsigned i = 0;i<count;++i){
        if (matchs[i] != i) return false;
    }
    return true;
}


bool CheckPointInRect(int x,int y,SDL_Rect* rect){
    return (x>=rect->x) && (y>=rect->y) && (x<rect->x+rect->w) && (y<rect->y+rect->h);
}
unsigned CheckMouseFocus(){
    SDL_GetMouseState(&MouseX,&MouseY);
    for (auto i=0;i<PieceCount;++i){
        if (CheckPointInRect(MouseX,MouseY,mapPieces+i)){
            return i;
        }
    }
    return -1;
}


void MouseDownHandler(){
    MouseDown = true;
    for (auto i :SelectedPieces)
        MapPiecesToUpdate.push_back(i);
    SelectedPieces.clear();

    //  检查选中的格
    unsigned focus = CheckMouseFocus();
    if (focus!=-1)
        SelectedPieces.push_back(focus);
}
void MouseUpHandler(){
    unsigned MovedFocus = CheckMouseFocus();
    if (MouseMovedBeforeRelease && SelectedPieces.size()==1){
        if (MovedFocus != SelectedPieces[0]){
            MapPiecesToUpdate.push_back(SelectedPieces[0]);
            MapPiecesToUpdate.push_back(MovedFocus);
            SwapValue(PieceMatch[MovedFocus],PieceMatch[SelectedPieces[0]]);
            SelectedPieces[0] = MovedFocus;
        }
    }
    MouseDown = false;
    MouseMovedBeforeRelease = false;
}
void MouseMotionHandler(){
    if (MouseDown){
        MouseMovedBeforeRelease = true;
    }
}


void ClearRect(SDL_Rect* r){
    ZeroMmry(r,sizeof(SDL_Rect));
}

/*
dustbin

    //  split
    SDL_SetRenderDrawBlendMode(rd,SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rd,255,255,255,255);
    SDL_RenderFillRect(rd,nullptr);

    SDL_SetRenderDrawColor(rd,0,0,0,255);
    SDL_RenderDrawRect(rd,nullptr); //  black outline
    
    SDL_SetRenderDrawColor(rd,255,128,0,0);
    // SDL_RenderFillRect(rd,&rdGrid);  //  abandoned
    // 改为在rect上方Draw Lines表示选择框

    // SDL_RenderCopy(rd,imgT,&imgGrid,&wdBox);
    // FramColor = SDL_MapRGBA(sf->format, 255, 136, 136, 0);
    // SetRectFrame(rd,&wdBox); 没效果啊？
    // thickLineColor(rd,(int16_t)(wdBox.x),(int16_t)(wdBox.y),(int16_t)(wdBox.x+wdBox.w),(int16_t)(wdBox.y+wdBox.h),(Uint8)5,(Uint16)FramColor);

    // SDL_SetRenderDrawColor(rd,0,0,0,0);
    // SDL_RenderDrawLine(rd,wdBox.x+wdBox.w,wdBox.y,wdBox.x,wdBox.y+wdBox.h);
    SDL_RenderPresent(rd);

MAIN_LOOP:

    imgSf = IMG_Load(imgPath);
    AlertPointer(imgSf);

    SDL_Rect imgBox = {0,0,imgSf->w,imgSf->h};
    SDL_Rect imgGrid = {0,0,imgSf->w / 3, imgSf->h /3};
    // SDL_Rect rdGrid = {0,0,imgGrid.w+12,imgGrid.h+12};   //  abandoned

    //  set & config window
    int ta = imgBox.w , tb = imgBox.h, tc = 0;
    while (ta>1920||ta>1080){
        if (++tc > 5) return 0;
        ta /= 2;    tb /= 2;
    }
    wd = SDL_CreateWindow("t",30,30,ta,tb,SDL_WindowFlags::SDL_WINDOW_OPENGL);
    SDL_Rect wdBox = {0,0,imgGrid.w,imgGrid.h};


*/