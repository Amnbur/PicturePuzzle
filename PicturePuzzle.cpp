#include "SDL.h"
#include "SDL_image.h"
#include "SDL_surface.h"
#include "SDL_render.h"

#include "synchapi.h"
#include "normal_header.hpp"
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <thread>

unsigned imgSizeX;
unsigned imgSizeY;
unsigned MapSizeX;
unsigned MapSizeY;
unsigned PieceCount = 0;
SDL_Rect *imgPieces = nullptr;
SDL_Rect *mapPieces = nullptr;
unsigned *PieceMatch = nullptr;
std::vector<unsigned> MapPiecesToUpdate;
std::vector<unsigned> SelectedPieces;
std::vector<unsigned> MultipleSwapBuff; //  clear it before using


// const Uint8 SelectingFrameColor[4] = {237,77,16,233};
const Uint8 SelectingFrameColor[4] = {255,255,255,255};
const char* DefaultPuzzleImage = "./src/img/countryside.png";
const char *imgPath = "./src/img/countryside.png";


SDL_Rect imgBox = {0,0,0,0};
SDL_Rect imgGrid = {0,0,0,0};
SDL_Surface* imgSf;
SDL_Window* wd;
SDL_Surface* sf;
SDL_Renderer* rd;
SDL_Texture* imgT;
SDL_Event evt;
SDL_EventFilter evtFlt;
SDL_Keycode key;
SDL_KeyCode k;

bool ContinueMainLoop = true;
bool MouseDown = false;
bool FrameSize = 10;
bool MultipleSelectionModeEnabled = false;
int MouseX = 0;
int MouseY = 0;
unsigned MultipleSelectionRepresentive = 0;
bool MouseMovedBeforeRelease = false;
Uint32 FramColor = 0xff8888;

void sdl_renderDrawThickRect(SDL_Renderer *renderer, const SDL_Rect *rect,Uint8 width, Uint32 color);
void SetRectFrame(SDL_Renderer *renderer, const SDL_Rect *rect);

void InitGame(const char* imagePath =imgPath,int x=5,int y=4);
void UninitGame();
void SplitMap(int w,int h,SDL_Rect* rects,int xCount,int yCount);
void UpdatePiece();
bool CheckPieceMatch(unsigned* matchs,unsigned count);
void ShowVictory();
void RandomizePieces();
void ExitCheck();

bool CheckPointInRect(int x,int y,SDL_Rect* rect);
unsigned CheckMouseFocus();

void MouseDownHandler();
void MouseUpHandler();
void MouseMotionHandler();
void KeyDownHandler();
void KeyUpHandler();
void KeyUpHandler(SDL_Keycode k);


void ClearRect(SDL_Rect* r);
/*
    abandoned class
    minimized program method used
*/
int main_puzzle();

# undef main
int main(int argc,char *argv[]){
    // InitActiveCodePage();
    atexit(ExitCheck);

    int x=5,y=4;
    char *p = (char*)imgPath;
    if (argc>1)
        p = argv[1];
    if (argc>3){
        sscanf(argv[2],"%d",&x);
        sscanf(argv[3],"%d",&y);
    }
    InitGame(p,x,y);
    return main_puzzle();
}
int main_puzzle(){
    bool win = false;
MAIN_LOOP:
    ContinueMainLoop = true;

    while (ContinueMainLoop){

        //  更新屏幕
        for (auto i: MapPiecesToUpdate){
            SDL_RenderCopy(rd,imgT, imgPieces+PieceMatch[i] , mapPieces+i);
        }   MapPiecesToUpdate.clear();
        // SDL_RenderCopy(,);
        // SDL_CreateTexture();
        // SDL_CreateTextureFromSurface()
        // SDL_FillRect()
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
                goto DestroyWindowAndQuit;
            
            case SDL_MOUSEBUTTONDOWN:
                MouseDownHandler();     break;
            case SDL_MOUSEBUTTONUP:
                MouseUpHandler();       break;
            case SDL_MOUSEMOTION:
                MouseMotionHandler();   break;
            case SDL_KEYDOWN:
                //  KeyDownHandler();       
                if (evt.key.keysym.sym == SDLK_LCTRL)
                    MultipleSelectionModeEnabled = true;   
                break;
            case SDL_KEYUP:
                //  鼠标按下后未抬起不能解除多选模式
                if ((key = evt.key.keysym.sym) == SDLK_LCTRL && MouseDown == false) 
                    MultipleSelectionModeEnabled = false;
                else    KeyUpHandler(key);
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
            Sleep(300);
            win = true;
            std::thread threadA = std::thread(ShowVictory);
            threadA.detach();
        }
        Sleep(20);
    }

    while (win){
        if (SDL_PollEvent(&evt)){
            switch(evt.type){
            case SDL_QUIT:
                goto DestroyWindowAndQuit;
                break;
            default:
                ;
            }
        }
        Sleep(20);
    }

