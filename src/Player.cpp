#include "Player.h"
#include "Tools.h"
#include "CollWorld.h"
using namespace Snow;

SDL_Texture* Player::m_images [2][5];
Player player[1];

void Player::Init()
{
    m_images [1] [0] = LoadPic("Player/Marisa/1.png");
    m_images [1] [1] = LoadPic("Player/Marisa/2.png");
    m_images [1] [2] = LoadPic("Player/Marisa/3.png");
    m_images [1] [3] = LoadPic("Player/Marisa/4.png");
    m_images [1] [4] = LoadPic("Player/Marisa/5.png");
    m_images [0] [0] = LoadPic("Player/Reimu/1.png");
    m_images [0] [1] = LoadPic("Player/Reimu/2.png");
    m_images [0] [2] = LoadPic("Player/Reimu/3.png");
    m_images [0] [3] = LoadPic("Player/Reimu/4.png");
    m_images [0] [4] = LoadPic("Player/Reimu/5.png");
}

Player::Player()
{
    //ctor
}

Player::~Player()
{
    //dtor
}

void Player::SetPlayerImage(int n)
{
    m_playerImageNum = n;
}

void Player::SetPlayer(int n){
    m_playerNum = n;
}


void Player::OnDraw()
{
    if(m_invin%2==0 && m_live != Player::DEAD)
        SDL_RenderCopy(pRnd,m_images[m_playerImageNum] [(m_cnt/5)%5],nullptr,&m_r);
    if(m_k[4]){
        SDL_Rect pdRec = {int(m_x)-3,int(m_y)-3,6,6};
        SDL_SetRenderDrawColor(pRnd,255,255,255,255);
        SDL_RenderFillRect(pRnd,&pdRec);
    }
}

void Player::OnNext()
{
    ++m_cnt;
    double spd = 7;

    if(m_k[4]) spd = 3;
    if((m_k[0] && m_k[2]) ||
       (m_k[0] && m_k[3]) ||
       (m_k[1] && m_k[2]) ||
       (m_k[1] && m_k[3])
    ) spd /= 1.4142135623730950488016887242097;

    if(m_live==LIVING){
        if(m_k[0]) m_y -= spd;
        if(m_k[1]) m_y += spd;
        if(m_k[2]) m_x -= spd;
        if(m_k[3]) m_x += spd;
        if(m_x<45) m_x = 45;
        else if(m_x >= WIDTH-45) m_x = WIDTH-45;

        if(m_y<45) m_y = 45;
        else if(m_y >= HEIGHT-45) m_y = HEIGHT-45;
    }

    if(m_live == Player::BIRTHING){
        m_x = 2*m_birthTimer++-50;
        m_y = HEIGHT/2;
        if(m_birthTimer==50) m_live = Player::LIVING;
    }

    if(m_invin==0) collWorld.SetPlayer(m_playerNum,true,m_x-2,m_y-2);

    m_r.x = m_x - m_r.w/2;
    m_r.y = m_y - m_r.h/2;

    if(m_invin!=0) --m_invin;
}


void Player::OnEvent(Key k, bool b)
{
    if(k == T_UP) m_k[0] = b;
    else if(k == T_DOWN) m_k[1] = b;
    else if(k == T_LEFT) m_k[2] = b;
    else if(k == T_RIGHT) m_k[3] = b;
    else if(k == T_SLOW) m_k[4] = b;
}

void Player::Invincible(int frame)
{
    m_invin = frame;
    collWorld.SetPlayer(m_playerNum,false);
}

void Player::Kill()
{
    m_live = Player::DEAD;
}

void Player::Birth()
{
    m_live = Player::BIRTHING;
    m_birthTimer=0;
    Invincible(300);
}