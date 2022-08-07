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

#include "HelloWorldScene.h"

USING_NS_CC;
using namespace std;
using namespace ui;

Size visibleSize;
Vec2 origin;
Vec2 center;
int bgmID;
int seID;

void playBGM(string fname)
{
    if (bgmID != AudioEngine::INVALID_AUDIO_ID)
        AudioEngine::stop(bgmID);
    bgmID = AudioEngine::play2d("sound/" + fname, 0.5f);
}
void playSE(string fname, FUNC callback)
{
    if (seID == AudioEngine::INVALID_AUDIO_ID)
    {
        seID = AudioEngine::play2d("sound/" + fname, false, 1.0f);
        AudioEngine::setFinishCallback(seID, [=](int id, string key) {
            seID = AudioEngine::INVALID_AUDIO_ID;
            if (callback != nullptr)
                callback(0);
            });
    }
}
void playSEonce(string fname)
{
    AudioEngine::play2d("sound/" + fname, false, 1.0f);
}
Sprite* drawEffect(string fname, int width, int height)
{
    Sprite* effect = Sprite::create(fname);;
    effect->getTexture()->setAliasTexParameters();
    float _wd = effect->getContentSize().width / width;
    float _ht = effect->getContentSize().height / height;
    Vector<SpriteFrame*> frames;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            string name = SPRINTF("%s_%d", fname.c_str(), x + y * width);
            auto frm = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
            if (frm)
                frames.pushBack(frm);
            else
            {
                frm = SpriteFrame::createWithTexture(effect->getTexture(), Rect(x * _wd, y * _ht, _wd, _ht));
                SpriteFrameCache::getInstance()->addSpriteFrame(frm, name);
                frames.pushBack(frm);
            }
        }
    }
    effect->setTextureRect(Rect(0, 0, _wd, _ht));
    effect->runAction(Sequence::create(
        Animate::create(Animation::createWithSpriteFrames(frames, 0.07f)),
        RemoveSelf::create(),
        NULL
    ));
    return effect;
}

Scene* GameScene::createScene()
{
    return GameScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    center = origin + Vec2(visibleSize * 0.5f);;

    bgmID = AudioEngine::INVALID_AUDIO_ID;
    seID = AudioEngine::INVALID_AUDIO_ID;

    auto bg = Sprite::create("background.png");
    bg->getTexture()->setAliasTexParameters();
    bg->setName("BG");
    bg->setPosition(center);
    addChild(bg);
    float ratio = visibleSize.width / bg->getContentSize().width;
    if (ratio < visibleSize.height / bg->getContentSize().height)
        ratio = visibleSize.height / bg->getContentSize().height;
    bg->setScale(ratio);

    Vector<SpriteFrame*> river;
    river.pushBack(SpriteFrame::create("river.png", Rect(0, 0, 160, 32)));
    river.pushBack(SpriteFrame::create("river.png", Rect(0, 32, 160, 32)));
    auto rv = Sprite::create("river.png", Rect(0, 0, 80, 32));
    rv->getTexture()->setAliasTexParameters();
    rv->setPosition(80, 30);
    bg->addChild(rv);
    rv->runAction(RepeatForever::create(
        Animate::create(Animation::createWithSpriteFrames(river, 0.5f))
    ));

    gameLayer = Layer::create();
    addChild(gameLayer);

    SetTitle();

    return true;
}