DestroyWindowAndQuit:
    SDL_DestroyWindow(wd);
UNINIT:
    UninitGame();
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

void InitGame(const char* imagePath,int x,int y){
    MapSizeX = x;
    MapSizeY = y;
    PieceCount = MapSizeX * MapSizeY;
    imgPieces = new SDL_Rect[PieceCount];
    mapPieces = new SDL_Rect[PieceCount];
    PieceMatch = new unsigned[PieceCount];

    for (auto i=0;i<PieceCount;++i){
        PieceMatch[i] = i;
        MapPiecesToUpdate.push_back(i);
    }
    AlertPointer(imgPieces);AlertPointer(imgPieces);AlertPointer(PieceMatch);
    RandomizePieces();

        if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_GAMECONTROLLER|SDL_INIT_SENSOR|SDL_INIT_EVENTS)<0)
        exit(0);// goto UNINIT;
    // const char *imgPath = "./src/img/countryside.png";

    //  create img
    imgSf = IMG_Load(imagePath);
    AlertPointer(imgSf);
    imgBox.w = imgSf->w;
    imgBox.h = imgSf->h;

    //  set & config window
    int ta = imgBox.w , tb = imgBox.h, tc = 0;
    while (ta>960||tb>540){
        if (++tc > 10) return;
        ta = ta*0.9;    tb = tb*0.9;
    }
    wd = SDL_CreateWindow("t",200,200,ta,tb,SDL_WindowFlags::SDL_WINDOW_OPENGL);
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
    SplitMap(imgSf->w,imgSf->h,imgPieces,MapSizeX,MapSizeY);
    SplitMap(sf->w,sf->h,mapPieces,MapSizeX,MapSizeY);


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
}
void UninitGame(){
    DeletePointer(&imgPieces);
    DeletePointer(&mapPieces);
    DeletePointer(&PieceMatch);
}
void ExitCheck(){
    SDL_Log("quit");
}

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
void UpdatePiece(){

}

