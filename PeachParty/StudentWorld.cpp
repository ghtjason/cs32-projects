#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Actor.h"
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_peach = nullptr;
    m_yoshi = nullptr;
    m_bank = 0;
}

int StudentWorld::init()
{
    string board_file = assetPath() + "board0" + to_string(getBoardNumber()) + ".txt";
    Board::LoadResult result = m_board.loadBoard(board_file);
    if (result == Board::load_fail_file_not_found || result == Board::load_fail_bad_format)
        return GWSTATUS_BOARD_ERROR;

    int viewX, viewY;
    for(int x = 0; x < BOARD_WIDTH; x++)
    {
        for(int y = 0; y < BOARD_HEIGHT; y++)
        {
            viewX = x * SPRITE_WIDTH;
            viewY = y * SPRITE_HEIGHT;
            Board::GridEntry ge = m_board.getContentsOf(x, y);
            switch(ge)
            {
                case Board::player:
                {
                    m_peach = new Player(this, 1, IID_PEACH, viewX, viewY);
                    m_yoshi = new Player(this, 2, IID_YOSHI, viewX, viewY);
                    CoinSquare* cs = new CoinSquare(this, 3, IID_BLUE_COIN_SQUARE, viewX, viewY);
                    actors.push_back(cs);
                    break;
                }
                case Board::blue_coin_square:
                {
                    CoinSquare* cs = new CoinSquare(this, 3, IID_BLUE_COIN_SQUARE, viewX, viewY);
                    actors.push_back(cs);
                    break;
                }
                case Board::red_coin_square:
                {
                    CoinSquare* cs = new CoinSquare(this, -3, IID_RED_COIN_SQUARE, viewX, viewY);
                    actors.push_back(cs);
                    break;
                }
                case Board::star_square:
                {
                    StarSquare* ss = new StarSquare(this, IID_STAR_SQUARE, viewX, viewY);
                    actors.push_back(ss);
                    break;
                }
                case Board::up_dir_square:
                {
                    DirectionalSquare* ds = new DirectionalSquare(this, IID_DIR_SQUARE, viewX, viewY, GraphObject::up);
                    actors.push_back(ds);
                    break;
                }
                case Board::right_dir_square:
                {
                    DirectionalSquare* ds = new DirectionalSquare(this, IID_DIR_SQUARE, viewX, viewY, GraphObject::right);
                    actors.push_back(ds);
                    break;
                }
                case Board::left_dir_square:
                {
                    DirectionalSquare* ds = new DirectionalSquare(this, IID_DIR_SQUARE, viewX, viewY, GraphObject::left);
                    actors.push_back(ds);
                    break;
                }
                case Board::down_dir_square:
                {
                    DirectionalSquare* ds = new DirectionalSquare(this, IID_DIR_SQUARE, viewX, viewY, GraphObject::down);
                    actors.push_back(ds);
                    break;
                }
                case Board::bank_square:
                {
                    BankSquare* bs = new BankSquare(this, IID_BANK_SQUARE, viewX, viewY);
                    actors.push_back(bs);
                    break;
                }
                case Board::event_square:
                {
                    EventSquare* es = new EventSquare(this, IID_EVENT_SQUARE, viewX, viewY);
                    actors.push_back(es);
                    break;
                }
                case Board::bowser:
                {
                    Bowser* bow = new Bowser(this, IID_BOWSER, viewX, viewY);
                    actors.push_back(bow);
                    CoinSquare* cs = new CoinSquare(this, 3, IID_BLUE_COIN_SQUARE, viewX, viewY);
                    actors.push_back(cs);
                    break;
                }
                case Board::boo:
                {
                    Boo* boo = new Boo(this, IID_BOO, viewX, viewY);
                    actors.push_back(boo);
                    CoinSquare* cs = new CoinSquare(this, 3, IID_BLUE_COIN_SQUARE, viewX, viewY);
                    actors.push_back(cs);
                    break;
                }
                case Board::empty:
                    break;
            }

        }
    }
    
    startCountdownTimer(99);
    return GWSTATUS_CONTINUE_GAME;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}