void GameScene::GameStart()
{
    Sprite* bg = (Sprite*)getChildByName("BG");

    CreateMap(1, 0);

    auto chr = Sprite::create("twiggy.png");
    chr->getTexture()->setAliasTexParameters();
    int x = (int)(chr->getContentSize().width / 32);
    int y = (int)(chr->getContentSize().height / 32);
    for (int _y = 0; _y < y; _y++)
    {
        for (int _x = 0; _x < x; _x++)
        {
            SpriteFrameCache::getInstance()->addSpriteFrame(SpriteFrame::createWithTexture(chr->getTexture(), Rect(_x * 32, _y * 32, 32, 32)), SPRINTF("CHR_00_%02d", _x + _y * x));
        }
    }

    vector<int> runs = {
        13,14,15,16,17,18
    };
    vector<int> jumps = {
        6, 7, 8, 9
    };
    vector<int> tires = {
        20
    };
    vector<int> sliding = {
        21, 22
    };
    run.clear();
    jump.clear();
    tired.clear();
    slide.clear();
    for (const auto num : runs)
    {
        run.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(SPRINTF("CHR_00_%02d", num)));
    }
    for (const auto num : jumps)
    {
        jump.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(SPRINTF("CHR_00_%02d", num)));
    }
    for (const auto num : tires)
    {
        tired.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(SPRINTF("CHR_00_%02d", num)));
    }
    for (const auto num : sliding)
    {
        slide.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(SPRINTF("CHR_00_%02d", num)));
    }

    score = 0;

    downSpeed = 0;
    isJump = false;
    isDJump = false;
    isSliding = false;
    life = 100.0f;
    invincible = 0;

    auto lifebase = Sprite::create("lifebase.png");
    lifebase->setPosition(center + Vec2(0, 86));
    gameLayer->addChild(lifebase, 2);

    auto lifebar = Sprite::create("lifebar.png");
    float lbarwidth = lifebar->getContentSize().width;
    float lbarheight = lifebar->getContentSize().height;
    lifebar->setPosition(center + Vec2(lifebar->getContentSize().width * (-0.5f), 86));
    lifebar->setAnchorPoint(Vec2(0, 0.5f));
    gameLayer->addChild(lifebar, 2);
    lifebar->schedule([=](float dt) {
        lifebar->setTextureRect(Rect(0, 0, lbarwidth * life / 100.0f, lbarheight));
        if (life <= 0)
        {
            bg->stopAllActions();
            bg->runAction(TintTo::create(1.0f, Color3B(60, 60, 60)));
            lifebar->unschedule("LIFE");
        }
        else
        {
            if (lifebar->getContentSize().width / lbarwidth < 0.3f)
            {
                if (bg->getNumberOfRunningActions() == 0)
                {
                    bg->runAction(RepeatForever::create(Sequence::create(
                        TintTo::create(0.5f, Color3B(255, 128, 128)),
                        TintTo::create(0.5f, Color3B::WHITE),
                        NULL)));
                }
            }
            else
            {
                bg->stopAllActions();
                bg->setColor(Color3B::WHITE);
            }
        }
        }, "LIFE");

    auto scorelbl = Label::createWithBMFont("pressstart_.txt", "SCORE 00000");
    scorelbl->setName("SCORE");
    scorelbl->setBMFontSize(8);
    scorelbl->setColor(Color3B::YELLOW);
    scorelbl->setPosition(center + Vec2(10, 70));
    scorelbl->setAnchorPoint(Vec2(0, 0.5));
    gameLayer->addChild(scorelbl, 2);

    auto pausebtn = Button::create("btn_pause.png");
    pausebtn->setPosition(origin + Vec2(visibleSize) + Vec2(-12, -12));
    gameLayer->addChild(pausebtn, 2);
    pausebtn->addTouchEventListener([=](Ref* sender, Widget::TouchEventType event) {
        Button* button = dynamic_cast<Button*>(sender);
        if (event == Widget::TouchEventType::ENDED)
        {
            Director::getInstance()->pause();
            auto menu = MenuLayer::create(0);
            menu->cb = [=](int ret) {
                if (ret == 1)
                {
                    Director::getInstance()->resume();
                    menu->removeFromParent();
                }
                else if (ret == 0)
                {
                    gameLayer->removeAllChildrenWithCleanup(true);
                }
            };
            gameLayer->addChild(menu, 4);

        }
        });

    auto reddot = Sprite::create("reddot.png");
    reddot->setPosition(origin + Vec2(80, 48));
    reddot->setName("REDDOT");
    gameLayer->addChild(reddot, 1);

    auto reddothead = Sprite::create("reddot.png");
    reddothead->setPosition(1, 14);
    reddothead->setName("REDDOTHEAD");
    reddot->addChild(reddothead, 1);

    auto player = Sprite::createWithSpriteFrameName("CHR_00_00");
    //player->setAnchorPoint(Vec2(0.5f, 0.0f));
    player->setPosition(1, 24);
    player->setScale(1.5f);
    player->setName("PLAYER");
    reddot->addChild(player);

    Vector<Node*> dots;
    dots.pushBack(reddot);
    dots.pushBack(reddothead);

    player->runAction(RepeatForever::create(
        Animate::create(Animation::createWithSpriteFrames(run, 0.06f))
    ));
    reddot->schedule([=](float dt) {

        if (reddot->getPosition().y < -100)
        {
            life -= 10.0f;
            if (life < 0)
                life = 0;
            return;
        }

        reddot->setPosition(reddot->getPosition() + Vec2(0, downSpeed));
        reddothead->setPosition(1, 14);
        downSpeed -= 0.09f;
        life -= 0.05f;

        if (life <= 0)
        {
            life = 0;
        }

        TMXTiledMap* map = (TMXTiledMap*)gameLayer->getChildByName("MAP");
        auto layer = map->getLayer("terrain");
        auto itemLayer = map->getLayer("item");
        auto obsLayer = map->getLayer("obs");
        for (const auto dot : dots)
        {
            Vect _pos = dot->getPosition();
            Vec2 dorpos = dot->getParent()->convertToWorldSpace(dot->getPosition());
            int tileX = (int)(map->convertToNodeSpace(dorpos).x / map->getTileSize().width);
            int tileY = (int)(map->getMapSize().height - map->convertToNodeSpace(dorpos).y / map->getTileSize().height);
            if ((0 <= tileX && tileX < map->getMapSize().width) &&
                (0 <= tileY && tileY < map->getMapSize().height))
            {
                int gid = layer->getTileGIDAt(Vec2(tileX, tileY));
                if (downSpeed < 0 && (17 <= gid && gid <= 19))
                {
                    if (isJump || isDJump)
                    {
                        player->stopActionByTag(0);
                        player->setRotation(0);
                        player->runAction(RepeatForever::create(
                            Animate::create(Animation::createWithSpriteFrames(run, 0.06f))
                        ));
                    }
                    downSpeed = 0;
                    isJump = false;
                    isDJump = false;
                    if ((int)dot->getPosition().y % 16 != 0)
                        dot->setPositionY((int)(dot->getPosition().y / 16 + 1) * 16);

                    if (isSliding)
                    {
                        auto slideani = player->getActionByTag(1);
                        if (!slideani)
                        {
                            player->stopAllActions();
                            auto slidingaction = RepeatForever::create(
                                Animate::create(Animation::createWithSpriteFrames(slide, 0.1f))
                            );
                            slidingaction->setTag(1);
                            player->runAction(slidingaction);
                        }
                        reddothead->setPosition(1, 2);
                    }

                    if (life == 0)
                    {
                        Gameover(reddot, player);
                        break;
                    }
                }

                int tileY2 = (int)(map->getMapSize().height - (map->convertToNodeSpace(dorpos).y + (12)) / map->getTileSize().height);
                //int tileY2 = (int)(map->getMapSize().height - (map->convertToNodeSpace(dot->getPosition()).y) / map->getTileSize().height);
                int itemgid = itemLayer->getTileGIDAt(Vec2(tileX, tileY2));
                if (2 <= itemgid && itemgid <= 8)
                {
                    itemLayer->setTileGID(1, Vec2(tileX, tileY2));
                    if (itemgid == 4)
                    {
                        score += 100;
                        life += 10;
                        if (life >= 100)
                            life = 100;
                        playSEonce("se_recovery.mp3");
                        auto effect = drawEffect("4.png", 5, 1);
                        effect->setPosition(tileX * map->getTileSize().width, (map->getMapSize().height - tileY) * map->getTileSize().height);
                        map->addChild(effect);
                    }
                    else
                    {
                        score += itemgid * 5;
                        playSEonce("effect_08.mp3");
                        auto effect = drawEffect("2.png", 5, 1);
                        effect->setPosition(tileX * map->getTileSize().width, (map->getMapSize().height - tileY) * map->getTileSize().height);
                        map->addChild(effect);
                    }
                    scorelbl->setString(SPRINTF("SCORE %05d", score));
                }
                if (invincible == 0)
                {
                    int obsid = obsLayer->getTileGIDAt(Vec2(tileX, tileY2));
                    if (obsid >= 49)
                    {
                        life -= 5;
                        invincible = 1.0f;
                        player->setOpacity(150);
                        auto effect = drawEffect("5.png", 5, 1);
                        effect->setPosition(dorpos);
                        gameLayer->addChild(effect);
                        playSEonce("deffend_A1.mp3");
                    }
                }
                else
                {
                    invincible -= dt;
                    if (invincible <= 0)
                    {
                        invincible = 0;
                        player->setOpacity(255);
                    }
                }
                
            }
        }
        }, "PLAYER");

    auto jumpbtn = Sprite::create("jump.png");
    jumpbtn->setName("JUMP");
    jumpbtn->setPosition(origin + Vec2(65, 20));
    jumpbtn->setOpacity(120);
    jumpbtn->setScale(0.8f);
    gameLayer->addChild(jumpbtn, 2);

    auto slidebtn = Sprite::create("slide.png");
    slidebtn->setName("SLIDE");
    slidebtn->setPosition(origin + Vec2(visibleSize.width - 65, 20));
    slidebtn->setOpacity(120);
    slidebtn->setScale(0.8f);
    gameLayer->addChild(slidebtn, 2);

    auto touchEvent = EventListenerTouchOneByOne::create();
    touchEvent->onTouchBegan = [=](Touch* touch, Event* event) {

        if (life <= 0)
            return false;

        auto loc = this->convertToNodeSpace(touch->getLocation());

        if (loc.x < visibleSize.width * 0.5f)
        {
            pushJump();
        }
        else
            if (loc.x > visibleSize.width * 0.5f)
            {
                pushSlide();
            }
        return true;
    };
    touchEvent->onTouchEnded = [=](Touch* touch, Event* event) {
        if (isJump)
        {
            releaseJump();
        }
        else
            if (isSliding)
            {
                releaseSlide();
            }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchEvent, this);

    auto keyEvent = EventListenerKeyboard::create();
    keyEvent->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
            pushJump();
        else
            if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
                pushSlide();
    };
    keyEvent->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
            releaseJump();
        else
            if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
                releaseSlide();
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyEvent, this);

    playBGM("movingrightalong.mp3");
}
void GameScene::SetTitle()
{
    auto title = TitleLayer::create();
    title->cb = [=](int ret) {
        if (ret == 0)
        {
            title->removeFromParent();
            GameStart();
        }
    };
    addChild(title);
}
void GameScene::CreateMap(int num, float adj)
{
    auto map = TMXTiledMap::create(SPRINTF("stage_%02d.tmx", num));
    map->setPosition(origin + Vec2(adj, 0));
    map->setName("MAP");
    gameLayer->addChild(map);
    map->runAction(RepeatForever::create(
        MoveBy::create(0.25f, Vec2(-16, 0))
    ));
    map->schedule([=](float dt) {
        score++;
        ((Label*)gameLayer->getChildByName("SCORE"))->setString(SPRINTF("SCORE %05d", score));

        if (map->getPositionX() < (-1) * (map->getMapSize().width * map->getTileSize().width * map->getScale() - visibleSize.width))
        {
            float _adjx = (-1) * (map->getMapSize().width * map->getTileSize().width * map->getScale() - visibleSize.width) - map->getPositionX();
            map->removeFromParent();
            CreateMap(num, _adjx);
        }
        }, "MAP");
}

