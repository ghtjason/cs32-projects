#include "Actor.h"
#include "StudentWorld.h"
#include <string>
#include <algorithm>
using namespace std;


// Actor
Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, int dir, int depth, bool impactable, bool replaceable)
: GraphObject(imageID, startX, startY, dir, depth)
{
    m_world = world;
    m_active = true;
    m_peachOnSquare = false;
    m_yoshiOnSquare = false;
    m_isImpactable = impactable;
    m_isReplaceable = replaceable;
}

void Actor::onImpact() {};

StudentWorld* Actor::getWorld() const { return m_world; }

bool Actor::isActive() const { return m_active; }

void Actor::setInactive() { m_active = false; }

bool Actor::isImpactable() const { return m_isImpactable; }

bool Actor::isReplaceable() const { return m_isReplaceable; }

bool Actor::checkNewActorOnSameSquare(Actor* actor)  // check if an Actor has newly moved on the same square
{
    if((getX() == actor->getX()) && (getY() == actor->getY()))            // if coords match
    {
        // if actor isn't already on the square
        if((actor == m_world->getPeach() && !m_peachOnSquare) || (actor == m_world->getYoshi() && !m_yoshiOnSquare))
        {
            setActorOnSquare(actor, true);
            return true;
        }

        else
            return false;   // same coords, but actor has already landed
    }
    else
    {
        setActorOnSquare(actor, false);
        return false;
    }
}

void Actor::setActorOnSquare(Actor* p, bool onSquare)
{
    if(p == m_world->getPeach())
        m_peachOnSquare = onSquare;
    else if(p == m_world->getYoshi())
        m_yoshiOnSquare = onSquare;
}

// MovingActor
MovingActor::MovingActor(StudentWorld* world, string state, int imageID, int startX, int startY, bool isImpactable, int walkDir)
: Actor(world, imageID, startX, startY, right, 0, isImpactable)
{
    m_walkDir = walkDir;
    ticks_to_move = 0;
    m_state = state;
    m_justTeleported = false;
    m_affectedByEvent = true;
}

bool MovingActor::checkCurrDirValid() { return checkValidDir(m_walkDir); }

bool MovingActor::checkValidNewDir(int dir)
{
    if((dir == up && m_walkDir == down) || (dir == down && m_walkDir == up) ||
       (dir == left && m_walkDir == right) || (dir == right && m_walkDir == left))
        return false;
    return checkValidDir(dir);
}

bool MovingActor::checkValidDir(int dir)
{
    int boardX = 0, boardY = 0;
    switch(dir)
    {
        case right:
        {
            boardX = (getX() + SPRITE_WIDTH) / SPRITE_WIDTH;
            boardY = getY() / SPRITE_WIDTH;
            break;
        }
        case left:
        {
            if(getX() == 0)
                return false;
            boardX = (getX() - 2) / SPRITE_WIDTH;
            boardY = getY() / SPRITE_WIDTH;
            break;
        }
        case up:
        {
            boardX = getX() / SPRITE_WIDTH;
            boardY = (getY() + SPRITE_WIDTH) / SPRITE_WIDTH;
            break;
        }
        case down:
        {
            if(getY() == 0)
                return false;
            boardX = getX() / SPRITE_WIDTH;
            boardY = (getY() - 2) / SPRITE_WIDTH;
            break;
        }

    }

    Board::GridEntry ge = getWorld()->getBoard().getContentsOf(boardX, boardY);
    return !(ge == Board::empty);
}

void MovingActor::walk()
{
    moveAtAngle(m_walkDir, 2);
    ticks_to_move--;
}

void MovingActor::autoChooseNewDir()
{
    if(m_walkDir == left || m_walkDir == right)
    {
        if(checkValidDir(up))
            setWalkDir(up);
        else
            setWalkDir(down);
    }
    else
    {
        if(checkValidDir(right))
            setWalkDir(right);
        else
            setWalkDir(left);
    }
}

