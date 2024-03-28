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
unsigned XTraSize;
unsigned YTraSize;  //  see the line above to get the meaning
unsigned mapSizeX;
unsigned mapSizeY;
unsigned imgPieceCount = 0;
unsigned mapPieceCount = 0;
SDL_Rect *imgPieces = nullptr;
SDL_Rect *mapPieces = nullptr;
unsigned *pieceMatch = nullptr;
unsigned matchedPiecesCount = 0;
std::vector<unsigned> mapPiecesToUpdate;
std::vector<unsigned> selectedPieces;
std::vector<unsigned> multipleSwapBuff; //  clear it before using


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

void Sdl_RenderDrawThickRect(SDL_Renderer *renderer, const SDL_Rect *rect,Uint8 width, Uint32 color);
void SetRectFrame(SDL_Renderer *renderer, const SDL_Rect *rect);

void InitGame(const char* imagePath =imgPath,int x=5,int y=4,int xx = 1,int yy = 1);
void UninitGame();
void SplitMap(int w,int h,SDL_Rect* rects,int xCount,int yCount);
void UpdatePieces();
void UpdatePiece(unsigned i);
bool CheckPieceMatch(unsigned* matchs,unsigned count);
void ShowVictory();
void RandomizePieces();
void AlignPieces();
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
    minimized program
*/
class PicturePuzzle;

int main_puzzle();