int StudentWorld::move()
{
    updateGameStatText();   // spec puts this after all the actors doSomething(), but before matches sample behavior

    list<Actor*>::iterator it = actors.begin();
    m_peach->doSomething();
    m_yoshi->doSomething();
    
    while(it != actors.end())
    {
        if((*it)->isActive())
        {
            (*it)->doSomething();
        }
        it++;
    }
    
    it = actors.begin();
    while(it != actors.end())
    {
        if(!(*it)->isActive())
        {
            delete (*it);
            it = actors.erase(it);
        }
        else
            it++;
    }
        
    if(timeRemaining() <= 0)
    {
        playSound(SOUND_GAME_FINISHED);
        Player* winner = chooseWinner();
        setFinalScore(winner->getStars(), winner->getCoins());
        if(winner == m_peach)
            return GWSTATUS_PEACH_WON;
        else
            return GWSTATUS_YOSHI_WON;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_peach;
    delete m_yoshi;
    m_peach = nullptr;  // prevents EXC_BAD_ACCESS error due to cleanUp() being called twice
    m_yoshi = nullptr;
    list<Actor*>::iterator it = actors.begin();

    while(it != actors.end())
    {
        delete (*it);
        it = actors.erase(it);
    }
}

Board& StudentWorld::getBoard() { return m_board; }

Player* StudentWorld::getPeach() const { return m_peach; }

Player* StudentWorld::getYoshi() const { return m_yoshi; }

int StudentWorld::getBank() const { return m_bank; }

void StudentWorld::setBank(int coins) { m_bank = coins; }

void StudentWorld::replaceWithDropping(int x, int y)
{
    list<Actor*>::iterator it = actors.begin();
    while(it != actors.end())
    {
        if(x == (*it)->getX() && y == (*it)->getY() && (*it)->isReplaceable())
        {
            (*it)->setInactive();
            DroppingSquare* ds = new DroppingSquare(this, IID_DROPPING_SQUARE, x, y);
            actors.push_back(ds);
            break;
        }
        it++;
    }
    playSound(SOUND_DROPPING_SQUARE_CREATED);
}

void StudentWorld::newVortex(int x, int y, int dir)
{
    Vortex* v = new Vortex(this, IID_VORTEX, x, y, dir);
    actors.push_back(v);
    playSound(SOUND_PLAYER_FIRE);
}


Actor* StudentWorld::vortexImpact(int x, int y)
{
    list<Actor*>::iterator it = actors.begin();
    while(it != actors.end())
    {
        if((*it)->isImpactable() && overlap(x, y, (*it)->getX(), (*it)->getY()))
        {
            return (*it);
        }
        it++;
    }
    return nullptr;
}
    
bool StudentWorld::overlap(int r1x1, int r1y1, int r2x1, int r2y1) const
{
    int r1x2 = r1x1 + SPRITE_WIDTH - 1;
    int r1y2 = r1y1 + SPRITE_HEIGHT - 1;
    int r2x2 = r2x1 + SPRITE_WIDTH - 1;
    int r2y2 = r2y1 + SPRITE_HEIGHT - 1;
    if(r1x1 > r2x2 || r2x1 > r1x2)  // one sprite's left edge is to the right of another's right
        return false;
    if(r1y1 > r2y2 || r2y1 > r1y2)  // one sprite's bottom edge is above the other's top
        return false;
    return true;
}

void StudentWorld::updateGameStatText()
{
    ostringstream oss;
    oss << playerStats(m_peach);
    oss << " | Time: " << timeRemaining() << " | Bank: " << m_bank << " | ";
    oss << playerStats(m_yoshi);

    setGameStatText(oss.str());
}

string StudentWorld::playerStats(Player* p)
{
    ostringstream oss;
    if(p == m_peach)
        oss << "P1 ";
    else if (p == m_yoshi)
        oss << "P2 ";
    oss << "Roll: " << p->getDice() << " Stars: " << p->getStars() << " $$: " << p->getCoins();
    if(p->hasVortex())
        oss << " VOR";
    return oss.str();
}
 
Player* StudentWorld::chooseWinner()
{
    int peachStars = m_peach->getStars();
    int yoshiStars = m_yoshi->getStars();
    
    if(yoshiStars > peachStars)
        return m_yoshi;
    
    if (yoshiStars == peachStars)
    {
        int peachCoins = m_peach->getCoins();
        int yoshiCoins = m_yoshi->getCoins();
        if(yoshiCoins > peachCoins)
            return m_yoshi;
        if(yoshiCoins == peachCoins)
        {
            int randWinner = randInt(1, 2);
            if(randWinner == 1)
                return m_yoshi;
        }
    }
    return m_peach;
}