void GameScene::Gameover(Sprite* reddot, Sprite* player)
{
    gameLayer->getChildByName("JUMP")->setVisible(false);
    gameLayer->getChildByName("SLIDE")->setVisible(false);

    AudioEngine::stopAll();

    playSEonce("se_kaidan_2.mp3");

    _eventDispatcher->removeAllEventListeners();
    gameLayer->getChildByName("MAP")->stopAllActions();
    gameLayer->getChildByName("MAP")->unschedule("MAP");
    reddot->unschedule("PLAYER");
    player->stopAllActions();
    player->runAction(Sequence::create(
        Animate::create(Animation::createWithSpriteFrames(tired, 0.2f)),
        DelayTime::create(1.0f),
        CallFunc::create([=](void) {
            auto menu = MenuLayer::create(1);
            menu->cb = [=](int ret) {
                if (ret == 0)
                    Director::getInstance()->replaceScene(GameScene::create());
            };
            addChild(menu, 4);
            }),
        NULL
    ));
}
void GameScene::pushJump()
{
    Sprite* player = (Sprite*)gameLayer->getChildByName("REDDOT")->getChildByName("PLAYER");
    
    if (!isJump)
    {
        isJump = true;
        gameLayer->getChildByName("JUMP")->setOpacity(255);
        downSpeed = 2.5f;
        player->stopAllActions();
        player->runAction(
            Animate::create(Animation::createWithSpriteFrames(jump, 0.15f))
        );
        playSEonce("effect_14.mp3");
    }
    else
        if (isJump && !isDJump)
        {
            isDJump = true;
            gameLayer->getChildByName("JUMP")->setOpacity(255);
            downSpeed = 2.5f;
            auto roll = RotateBy::create(1.0f, 360);
            roll->setTag(0);
            player->runAction(roll);
            playSEonce("effect_14.mp3");
        }
}
void GameScene::pushSlide()
{
    if (!isSliding)
    {
        isSliding = true;
        gameLayer->getChildByName("SLIDE")->setOpacity(255);
    }
}
void GameScene::releaseJump()
{
    gameLayer->getChildByName("JUMP")->setOpacity(120);
}
void GameScene::releaseSlide()
{
    Sprite* player = (Sprite*)gameLayer->getChildByName("REDDOT")->getChildByName("PLAYER");

    gameLayer->getChildByName("SLIDE")->setOpacity(120);
    isSliding = false;
    auto jumping = player->getActionByTag(0);
    if (!jumping)
    {
        player->stopAllActions();
        player->runAction(RepeatForever::create(
            Animate::create(Animation::createWithSpriteFrames(run, 0.06f))
        ));
    }
}