void MovingActor::chooseRandDir()
{
    while(!checkCurrDirValid())
    {
        int newDir = randInt(1, 4);
        switch(newDir)
        {
            case 1:
                setWalkDir(up);
                break;
            case 2:
                setWalkDir(down);
                break;
            case 3:
                setWalkDir(right);
                break;
            case 4:
                setWalkDir(left);
                break;
        }
    }
}

void MovingActor::setWalkDir(int dir)
{
    m_walkDir = dir;
    updateSpriteDir();
}

int MovingActor::getWalkDir() const { return m_walkDir; }

void MovingActor::setTicks(int ticks) { ticks_to_move = ticks; }

int MovingActor::getTicks() const { return ticks_to_move; }

void MovingActor::setState(std::string state) { m_state = state; }

std::string MovingActor::getState() const { return m_state; }

void MovingActor::updateSpriteDir()
{
    if(m_walkDir == left)
        setDirection(left);
    else
        setDirection(right);
}

void MovingActor::teleport()
{
    int currBoardX = getX() / SPRITE_WIDTH;
    int currBoardY = getY() / SPRITE_HEIGHT;
    int boardX = randInt(0, BOARD_WIDTH - 1);
    int boardY = randInt(0, BOARD_HEIGHT - 1);
    Board::GridEntry ge = getWorld()->getBoard().getContentsOf(boardX, boardY);
    while(ge == Board::empty || (boardX == currBoardX && boardY == currBoardY))
    {
        boardX = randInt(0, BOARD_WIDTH - 1);
        boardY = randInt(0, BOARD_HEIGHT - 1);
        ge = getWorld()->getBoard().getContentsOf(boardX, boardY);
    }
    int viewX = boardX * SPRITE_WIDTH;
    int viewY = boardY * SPRITE_HEIGHT;
    moveTo(viewX, viewY);
    m_justTeleported = true;
}

void MovingActor::setJustTeleported(bool justTeleported) { m_justTeleported = justTeleported; }

bool MovingActor::getJustTeleported() const { return m_justTeleported; }

void MovingActor::swapPosAndMov(MovingActor *p)
{
    int currX = getX();
    int currY = getY();
    int currTicks = ticks_to_move;
    string currState = m_state;
    int currWalkDir = m_walkDir;
    int currSpriteDir = getDirection();
    
    moveTo(p->getX(), p->getY());
    ticks_to_move = p->ticks_to_move;
    m_state = p->m_state;
    m_walkDir = p->m_walkDir;
    setDirection(p->getDirection());
    
    p->moveTo(currX, currY);
    p->ticks_to_move = currTicks;
    p->m_state = currState;
    p->m_walkDir = currWalkDir;
    p->setDirection(currSpriteDir);
    
    // prevents swapped actor from being affected by event square
    p->m_affectedByEvent = false;
}

void MovingActor::setAffectedByEvent(bool affected) { m_affectedByEvent = affected; }

bool MovingActor::getAffectedByEvent() const { return m_affectedByEvent; }

bool MovingActor::atFork()
{
    if(getX() % SPRITE_WIDTH != 0 || getY() % SPRITE_HEIGHT != 0)   // checks if directly on square
        return false;
    int count = 0;
    if(checkValidDir(up))
        count++;
    if(checkValidDir(right))
        count++;
    if(checkValidDir(left))
        count++;
    if(checkValidDir(down))
        count++;
    return (count >= 3);
}

