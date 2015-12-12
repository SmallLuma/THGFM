#include "CollWorld.h"
#include <mem.h>
#include "Snow.h"
#include "Player.h"
#include "StageMgr.h"
#include "PlayerBullet.h"

using namespace Snow;

CollWorld collWorld;

inline static bool CInRect(const SDL_Rect& r,int x,int y){
    return x>=r.x && x<=r.x+r.w && y>=r.y && y<=r.y+r.h;
}

void CollWorld::SetEnemyBuffurSize(int n)
{
    delete [] m_enemys;
    m_enemys = new CollCircle [n];
    m_enemySearchBotton = 0;
    m_enemySearchTop = 0;
    m_enemySize = n;
    for(int i = 0;i < n;++i)
        m_enemys[i].enable = false;
}

void CollWorld::ClearEnemy()
{
    m_enemySearchBotton = m_enemySearchTop = 0;
    for(int i = 0;i < m_enemySize;++i)
        m_enemys[i].enable = false;
}


void CollWorld::ClearEnemyBullet()
{
    m_enemyBulletSearchTop = 0;
    memset(m_enemyBullets,0,1500*sizeof(CollCircle));
}

void CollWorld::Debug_DrawEnemy()
{
    for(int i = m_enemySearchBotton;i < m_enemySearchTop;++i){
        if(m_enemys[i].enable && CInRect(m_enemy_playerBullet_opmRect,m_enemys[i].x,m_enemys[i].y)){
            SDL_SetRenderDrawColor(pRnd,0,255,255,92);
            SDL_Rect r = {int(m_enemys[i].x - m_enemys[i].r),
                                int(m_enemys[i].y - m_enemys[i].r),
                                int(2*m_enemys[i].r),
                                int(2*m_enemys[i].r)
            };
            SDL_RenderFillRect(pRnd,&r);
            char buf [4];
            sprintf(buf,"%d",i);
            SDL_SetRenderDrawColor(pRnd,255,255,255,255);
            SDLTest_DrawString(pRnd,r.x,r.y,buf);
        }
    }
}

void CollWorld::Debug_DrawEnemyBullet()
{
    for(int i = 0;i < m_enemyBulletSearchTop;++i){
        if(m_enemyBullets[i].enable){
            SDL_SetRenderDrawColor(pRnd,0,255,0,255);
            SDL_Rect r = {int(m_enemyBullets[i].x - m_enemyBullets[i].r),
                                int(m_enemyBullets[i].y - m_enemyBullets[i].r),
                                int(2*m_enemyBullets[i].r),
                                int(2*m_enemyBullets[i].r)
            };
            SDL_RenderFillRect(pRnd,&r);
            char buf [4];
            sprintf(buf,"%d",i);
            SDL_SetRenderDrawColor(pRnd,255,255,255,255);
            SDLTest_DrawString(pRnd,r.x,r.y,buf);
        }
    }
}

void CollWorld::Debug_DrawPlayer()
{
    for(int i = 0;i<2;++i){
        if(m_player[i].enable){
            SDL_SetRenderDrawColor(pRnd,0,255,0,92);
            SDL_Rect r = {int(m_player[i].x),int(m_player[i].y),int(m_player[i].r),int(m_player[i].r)};
            SDL_RenderFillRect(pRnd,&r);
        }
    }
}



void CollWorld::Update_Player_Enemy()
{
    for(int player = 0;player < 2;++player){
        if(!m_player[player].enable) continue;
        for(int i = m_enemySearchBotton;i < m_enemySearchTop;++i)
            if(m_enemys[i].enable)
                if(coll_c2c(m_player[player],m_enemys[i]))
                    OnPlayerEnemy(player,i);
    }
}

#include "Snow/Debug.h"
#include <ctime>
void CollWorld::Update_Player_EnemyBullet()
{
    for(int player = 0;player < 2;++player){
        if(!m_player[player].enable) continue;
        for(int i = 0;i < m_enemyBulletSearchTop;++i)
            if(m_enemyBullets[i].enable)
                if(coll_c2c(m_player[player],m_enemyBullets[i])){
                    OnPlayerEnemyBullet(player,i);
                }
    }
}

void CollWorld::Update_Enemy_PlayerBullet()
{
    static int bulletNeedTest[1500];
    int bulletNTSize = -1;

    for(int enemyNum = m_enemySearchBotton;enemyNum < m_enemySearchTop;++enemyNum){
        if(InRect(m_enemy_playerBullet_opmRect,m_enemys[enemyNum].x,m_enemys[enemyNum].y) && m_enemys[enemyNum].enable){
            if(bulletNTSize == -1){
                bulletNTSize = 0;
                for(int playerBulletNum = 0;playerBulletNum < m_playerBulletSearchTop;++playerBulletNum){
                    if(m_playerBullets[playerBulletNum].enable &&
                        InRect(m_enemy_playerBullet_opmRect,m_playerBullets[playerBulletNum].x,m_playerBullets[playerBulletNum].y)
                        ){
                            bulletNeedTest[bulletNTSize++] = playerBulletNum;
                            if(coll_r2c(m_playerBullets[playerBulletNum],m_enemys[enemyNum])){
                                stage.KillEnemy(enemyNum,playerBulletMgr.GetPower(playerBulletNum));
                                playerBulletMgr.Kill(playerBulletNum);
                                //PNT("CollWorld::Enemy_PlayerBullet:"<<enemyNum<<"  "<<playerBulletNum);
                            }
                        }
                }
            }else{
                for(int playerBulletNum = 0;playerBulletNum < bulletNTSize;++playerBulletNum){
                    if(coll_r2c(m_playerBullets[bulletNeedTest[playerBulletNum]],m_enemys[enemyNum])){
                        stage.KillEnemy(enemyNum,playerBulletMgr.GetPower(bulletNeedTest[playerBulletNum]));
                        playerBulletMgr.Kill(bulletNeedTest[playerBulletNum]);
                    }
                }
            }
        }
    }

    //PNT("RECT"<<m_enemy_playerBullet_opmRect.x<<" "<<m_enemy_playerBullet_opmRect.y<<" "<<m_enemy_playerBullet_opmRect.w<<" "<<m_enemy_playerBullet_opmRect.h);
}

void CollWorld::Debug_DrawPlayerBullet()
{
    SDL_SetRenderDrawColor(pRnd,255,0,0,128);
    for(int i = 0;i < 512;++i){
        CollRect& c = m_playerBullets[i];
        if(c.enable && CInRect(m_enemy_playerBullet_opmRect,c.x,c.y)){
            SDL_Rect r = {
                int(c.x),
                int(c.y),
                int(c.w),
                int(c.h)
            };
            SDL_RenderFillRect(pRnd,&r);
        }
    }
    SDL_SetRenderDrawColor(pRnd,255,0,0,96);
    SDL_RenderFillRect(pRnd,&m_enemy_playerBullet_opmRect);
}



#include "EffectMgr.h"
void CollWorld::OnPlayerEnemy(int player, int enemy)
{
    effMgr.Install(0,::player[player].GetX(),::player[player].GetY());
    ::player[player].Birth();
    //SDL_assert(false);
}

void CollWorld::OnPlayerEnemyBullet(int player, int eb)
{
    effMgr.Install(0,::player[player].GetX(),::player[player].GetY());
    ::player[player].Birth();
    //SDL_assert(false);
}
