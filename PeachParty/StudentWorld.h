#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include <string>
#include <list>

class Actor;
class Player;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    Board& getBoard();
    Player* getPeach() const;
    Player* getYoshi() const;
    int getBank() const;
    void setBank(int coins);
    void replaceWithDropping(int x, int y);
    void newVortex(int x, int y, int dir);
    Actor* vortexImpact(int x, int y);
    int decideWinner();
private:
    std::list<Actor*> actors;
    Player* m_peach;
    Player* m_yoshi;
    Board m_board;
    int m_bank;
    bool overlap(int x1, int y1, int x2, int y2) const;
    void updateGameStatText();
    std::string playerStats(Player* p);
    Player* chooseWinner();
};

#endif // STUDENTWORLD_H_