// Player
Player::Player(StudentWorld* world, int player, int imageID, int startX, int startY)
: MovingActor(world, "waiting_to_roll", imageID, startX, startY, false)
{
    m_playerNum = player;
    m_coins = 0;
    m_stars = 0;
    m_hasVortex = false;
    m_allowFork = true;
}
void Player::doSomething()
{

    if(getState() == "waiting_to_roll")
    {
        if(getJustTeleported())
        {
            chooseRandDir();
            setJustTeleported(false);
        }
            
        int playerAction = getWorld()->getAction(m_playerNum);
        if(playerAction == ACTION_ROLL)
        {
            int dieRoll = randInt(1, 10);
            setTicks(dieRoll * 8);
            setState("walking");
        }
        
        else if(playerAction == ACTION_FIRE && m_hasVortex)
        {
            int vortexX, vortexY;
            int walkDir = getWalkDir();
            if(walkDir == right || walkDir == left)
                getPositionInThisDirection(walkDir, SPRITE_WIDTH, vortexX, vortexY);
            else
                getPositionInThisDirection(walkDir, SPRITE_HEIGHT, vortexX, vortexY);
            getWorld()->newVortex(vortexX, vortexY, getWalkDir());  // plays vortex sound
            m_hasVortex = false;
        }
        else
            return;
    }
        
    if(getState() == "walking")
    {
        if(atFork())
        {
            if(m_allowFork)
            {
                int playerAction = getWorld()->getAction(m_playerNum);
    
                if(playerAction == ACTION_UP && checkValidNewDir(up))
                    setWalkDir(up);
                else if (playerAction == ACTION_DOWN && checkValidNewDir(down))
                    setWalkDir(down);
                else if (playerAction == ACTION_RIGHT && checkValidNewDir(right))
                    setWalkDir(right);
                else if (playerAction == ACTION_LEFT && checkValidNewDir(left))
                    setWalkDir(left);
                else
                    return;
            }
            else
                m_allowFork = true;     // always allow fork unless otherwise set by directional square during tick
        }
        
        else if(!checkCurrDirValid())
            autoChooseNewDir();
        
        walk();     // also decrements ticks
        if(getTicks() == 0)
            setState("waiting_to_roll");
    }
}

int Player::getDice() const { return (ceil(getTicks() / 8.0)); }

int Player::getStars() const { return m_stars; }

int Player::getCoins() const { return m_coins; }

bool Player::hasVortex() const { return m_hasVortex; }

void Player::addCoins(int coins) { m_coins = max(0, m_coins + coins); }

void Player::addStars(int stars) { m_stars = max(0, m_stars + stars); }

void Player::giveVortex() { m_hasVortex = true; }

void Player::setAllowFork(bool fork) { m_allowFork = fork; }

void Player::resetCoins() { m_coins = 0; }

void Player::resetStars() { m_stars = 0; }

void Player::swapCoins(Player* p)
{
    int currCoins = m_coins;
    m_coins = p->m_coins;
    p->m_coins = currCoins;
}
void Player::swapStars(Player* p)
{
    int currStars = m_stars;
    m_stars = p->m_stars;
    p->m_stars = currStars;
}

// Enemy
Enemy::Enemy(StudentWorld* world, int maxSquares, int imageID, int startX, int startY)
: MovingActor(world, "paused", imageID, startX, startY, true)
{
    m_maxSquares = maxSquares;
    squares_to_move = 0;
    m_pauseCounter = 180;
}

void Enemy::doSomething()
{
    if(getState() == "paused")
    {
        if((checkNewActorOnSameSquare(getWorld()->getPeach())) && getWorld()->getPeach()->getState() == "waiting_to_roll")
            newPlayerLandAction(getWorld()->getPeach());
        if((checkNewActorOnSameSquare(getWorld()->getYoshi())) && getWorld()->getYoshi()->getState() == "waiting_to_roll")
            newPlayerLandAction(getWorld()->getYoshi());

        m_pauseCounter--;
        if(m_pauseCounter == 0)
        {
            squares_to_move = randInt(1, m_maxSquares);
            setTicks(squares_to_move * 8);
            chooseRandDir();
            setState("walking");
        }
    }
    
    if(getState() == "walking")
    {
        if(atFork())
            chooseRandDir();
        
        else if(!checkCurrDirValid())
            autoChooseNewDir();
        
        walk();
        if(getTicks() == 0)
        {
            setState("paused");
            m_pauseCounter = 180;
            onPause();
        }
    }
}

