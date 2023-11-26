#include "pch.h"
#include "TileImage.h"
#include "ResMgr.h"
#include "Texture.h"

TileImage::TileImage(const TILE_TYPE& _eType)
	:m_eType(_eType)
{
	switch (m_eType)
	{
	case TILE_TYPE::WATER:
		m_pTex = ResMgr::GetInst()->TexLoad(L"Water", L"Texture\\water.bmp");
		break;
	case TILE_TYPE::FIRE:
		m_pTex = ResMgr::GetInst()->TexLoad(L"Fire", L"Texture\\fire.bmp");
		break;
	case TILE_TYPE::GRASS:
		m_pTex = ResMgr::GetInst()->TexLoad(L"Grass", L"Texture\\grass.bmp");
		break;
	case TILE_TYPE::GRAY:
		break;
	case TILE_TYPE::TELEPORT:
		break;
	case TILE_TYPE::ForcedMovement:
		break;
	case TILE_TYPE::WIND:
		break;
	case TILE_TYPE::END:
		break;
	default:
		break;
	}
}

TileImage::~TileImage()
{
}

void TileImage::Update()
{
}

void TileImage::Render(HDC _dc)
{
	Vec2 vPos = GetPos();
	Vec2 vScale = GetScale();
	int Width = m_pTex->GetWidth();
	int Height = m_pTex->GetHeight();

	TransparentBlt(_dc, (int)(vPos.x - vScale.x / 2)
		, (int)(vPos.y - vScale.y / 2),
		Width * (vScale.x / 100), Height * (vScale.y / 100), m_pTex->GetDC(),
		0, 0, Width, Height, RGB(255, 0, 255));
}