bool CheckPieceMatch(unsigned* matchs,unsigned count){
    for (unsigned i = 0;i<count;++i){
        if (matchs[i] != i) return false;
    }
    return true;
}
void ShowVictory(){
    // std::fstream in("./src/txt/VictoryInfo.txt"); 
    // std::istreambuf_iterator<char> beg(in), end; 
    // std::string str(beg, end); 
    // in.close();
    // SDL_MessageBoxData data={   SDL_MESSAGEBOX_INFORMATION,
    //     wd,
    //     "INFO", 
    //     (str+std::string("\n\n\npress esc to quit")).c_str()
    // };
    // SDL_ShowMessageBox(&data,NULL);
    system("vic.cmd");  //cmd /c 
}
void RandomizePieces(){
    srand(time(0));
    int a,b;
    for (auto i=0;i<PieceCount;++i){    //  应该会非常乱
        a = rand()%PieceCount;
        b = rand()%PieceCount;
        SwapValue(PieceMatch[a],PieceMatch[b]);
    }
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
    //  检查选中的格
    unsigned focus = CheckMouseFocus();
    if (focus!=-1){
        //  取消选择之前选中的
        if (std::find(SelectedPieces.begin(),SelectedPieces.end(),focus) == SelectedPieces.end()){
            for (auto i:SelectedPieces)
                MapPiecesToUpdate.push_back(i);
            SelectedPieces.clear();
            SelectedPieces.push_back(focus);
        }else{  //  点击已经被选中的元素
            if (SelectedPieces.size()>1)
                MultipleSelectionRepresentive = focus;
        }
    }
}
void MouseUpHandler(){
    unsigned MovedFocus = CheckMouseFocus();
    unsigned MoveOffset;
    if (MultipleSelectionModeEnabled){
        unsigned presentFocus = CheckMouseFocus();
        unsigned formerFocus;
        if (SelectedPieces.size()==1){
            formerFocus = SelectedPieces[0];
            int xa,xb;
            int ya,yb;
            int xx,yy;
            xa = ((presentFocus%MapSizeX)<(formerFocus%MapSizeX))?(presentFocus%MapSizeX):(formerFocus%MapSizeX);
            xb = ((presentFocus%MapSizeX)>=(formerFocus%MapSizeX))?(presentFocus%MapSizeX):(formerFocus%MapSizeX);
            ya = ((presentFocus/MapSizeX)<(formerFocus/MapSizeX))?(presentFocus/MapSizeX):(formerFocus/MapSizeX);
            yb = ((presentFocus/MapSizeX)>=(formerFocus/MapSizeX))?(presentFocus/MapSizeX):(formerFocus/MapSizeX);
            
            SelectedPieces.clear();
            for (yy=ya;yy<=yb;++yy) //  完成多选
                for (xx=xa;xx<=xb;++xx)
                    SelectedPieces.push_back(yy*MapSizeX+xx);
        }
        MultipleSelectionModeEnabled = false;
    }
    else{   //  multipleSelectionMode disabled
        if (MouseMovedBeforeRelease){
            if (SelectedPieces.size()==1){   //  普通的交换两个块
                if (MovedFocus != SelectedPieces[0]){
                    MapPiecesToUpdate.push_back(SelectedPieces[0]);
                    MapPiecesToUpdate.push_back(MovedFocus);
                    SwapValue(PieceMatch[MovedFocus],PieceMatch[SelectedPieces[0]]);
                    SelectedPieces[0] = MovedFocus;
                }
            }else if (SelectedPieces.size() > 1 && std::find(SelectedPieces.begin(),SelectedPieces.end(),MovedFocus)==SelectedPieces.end()){
            //  如果移动后的矩阵与移动前的有重叠，则不可移动
                MoveOffset = PieceCount + MovedFocus - MultipleSelectionRepresentive;    //  防止负数溢出，之后需要取模
                if (MoveOffset % PieceCount){
                    MultipleSwapBuff.clear();
                    
                    for (unsigned &s:SelectedPieces){    //  交换
                        if (std::find(SelectedPieces.begin(),SelectedPieces.end(),(s+MoveOffset) % PieceCount)!=SelectedPieces.end())
                            goto MOUSE_UP_END;
                        MapPiecesToUpdate.push_back(s);
                        MapPiecesToUpdate.push_back((s+MoveOffset) % PieceCount);
                        MultipleSwapBuff.push_back(PieceMatch[s]);
                    }
                    for (unsigned &s:SelectedPieces){
                        PieceMatch[s] = PieceMatch[(s+MoveOffset) % PieceCount];
                        s = (s+MoveOffset) % PieceCount;
                    }
                    auto it = MultipleSwapBuff.begin();
                    for (unsigned s:SelectedPieces){
                        PieceMatch[s] = *it++;
                    }
                }
                else goto RETURN_TO_SINGLE_MODE;
            }
        }else{  //  选择一个但并且没有动
        RETURN_TO_SINGLE_MODE:
            if (SelectedPieces.size()>1 && MouseMovedBeforeRelease==false){
                for (auto i:SelectedPieces)
                    MapPiecesToUpdate.push_back(i);
                SelectedPieces.clear();
                SelectedPieces.push_back(CheckMouseFocus());
            }
        }
    }
MOUSE_UP_END:
    MouseDown = false;
    MouseMovedBeforeRelease = false;
}

void MouseMotionHandler(){
    if (MouseDown){
        MouseMovedBeforeRelease = true;
    }
}
// void KeyDownHandler(){   弃用
//     int keyNum;
//     SDL_GetKeyboardState(&keyNum);
// }
// void KeyUpHandler(){
// }

void KeyUpHandler(SDL_Keycode k){  // c63at1n9!
    if (k == SDLK_UP){
        for (auto i=0;i<PieceCount;++i){
            PieceMatch[i] = i;
            MapPiecesToUpdate.push_back(i);
        }
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

    imgGrid = {0,0,imgSf->w / 3, imgSf->h /3};
    // SDL_Rect rdGrid = {0,0,imgGrid.w+12,imgGrid.h+12};   //  abandoned
        SDL_Rect wdBox = {0,0,imgGrid.w,imgGrid.h};

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


// for (auto s=SelectedPieces.begin();s!=SelectedPieces.end();++s){    //  交换
                    //     MultipleSwapBuff.push_back(PieceMatch[*s]);
                    //     PieceMatch[*s] = PieceMatch[(*s+MoveOffset) % PieceCount];
*/