void Enemy::onImpact()
{
    teleport();
    setWalkDir(right);
    setState("paused");
    m_pauseCounter = 180;
}


// Bowser
Bowser::Bowser(StudentWorld* world, int imageID, int startX, int startY)
: Enemy(world, 10, imageID, startX, startY) {}

void Bowser::newPlayerLandAction(Player* p)
{
    int rand = randInt(1, 2);
    if(rand == 1)
    {
        p->resetCoins();
        p->resetStars();
        getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
    }
}

void Bowser::onPause()
{
    int rand = randInt(1, 4);
    if(rand == 1)
        getWorld()->replaceWithDropping(getX(), getY());    // also plays sound
}

// Boo
Boo::Boo(StudentWorld* world, int imageID, int startX, int startY)
: Enemy(world, 3, imageID, startX, startY) {}

void Boo::newPlayerLandAction(Player* p)
{
    Player* otherP = nullptr;
    if(p == getWorld()->getPeach())
        otherP = getWorld()->getYoshi();
    else
        otherP = getWorld()->getPeach();

    int rand = randInt(1, 2);
    switch(rand)
    {
        case 1:
            p->swapCoins(otherP);
            break;
        case 2:
            p->swapStars(otherP);
            break;
    }
    getWorld()->playSound(SOUND_BOO_ACTIVATE);
}

void Boo::onPause() {}

// Vortex
Vortex::Vortex(StudentWorld* world, int imageID, int startX, int startY, int walkDir)
: MovingActor(world, "walking", imageID, startX, startY, false, walkDir) {}

void Vortex::doSomething()
{
    if(!isActive())
        return;
    
    walk();
    if(getX() < 0 || getX() >= VIEW_WIDTH || getY() < 0 || getY() >= VIEW_HEIGHT)
        setInactive();
    
    Actor* actorImpacted = getWorld()->vortexImpact(getX(), getY());
    if(actorImpacted == nullptr)
        return;
    actorImpacted->onImpact();
    setInactive();
    getWorld()->playSound(SOUND_HIT_BY_VORTEX);
}

// Square
Square::Square(StudentWorld* world, int imageID, int startX, int startY, int dir)
: Actor(world, imageID, startX, startY, dir, 1, false, true) {}

void Square::doSomething()
{
    if(!isActive())
        return;
    
    if(checkNewActorOnSameSquare(getWorld()->getPeach()))
    {
        string state = getWorld()->getPeach()->getState();
        if(state == "waiting_to_roll")
            newPlayerLandAction(getWorld()->getPeach());
        else if (state == "walking")
            newPlayerMoveAction(getWorld()->getPeach());
    }
    
    if(checkNewActorOnSameSquare(getWorld()->getYoshi()))
    {
        string state = getWorld()->getYoshi()->getState();
        if(state == "waiting_to_roll")
            newPlayerLandAction(getWorld()->getYoshi());
        else if (state == "walking")
            newPlayerMoveAction(getWorld()->getYoshi());
    }
}

// CoinSquare
CoinSquare::CoinSquare(StudentWorld* world, int coins, int imageID, int startX, int startY)
: Square(world, imageID, startX, startY)
{
    m_coinsToAdd = coins;
}

void CoinSquare::newPlayerLandAction(Player* p) { addCoinsToPlayer(p); }

void CoinSquare::newPlayerMoveAction(Player* p) {}

void CoinSquare::addCoinsToPlayer(Player* p)
{
    p->addCoins(m_coinsToAdd);
    if(m_coinsToAdd > 0)
        getWorld()->playSound(SOUND_GIVE_COIN);
    else
        getWorld()->playSound(SOUND_TAKE_COIN);
}

// StarSquare
StarSquare::StarSquare(StudentWorld* world, int imageID, int startX, int startY)
: Square(world, imageID, startX, startY) {}

