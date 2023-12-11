#include "pch.h"
#include "ReIcon.h"
#include "SceneMgr.h"

ReIcon::ReIcon(Texture* _tex, Vec2 _pos, Vec2 _scale, Vec2 _vmenupos)
	:Image(_tex, _pos, _scale, _vmenupos)
{
}

void ReIcon::Function()
{
	SceneMgr::GetInst()->LoadScene(L"GameScene");
}