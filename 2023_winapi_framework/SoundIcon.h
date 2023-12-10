#pragma once
#include "Image.h"
class Texture;
class SoundIcon :
    public Image
{
public:
	SoundIcon(Texture* _tex, Vec2 _pos, Vec2 _scale);
public:
	virtual void Function() override;
	virtual void Render(HDC _dc) override;
private:
	Texture* m_pTex;
	Texture* m_pOffTex;
};

