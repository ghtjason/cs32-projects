#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, int dir = right, int depth = 0, bool impactable = false, bool replaceable = false);
    virtual void doSomething() = 0;
    virtual void onImpact();
    StudentWorld* getWorld() const;
    bool isActive() const;
    void setInactive();
    bool checkNewActorOnSameSquare(Actor* actor);
    bool isImpactable() const;
    bool isReplaceable() const;
private:
    void setActorOnSquare(Actor* p, bool onSquare);
    bool m_active;
    bool m_peachOnSquare;
    bool m_yoshiOnSquare;
    bool m_isImpactable;
    StudentWorld* m_world;
    bool m_isReplaceable;
};

class MovingActor : public Actor
{
public:
    MovingActor(StudentWorld* world, std::string state, int imageID, int startX, int startY, bool isImpactable, int walkDir = right);
    bool checkCurrDirValid();
    bool checkValidNewDir(int dir);
    void walk();
    void autoChooseNewDir();
    void chooseRandDir();
    void setWalkDir(int dir);
    int getWalkDir() const;
    void setTicks(int ticks);
    int getTicks() const;
    void setState(std::string state);
    std::string getState() const;
    void teleport();
    void setJustTeleported(bool justTeleported);
    bool getJustTeleported() const;
    void swapPosAndMov(MovingActor *p);
    void setAffectedByEvent(bool affected);
    bool getAffectedByEvent() const;
    bool atFork();
private:
    bool checkValidDir(int dir);
    void updateSpriteDir();
    int m_walkDir;
    int ticks_to_move;
    std::string m_state;
    bool m_justTeleported;
    bool m_affectedByEvent;
};

class Player : public MovingActor
{
public:
    Player(StudentWorld* world, int player, int imageID, int startX, int startY);
    virtual void doSomething();
    int getDice() const;
    int getStars() const;
    int getCoins() const;
    bool hasVortex() const;
    void addCoins(int coins);
    void addStars(int stars);
    void giveVortex();
    void setAllowFork(bool fork);
    void resetCoins();
    void resetStars();
    void swapCoins(Player* p);
    void swapStars(Player* p);
private:
    int m_playerNum;
    int m_coins;
    int m_stars;
    bool m_hasVortex;
    bool m_allowFork;
};

class Enemy : public MovingActor
{
public:
    Enemy(StudentWorld* world, int maxSquares, int imageID, int startX, int startY);
    void doSomething();
    virtual void onImpact();
private:
    virtual void newPlayerLandAction(Player *p) = 0;
    virtual void onPause() = 0;
    int m_maxSquares;
    int squares_to_move;
    int m_pauseCounter;
};

class Bowser : public Enemy
{
public:
    Bowser(StudentWorld* world, int imageID, int startX, int startY);
private:
    virtual void newPlayerLandAction(Player *p);
    virtual void onPause();
};

class Boo : public Enemy
{
public:
    Boo(StudentWorld* world, int imageID, int startX, int startY);
private:
    virtual void newPlayerLandAction(Player *p);
    virtual void onPause();
};

class Vortex : public MovingActor
{
public:
    Vortex(StudentWorld* world, int imageID, int startX, int startY, int walkDir);
    virtual void doSomething();
};

class Square : public Actor
{
public:
    Square(StudentWorld* world, int imageID, int startX, int startY, int dir = right);
    virtual void doSomething();
private:
    virtual void newPlayerLandAction(Player* p) = 0;
    virtual void newPlayerMoveAction(Player* p) = 0;
};

class CoinSquare : public Square
{
public:
    CoinSquare(StudentWorld* world, int coins, int imageID, int startX, int startY);
private:
    virtual void newPlayerLandAction(Player* p);
    virtual void newPlayerMoveAction(Player* p);
    int m_coinsToAdd;
    void addCoinsToPlayer(Player* p);
};

class StarSquare : public Square
{
public:
    StarSquare(StudentWorld* world, int imageID, int startX, int startY);
private:
    virtual void newPlayerLandAction(Player* p);
    virtual void newPlayerMoveAction(Player* p);
    void giveStarIfRich(Player* p);
};

class DirectionalSquare : public Square
{
public:
    DirectionalSquare(StudentWorld* world, int imageID, int startX, int startY, int dir);
    virtual void doSomething();
private:
    virtual void newPlayerLandAction(Player* p);
    virtual void newPlayerMoveAction(Player* p);
    int m_forcingDir;
};

class BankSquare : public Square
{
public:
    BankSquare(StudentWorld* world, int imageID, int startX, int startY);
    virtual void newPlayerLandAction(Player* p);
    virtual void newPlayerMoveAction(Player* p);
private:
    void withdrawBank(Player* p);
    void depositBank(Player* p);
};

class EventSquare : public Square
{
public:
    EventSquare(StudentWorld* world, int imageID, int startX, int startY);
private:
    virtual void newPlayerLandAction(Player* p);
    virtual void newPlayerMoveAction(Player* p);
    void randEvent(Player* p);
};

class DroppingSquare : public Square
{
public:
    DroppingSquare(StudentWorld* world, int imageID, int startX, int startY);
private:
    virtual void newPlayerLandAction(Player* p);
    virtual void newPlayerMoveAction(Player* p);
    void randDeduct(Player* p);
};

#endif // ACTOR_H_
