#pragma once
#include "Image.h"
#include "Texture.h"
class GameSceneUI;
class MenuIcon :
	public Image
{
public:
	MenuIcon(Texture* _tex, Vec2 _pos, Vec2 _scale, GameSceneUI* _pGameSceneUI,Vec2 _vmenupos);
public:
	virtual void Function() override;
	virtual void Render(HDC _dc) override;
private:
	GameSceneUI* m_pGameSceneUI;
	float m_fAngle;
	float m_fcurAngle;
	float m_fcurTime;
	float m_fdelay;
};

