/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#define SPRINTF StringUtils::format

#include "cocos2d.h"
#include "audio/include/AudioEngine.h"
#include "ui/CocosGUI.h"

using namespace std;
USING_NS_CC;
using namespace ui;

typedef function<void(int)> FUNC;

extern int bgmID;
extern int seID;
extern Size visibleSize;
extern Vec2 origin;
extern Vec2 center;

void playBGM(string fname);
void playSE(string fname, FUNC callback = nullptr);
void playSEonce(string fname);
Sprite* drawEffect(string fname, int width, int height);

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);


    Vector<SpriteFrame*> run;
    Vector<SpriteFrame*> jump;
    Vector<SpriteFrame*> tired;

    int score;

    bool isJump;
    bool isDJump;
    bool isSliding;
    float downSpeed;
    float life;


    Layer* gameLayer;
    void GameStart();
    void SetTitle();

    void CreateMap(int num);
    void Gameover(Sprite* reddot, Sprite* player);
};

class MenuLayer : public cocos2d::Layer
{
public:
    static MenuLayer* create(int mode);
    virtual bool init();
    int mode;
    FUNC cb;
};
class TitleLayer : public cocos2d::Layer
{
public:
    static TitleLayer* create();
    virtual bool init();
    FUNC cb;
};

#endif // __HELLOWORLD_SCENE_H__