void StarSquare::newPlayerLandAction(Player* p) { giveStarIfRich(p); }

void StarSquare::newPlayerMoveAction(Player* p) { newPlayerLandAction(p); }

void StarSquare::giveStarIfRich(Player* p)
{
    if(p->getCoins() < 20)
        return;
    p->addCoins(-20);
    p->addStars(1);
    getWorld()->playSound(SOUND_GIVE_STAR);
}

// DirectionalSquare
DirectionalSquare::DirectionalSquare(StudentWorld* world, int imageID, int startX, int startY, int dir)
: Square(world, imageID, startX, startY, dir)
{
    m_forcingDir = dir;
}

void DirectionalSquare::doSomething()
{
    Square::doSomething();
    int x = getX();
    int y = getY();
    if(x == getWorld()->getPeach()->getX() && y == getWorld()->getPeach()->getY())
        getWorld()->getPeach()->setAllowFork(false);
    if(x == getWorld()->getYoshi()->getX() && y == getWorld()->getYoshi()->getY())
        getWorld()->getYoshi()->setAllowFork(false);
}

void DirectionalSquare::newPlayerLandAction(Player* p) { p->setWalkDir(m_forcingDir); }

void DirectionalSquare::newPlayerMoveAction(Player* p) { newPlayerLandAction(p); }

// BankSquare
BankSquare::BankSquare(StudentWorld* world, int imageID, int startX, int startY)
: Square(world, imageID, startX, startY) {}

void BankSquare::newPlayerLandAction(Player* p) { withdrawBank(p); }

void BankSquare::newPlayerMoveAction(Player* p) { depositBank(p); }

void BankSquare::withdrawBank(Player* p)
{
    p->addCoins(getWorld()->getBank());
    getWorld()->setBank(0);
    getWorld()->playSound(SOUND_WITHDRAW_BANK);
}

void BankSquare::depositBank(Player* p)
{
    int coinsToDeposit = min(5, p->getCoins());
    p->addCoins(-coinsToDeposit);
    getWorld()->setBank(getWorld()->getBank() + coinsToDeposit);
    getWorld()->playSound(SOUND_DEPOSIT_BANK);
}

// EventSquare
EventSquare::EventSquare(StudentWorld* world, int imageID, int startX, int startY)
: Square(world, imageID, startX, startY) {}

void EventSquare::newPlayerLandAction(Player* p)
{
    if(p->getAffectedByEvent())
        randEvent(p);
    else
        p->setAffectedByEvent(true);
}

void EventSquare::newPlayerMoveAction(Player* p) {}

void EventSquare::randEvent(Player* p)
{
    int event = randInt(1, 3);
    switch(event)
    {
        case 1:     // teleport
        {
            p->teleport();
            getWorld()->playSound(SOUND_PLAYER_TELEPORT);
            break;
        }
            
        case 2:     // swap
        {
            if(p == getWorld()->getPeach())
                p->swapPosAndMov(getWorld()->getYoshi());
            else
                p->swapPosAndMov(getWorld()->getPeach());

            getWorld()->playSound(SOUND_PLAYER_TELEPORT);
            break;
        }
            
        case 3:     // give vortex
        {
            p->giveVortex();
            getWorld()->playSound(SOUND_GIVE_VORTEX);
            break;
        }
    }
}

// DroppingSquare
DroppingSquare::DroppingSquare(StudentWorld* world, int imageID, int startX, int startY)
: Square(world, imageID, startX, startY) {}

void DroppingSquare::newPlayerLandAction(Player* p)
{
    randDeduct(p);
    getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
}

void DroppingSquare::newPlayerMoveAction(Player* p) {}

void DroppingSquare::randDeduct(Player* p)
{
    int rand = randInt(1, 2);
    switch(rand)
    {
        case 1:
        {
            p->addCoins(-10);
            break;
        }
        case 2:
        {
            p->addStars(-1);
            break;
        }
    }
}
