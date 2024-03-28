Languages : 	[English](#English README message)、[简体中文](#简体中文自述信息)

### **English README message**

Its a simple Picture Puzzle game.  
What makes it a little different is that it is going to allow the gamer to move multiple pieces in one step, and there will be extra space for the gamer to place pieces temporarily.

#### Move Several Pieces  

To move pieces. use the `left_ctrl` key to select them. Note that the positions of these pieces before and after movement should not overlap. This is forbidden because it may cause something unwanted.

#### Use Another Background image  

To use another picture or change level, pass the path of the picture to the program as the first parameter, or edit ` StartGame.cmd` and start the game with this script.  
Of course any other pictures of your own can be used if you configure it in the same way.

#### Difficulty

The second and the third parameters determine how many pieces the picture is to be divided into.

#### Extra space

The forth and fifth parameters determine how much extra space there is.

#### Command Line  

parameters with "[]" are optional  
skim `StartGame.cmd` as an example

```cmd
PicturePuzzle [imgPath] [xSize] [ySize] [XtraSize] [YtraSize]
	imgPath: path of the image used by the Puzzle
	xSize:	the picture is devided into "xSize" parts in the horizontal direction
	ySize:	similar to the parameter xSize but in another direction
	XtraSize:	extra size :) in the horizontal direction
	YtraSize:	similar to XtraSize (though it doesnot sound like a word or a phrase)
```

#### Project on Github: 

[ PicturePuzzle ](https://github.com/Amnbur/PicturePuzzle/tree/trunk)

### **简体中文自述信息**

简单的拼图游戏，稍稍不一样的地方在于可以同时移动多块拼图，而且可以使用更多空间来进行腾挪交换。

#### 同时移动多块拼图

按住左Ctrl键，然后使用鼠标左键并且移动鼠标并且在所需位置松开鼠标，从而选中多片拼图；然后可以移动这些拼图。注意：多片拼图移动前和移动后的位置不能重叠，这样的移动被程序禁止。

#### 更换拼图图片

在命令行窗口中传入第一个参数为图片路径，或者在`StartGame.cmd`脚本中编辑，并且使用脚本启动游戏。

#### 难度

第二、三个参数决定图片的划分方式，参数越小，划分越粗，难度越低。

#### 额外空间

第四、五个参数决定额外空间的大小。

#### 命令行

带有“[]”的参数是可选的  
查看StartGame.cmd作为示例  

```cmd
PicturePuzzle [imgPath] [xSize] [ySize] [XtraSize] [YtraSize]
	imgPath: 图片路径
	xSize:	水平方向划分块数量
	ySize:	垂直方向划分块数量
	XtraSize:	水平方向额外块数量 (Extra) (⊙o⊙)
	YtraSize:	垂直方向额外块数量
```

<span style="color:#cdcdcd">Probably no one will notice it.</span>