MenuLayer* MenuLayer::create(int mode)
{
    MenuLayer* layer = new MenuLayer();
    if (layer)
    {
        layer->mode = mode;
        if (layer->init())
            return layer;
    }
    CC_SAFE_DELETE(layer);
    return NULL;
}
bool MenuLayer::init()
{
    if (!Layer::init())
        return false;

    cb = nullptr;

    auto window = Scale9Sprite::create("popupwindow.png");
    window->setPosition(center);
    window->setCapInsets(Rect(13, 13, 68, 64));
    window->setContentSize(Size(180, 120));
    addChild(window);

    auto gameovermsg = Label::createWithBMFont("pressstart_.txt", "GAME OVER");
    gameovermsg->setBMFontSize(14);
    gameovermsg->setPosition(90, 90);
    gameovermsg->setColor(Color3B(240, 140, 150));
    window->addChild(gameovermsg);
    if (mode == 0)
    {
        gameovermsg->setString("PAUSE");
        gameovermsg->setColor(Color3B(240, 240, 120));
    }

    auto restarter = Button::create("buton_green.png");
    restarter->setScale9Enabled(true);
    restarter->setCapInsets(Rect(4, 4, 8, 6));
    restarter->setContentSize(Size(80, 20));
    restarter->setPosition(Vec2(90, 48));
    window->addChild(restarter);
    restarter->addTouchEventListener([=](Ref* sender, Widget::TouchEventType event) {
        Button* button = dynamic_cast<Button*>(sender);
        if (event == Widget::TouchEventType::ENDED)
        {
            if (cb != nullptr)
                cb(0);
        }
        });
    auto restartmsg = Label::createWithBMFont("pressstart_.txt", "RESTART");
    restartmsg->setBMFontSize(8);
    restarter->setTitleLabel(restartmsg);

    auto closer = Button::create("buton_green.png");
    closer->setScale9Enabled(true);
    closer->setCapInsets(Rect(4, 4, 8, 6));
    closer->setContentSize(Size(80, 20));
    closer->setPosition(Vec2(90, 24));
    window->addChild(closer);
    closer->addTouchEventListener([=](Ref* sender, Widget::TouchEventType event) {
        Button* button = dynamic_cast<Button*>(sender);
        if (event == Widget::TouchEventType::ENDED)
        {
            Director::getInstance()->end();
        }
        });
    auto closemsg = Label::createWithBMFont("pressstart_.txt", "QUIT");
    closemsg->setBMFontSize(8);
    closer->setTitleLabel(closemsg);

    if (mode == 0)
    {
        auto xbtn = Button::create("buton_green.png");
        xbtn->setScale9Enabled(true);
        xbtn->setCapInsets(Rect(4, 4, 8, 6));
        xbtn->setContentSize(Size(20, 20));
        xbtn->setPosition(Vec2(160, 100));
        window->addChild(xbtn);
        xbtn->addTouchEventListener([=](Ref* sender, Widget::TouchEventType event) {
            Button* button = dynamic_cast<Button*>(sender);
            if (event == Widget::TouchEventType::ENDED)
            {
                if (cb != nullptr)
                    cb(1);
            }
            });
        auto xmsg = Label::createWithBMFont("pressstart_.txt", "X");
        xmsg->setBMFontSize(8);
        xbtn->setTitleLabel(xmsg);
    }

    return true;
}
TitleLayer* TitleLayer::create()
{
    TitleLayer* layer = new TitleLayer();
    if (layer)
    {
        if (layer->init())
            return layer;
    }
    CC_SAFE_DELETE(layer);
    return NULL;
}
bool TitleLayer::init()
{
    if (!Layer::init())
        return false;

    auto logo = Sprite::create("titlelogo.png");
    logo->setPosition(center + Vec2(0, 20));
    addChild(logo);

    auto restarter = Button::create("buton_green.png");
    restarter->setScale9Enabled(true);
    restarter->setCapInsets(Rect(4, 4, 8, 6));
    restarter->setContentSize(Size(80, 20));
    restarter->setPosition(center + Vec2(-50, -60));
    addChild(restarter);
    restarter->addTouchEventListener([=](Ref* sender, Widget::TouchEventType event) {
        Button* button = dynamic_cast<Button*>(sender);
        if (event == Widget::TouchEventType::ENDED)
        {
            if (cb != nullptr)
                cb(0);
        }
        });
    auto restartmsg = Label::createWithBMFont("pressstart_.txt", "START");
    restartmsg->setBMFontSize(8);
    restarter->setTitleLabel(restartmsg);

    auto closer = Button::create("buton_green.png");
    closer->setScale9Enabled(true);
    closer->setCapInsets(Rect(4, 4, 8, 6));
    closer->setContentSize(Size(80, 20));
    closer->setPosition(center + Vec2(50, -60));
    addChild(closer);
    closer->addTouchEventListener([=](Ref* sender, Widget::TouchEventType event) {
        Button* button = dynamic_cast<Button*>(sender);
        if (event == Widget::TouchEventType::ENDED)
        {
            Director::getInstance()->end();
        }
        });
    auto closemsg = Label::createWithBMFont("pressstart_.txt", "QUIT");
    closemsg->setBMFontSize(8);
    closer->setTitleLabel(closemsg);

    playBGM("wyver9_Funny Chase (8-bit).mp3");

    return true;
}

/*
https://opengameart.org/content/8bit-adventure
https://opengameart.org/content/8-bit-theme-moving-right-along
https://opengameart.org/content/funny-chase-8-bit-chiptune
*/