# undef main
int main(int argc,char *argv[]){
    // InitActiveCodePage();    //  
    atexit(ExitCheck);          //

    int x=5,y=4;
    int xx=1,yy=1;
    char *p = (char*)imgPath;
    if (argc>1)
        p = argv[1];
    if (argc>3){
        sscanf(argv[2],"%d",&x);    sscanf(argv[3],"%d",&y);
    }
    if (argc>5){
        sscanf(argv[4],"%d",&xx);   sscanf(argv[5],"%d",&yy);
    }
    InitGame(p,x,y,xx,yy);
    return main_puzzle();
}
int main_puzzle(){
    bool win = false;
MAIN_LOOP:
    ContinueMainLoop = true;

    while (ContinueMainLoop){

        //  更新屏幕
        if (mapPiecesToUpdate.size())   UpdatePieces();

        //  接着处理焦点事件
        SDL_SetRenderDrawColor(rd,SelectingFrameColor[0],SelectingFrameColor[1],SelectingFrameColor[2],SelectingFrameColor[3]);
        for (auto i:selectedPieces){
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

        if (CheckPieceMatch(pieceMatch,mapPieceCount)){
            selectedPieces.clear();
            ContinueMainLoop = false;
            for (unsigned i=0;i<mapPieceCount;++i)  mapPiecesToUpdate.push_back(i);
            UpdatePieces();
            SDL_RenderPresent(rd);
            Sleep(300);
            win = true;
            std::thread threadA = std::thread(ShowVictory);
            threadA.detach();
        }
        Sleep(20);
    }

    //  另一个主循环，防止退出
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

void InitGame(const char* imagePath,int x,int y,int xx,int yy){
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_GAMECONTROLLER|SDL_INIT_SENSOR|SDL_INIT_EVENTS)<0)
        exit(0);// goto UNINIT;

    imgSizeX = x;
    imgSizeY = y;
    mapSizeX = x + xx;
    mapSizeY = y + yy;
    imgPieceCount = x*y;
    mapPieceCount = mapSizeX * mapSizeY;
    imgPieces = new SDL_Rect[imgPieceCount];
    mapPieces = new SDL_Rect[mapPieceCount];
    pieceMatch = new unsigned[mapPieceCount];

    AlignPieces();
    for (unsigned i=0;i<mapPieceCount;++i)
        mapPiecesToUpdate.push_back(i);
    
    AlertPointer(imgPieces);AlertPointer(imgPieces);AlertPointer(pieceMatch);
    RandomizePieces();

    //  create img
    imgSf = IMG_Load(imagePath);
    AlertPointer(imgSf);
    imgBox.w = imgSf->w;
    imgBox.h = imgSf->h;

    //  set & config window
    int ta = imgBox.w/imgSizeX*mapSizeX , tb = imgBox.h/imgSizeY*mapSizeY, tc = 0;
    while (ta>1280||tb>720){
        if (++tc > 10) return;
        ta = ta*0.9;    tb = tb*0.9;
    }
    wd = SDL_CreateWindow("t",150,150,ta,tb,SDL_WindowFlags::SDL_WINDOW_OPENGL);
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
    SplitMap(imgSf->w,imgSf->h,imgPieces,imgSizeX,imgSizeY);
    SplitMap(sf->w,sf->h,mapPieces,mapSizeX,mapSizeY);


    SDL_SetRenderDrawBlendMode(rd,SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rd,255,255,255,255);
    SDL_RenderFillRect(rd,nullptr); //  白底
    SDL_SetRenderDrawColor(rd,0,0,0,0x80);  
    SDL_RenderFillRect(rd,nullptr);//  黑色蒙版

    SDL_SetRenderDrawColor(rd,255,128,0,0);
    SDL_RenderPresent(rd);
    // SelectedPieces.push_back(3);
}
void UninitGame(){
    DeletePointer(&imgPieces);
    DeletePointer(&mapPieces);
    DeletePointer(&pieceMatch);
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
void UpdatePieces(){
    unsigned bgColor = matchedPiecesCount/(imgPieceCount/3)*0x10+0x70;
    if (matchedPiecesCount == imgPieceCount)
        SDL_SetRenderDrawColor(rd,0xa0,0xa0,0xa0,0xff);
    else if (matchedPiecesCount == 0)
        SDL_SetRenderDrawColor(rd,0x60,0x60,0x60,0xff);
    else 
        SDL_SetRenderDrawColor(rd,bgColor,bgColor,bgColor,0xff);
    for (auto i:mapPiecesToUpdate)
        UpdatePiece(i);
    mapPiecesToUpdate.clear();
}
void UpdatePiece(unsigned i){
    if (pieceMatch[i] < imgPieceCount){
        SDL_RenderCopy(rd,imgT, imgPieces+pieceMatch[i], mapPieces+i);
    }else{
        SDL_RenderFillRect(rd,mapPieces+i);
    }
}

bool CheckPieceMatch(unsigned* matchs,unsigned count){
    unsigned x,y;
    unsigned c=0;
    for (x=0;x<imgSizeX;++x){
        for (y=0;y<imgSizeY;++y){
            if (pieceMatch[y*mapSizeX+x] == y*imgSizeX+x)
                ++c;
        }
    }
    matchedPiecesCount = c;
    return c == imgPieceCount;
}
void ShowVictory(){
    system("vic.cmd");  //cmd /c 
}
void RandomizePieces(){
    srand(time(0));
    int a,b;
    for (auto i=0;i<mapPieceCount;++i){    //  应该会非常乱
        a = rand()%imgPieceCount;
        b = rand()%imgPieceCount;
        SwapValue(pieceMatch[a/imgSizeX*mapSizeX+a%imgSizeX],pieceMatch[b/imgSizeX*mapSizeX+b%imgSizeX]);
    }
    matchedPiecesCount = 0;
}
void AlignPieces(){
    unsigned x,y;
    memset(pieceMatch,-1,sizeof(unsigned)*mapPieceCount);   //  设置为最大的    unsigned int
    for (x=0;x<imgSizeX;++x){
        for (y=0;y<imgSizeY;++y){
            pieceMatch[y*mapSizeX+x] = y*imgSizeX+x;
        }
    }
    matchedPiecesCount = imgPieceCount;
}

bool CheckPointInRect(int x,int y,SDL_Rect* rect){
    return (x>=rect->x) && (y>=rect->y) && (x<rect->x+rect->w) && (y<rect->y+rect->h);
}
unsigned CheckMouseFocus(){
    SDL_GetMouseState(&MouseX,&MouseY);
    for (auto i=0;i<mapPieceCount;++i){
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
        if (std::find(selectedPieces.begin(),selectedPieces.end(),focus) == selectedPieces.end()){
            for (auto i:selectedPieces)
                mapPiecesToUpdate.push_back(i);
            selectedPieces.clear();
            selectedPieces.push_back(focus);
        }else{  //  点击已经被选中的元素
            if (selectedPieces.size()>1)
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
        if (selectedPieces.size()==1){
            formerFocus = selectedPieces[0];
            int xa,xb;
            int ya,yb;
            int xx,yy;
            xa = ((presentFocus%mapSizeX)<(formerFocus%mapSizeX))?(presentFocus%mapSizeX):(formerFocus%mapSizeX);
            xb = ((presentFocus%mapSizeX)>=(formerFocus%mapSizeX))?(presentFocus%mapSizeX):(formerFocus%mapSizeX);
            ya = ((presentFocus/mapSizeX)<(formerFocus/mapSizeX))?(presentFocus/mapSizeX):(formerFocus/mapSizeX);
            yb = ((presentFocus/mapSizeX)>=(formerFocus/mapSizeX))?(presentFocus/mapSizeX):(formerFocus/mapSizeX);
            
            selectedPieces.clear();
            for (yy=ya;yy<=yb;++yy) //  完成多选
                for (xx=xa;xx<=xb;++xx)
                    selectedPieces.push_back(yy*mapSizeX+xx);
        }
        MultipleSelectionModeEnabled = false;
    }
    else{   //  multipleSelectionMode disabled
        if (MouseMovedBeforeRelease){
            if (selectedPieces.size()==1){   //  普通的交换两个块
                if (MovedFocus != selectedPieces[0]){
                    mapPiecesToUpdate.push_back(selectedPieces[0]);
                    mapPiecesToUpdate.push_back(MovedFocus);
                    SwapValue(pieceMatch[MovedFocus],pieceMatch[selectedPieces[0]]);
                    selectedPieces[0] = MovedFocus;
                }
            }else if (selectedPieces.size() > 1 && std::find(selectedPieces.begin(),selectedPieces.end(),MovedFocus)==selectedPieces.end()){
            //  如果移动后的矩阵与移动前的有重叠，则不可移动
                MoveOffset = mapPieceCount + MovedFocus - MultipleSelectionRepresentive;    //  防止负数溢出，之后需要取模
                if (MoveOffset % mapPieceCount){
                    multipleSwapBuff.clear();
                    
                    for (unsigned &s:selectedPieces){    //  交换
                        if (std::find(selectedPieces.begin(),selectedPieces.end(),(s+MoveOffset) % mapPieceCount)!=selectedPieces.end())
                            goto MOUSE_UP_END;
                        mapPiecesToUpdate.push_back(s);
                        mapPiecesToUpdate.push_back((s+MoveOffset) % mapPieceCount);
                        multipleSwapBuff.push_back(pieceMatch[s]);
                    }
                    for (unsigned &s:selectedPieces){
                        pieceMatch[s] = pieceMatch[(s+MoveOffset) % mapPieceCount];
                        s = (s+MoveOffset) % mapPieceCount;
                    }
                    auto it = multipleSwapBuff.begin();
                    for (unsigned s:selectedPieces){
                        pieceMatch[s] = *it++;
                    }
                }
                else goto RETURN_TO_SINGLE_MODE;
            }
        }else{  //  选择一个但并且没有动
        RETURN_TO_SINGLE_MODE:
            if (selectedPieces.size()>1 && MouseMovedBeforeRelease==false){
                for (auto i:selectedPieces)
                    mapPiecesToUpdate.push_back(i);
                selectedPieces.clear();
                selectedPieces.push_back(CheckMouseFocus());
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

void KeyUpHandler(SDL_Keycode k){  // c63at1n9!
    if (k == SDLK_UP){
        AlignPieces();  //  
    }
}

void ClearRect(SDL_Rect* r){
    ZeroMmry(r,sizeof(SDL_Rect));
}
