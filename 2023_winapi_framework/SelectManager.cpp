#include "pch.h"
#include "SelectManager.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Object.h"
#include "KeyMgr.h"
#include "Tile.h"
#include "Debug.h"
#include "Core.h"
#include "Dotween.h"
#include "TimeMgr.h"
#include "ResMgr.h"

void SelectManager::Init()
{
	m_selectTile = nullptr;
	m_fRadius = 70.f;
	m_fmovetime = 0.2f;
	m_ftime = m_fmovetime;
	m_canMove = true;
	m_fCorrectionX = 75.f;
	m_fCorrectionY = 75.f;
}

void SelectManager::Update()
{
	m_ftime += TimeMgr::GetInst()->GetDT();
	if (m_ftime >= m_fmovetime)
	{
		if (!m_canMove)
		{
			m_canMove = true;
			ResMgr::GetInst()->Play(L"Move");
			Merge();
		}
	}
	else
	{
		m_canMove = false;
	}
}

const void SelectManager::TileClick(const vector<Object*>& _tilegroup)
{
	if (KeyMgr::GetInst()->GetKey(KEY_TYPE::LBUTTON) == KEY_STATE::DOWN && m_canMove)
	{
		Vec2 tileCenterPos;
		Vec2 p1, p2, p3;
		float angle1, angle2;
		m_pMousePos = KeyMgr::GetInst()->GetMousePos();
		for (auto tile : _tilegroup)
		{
			Tile* a = (Tile*)tile;
			tileCenterPos = a->GetPos();
			tileCenterPos.x += m_fCorrectionX;
			tileCenterPos.y += m_fCorrectionY;
			for (int i = 0; i < 6; i++)
			{
				angle1 = (i * (2 * M_PI / 6.0f)) + M_PI / 2.f;
				angle2 = ((i + 1) * (2 * M_PI / 6.0f)) + M_PI / 2.f;

				p1.x = tileCenterPos.x + m_fRadius * cos(angle1);
				p1.y = tileCenterPos.y + m_fRadius * sin(angle1);

				p2.x = tileCenterPos.x + m_fRadius * cos(angle2);
				p2.y = tileCenterPos.y + m_fRadius * sin(angle2);

				p3 = tileCenterPos;
				/*POINT points[] = {{p1.x,p1.y}, {p2.x,p2.y}, {p3.x,p3.y}};
				Polygon(Core::GetInst()->GetMainDC(), points, 3);*/
				if (TriangleInPoint(p1, p2, p3, m_pMousePos))
				{
					ResMgr::GetInst()->Play(L"Tile");
					if (m_selectTile == a)
					{
						m_selectTile = nullptr;
					}
					else if (m_selectTile == nullptr)
					{
						m_selectTile = a;
					}
					else
					{
						if (a->GetState() == TILE_STATE::CANMOVE)
						{
							m_to = a;
							m_ftime = 0;
							SceneMgr::GetInst()->GetCurScene()->
								AddObject(new Dotween(m_selectTile, m_to->GetPos()
									, m_fmovetime, DOTWEEN_TYPE::MOVE), OBJECT_GROUP::DOTWEEN);
						}
						else
						{
							m_selectTile = a;
						}
					}
					return;
				}
			}
		}
	}
}

const bool& SelectManager::TriangleInPoint(Vec2& _p1,
	Vec2& _p2, Vec2& _p3, Vec2& _point)
{
	Vec2 v1 = _p2 - _p1;
	Vec2 v2 = _point - _p1;

	float cross1 = Cross(v1, v2);

	v1 = _p3 - _p2;
	v2 = _point - _p2;

	float cross2 = Cross(v1, v2);

	v1 = _p1 - _p3;
	v2 = _point - _p3;

	float cross3 = Cross(v1, v2);

	if ((cross1 > 0 && cross2 > 0 && cross3 > 0)
		|| (cross1 < 0 && cross2 < 0 && cross3 < 0))
	{
		return true;
	}

	return false;
}

const void SelectManager::Merge()
{
	Vec2 tempScale = m_to->GetScale();
	Tile* beforeTile = m_selectTile;
	Tile* newTile = m_to;
	auto& tilevec = SceneMgr::GetInst()->GetCurScene()->GetGroupObject(OBJECT_GROUP::TILE);
	for (int i = 0; i < tilevec.size();)
	{
		if (tilevec[i] == m_selectTile || tilevec[i] == m_to)
			tilevec.erase(tilevec.begin() + i);
		else
			++i;
	}
	switch (m_to->GetType())
	{
	case TILE_TYPE::WATER:
	case TILE_TYPE::FIRE:
	case TILE_TYPE::GRASS:
	{
		if (m_to->GetType() == m_selectTile->GetType() || m_selectTile->GetType() == TILE_TYPE::PLUS)
		{
			newTile->SetCnt(1);
			newTile->ResetVec();
			newTile->AddImage(newTile->GetCnt(), newTile->GetType());
			SetTilePos(newTile);
			SceneMgr::GetInst()->GetCurScene()->AddObject(newTile, OBJECT_GROUP::TILE);
			SceneMgr::GetInst()->GetCurScene()->AddObject(new Dotween(newTile, Vec2(17, 17), 0.05f,
				DOTWEEN_TYPE::SCALE), OBJECT_GROUP::DOTWEEN);
			SceneMgr::GetInst()->GetCurScene()->AddObject(new Dotween(newTile, tempScale, 0.05f, 0.05f,
				DOTWEEN_TYPE::SCALE), OBJECT_GROUP::DOTWEEN);
		}
		else if (m_selectTile->GetType() == TILE_TYPE::WIND)
		{
			m_to = nullptr;
			m_selectTile = nullptr;
		}
		else
		{
			XY xy = m_to->GetposIdx();
			int tempNum = abs(m_to->GetCnt() - m_selectTile->GetCnt());
			if (tempNum == 0)
			{
				m_to = nullptr;
				m_selectTile = nullptr;
				return;
			}

			switch (m_to->GetType())
			{
			case TILE_TYPE::FIRE:
				if (m_selectTile->GetType() == TILE_TYPE::WATER)
				{
					newTile = beforeTile;
				}
				break;
			case TILE_TYPE::WATER:
				if (m_selectTile->GetType() == TILE_TYPE::GRASS)
				{
					newTile = beforeTile;
				}
				break;
			case TILE_TYPE::GRASS:
				if (m_selectTile->GetType() == TILE_TYPE::FIRE)
				{
					newTile = beforeTile;
				}
				break;
			}
			newTile->SetCnt(-newTile->GetCnt());
			newTile->SetCnt(tempNum);
			newTile->ResetVec();
			newTile->AddImage(newTile->GetCnt(), newTile->GetType());
			newTile->SetposIdx(xy);
			SetTilePos(newTile);
			SceneMgr::GetInst()->GetCurScene()->AddObject(newTile, OBJECT_GROUP::TILE);
			SceneMgr::GetInst()->GetCurScene()->AddObject(new Dotween(newTile, Vec2(17, 17), 0.05f,
				DOTWEEN_TYPE::SCALE), OBJECT_GROUP::DOTWEEN);
			SceneMgr::GetInst()->GetCurScene()->AddObject(new Dotween(newTile, tempScale, 0.05f, 0.05f,
				DOTWEEN_TYPE::SCALE), OBJECT_GROUP::DOTWEEN);
		}
		m_to = nullptr;
		m_selectTile = nullptr;
	}
		break;
	case TILE_TYPE::FIRELOCK:
	case TILE_TYPE::WATERLOCK:
	case TILE_TYPE::GRASSLOCK:
	{
		m_to = nullptr;
		m_selectTile = nullptr;
	}
		break;
	case TILE_TYPE::TELEPORT:
	{
		for (int i = 0; i < tilevec.size(); ++i)
		{
			Tile* t = (Tile*)tilevec[i];
			if (t != m_to && t->GetType() == TILE_TYPE::TELEPORT)
			{
				newTile = beforeTile;
				newTile->SetposIdx(t->GetposIdx());
				SetTilePos(newTile);
				tilevec.erase(tilevec.begin() + i);
			}
		}
		SceneMgr::GetInst()->GetCurScene()->AddObject(newTile, OBJECT_GROUP::TILE);
		SceneMgr::GetInst()->GetCurScene()->AddObject(new Dotween(newTile, Vec2(17, 17), 0.05f,
			DOTWEEN_TYPE::SCALE), OBJECT_GROUP::DOTWEEN);
		SceneMgr::GetInst()->GetCurScene()->AddObject(new Dotween(newTile, tempScale, 0.05f, 0.05f,
			DOTWEEN_TYPE::SCALE), OBJECT_GROUP::DOTWEEN);
		m_to = nullptr;
		m_selectTile = nullptr;
	}
		break;
	case TILE_TYPE::MOVELU:
	{
		newTile->ResetVec();

		SetTilePos(newTile);

		beforeTile->AddImage(beforeTile->GetCnt(), beforeTile->GetType());
		beforeTile->SetPos(newTile->GetPos());
		m_selectTile = beforeTile;
		SceneMgr::GetInst()->GetCurScene()->AddObject(beforeTile, OBJECT_GROUP::TILE);
		for (auto i : SceneMgr::GetInst()->GetCurScene()->GetGroupObject(OBJECT_GROUP::TILE))
		{
			Tile* tempTile = (Tile*)i;
			if (tempTile->GetposIdx().xidx == (tempTile->GetposIdx().yidx % 2 == 0 ?
				newTile->GetposIdx().xidx - 1 : newTile->GetposIdx().xidx)
				&& tempTile->GetposIdx().yidx == newTile->GetposIdx().yidx - 1)
			{
				m_to = tempTile;
				Merge();
				return;
			}
		}
	}
		break;
	case TILE_TYPE::MOVEL:
	{
		newTile->ResetVec();

		SetTilePos(newTile);

		beforeTile->AddImage(beforeTile->GetCnt(), beforeTile->GetType());
		beforeTile->SetPos(newTile->GetPos());
		m_selectTile = beforeTile;
		SceneMgr::GetInst()->GetCurScene()->AddObject(beforeTile, OBJECT_GROUP::TILE);
		for (auto i : SceneMgr::GetInst()->GetCurScene()->GetGroupObject(OBJECT_GROUP::TILE))
		{
			Tile* tempTile = (Tile*)i;
			if (tempTile->GetposIdx().xidx == newTile->GetposIdx().xidx - 1
				&& tempTile->GetposIdx().yidx == newTile->GetposIdx().yidx)
			{
				m_to = tempTile;
				Merge();
				return;
			}
		}
	}
		break;
	case TILE_TYPE::MOVELD:
	{
		newTile->ResetVec();

		SetTilePos(newTile);

		beforeTile->AddImage(beforeTile->GetCnt(), beforeTile->GetType());
		beforeTile->SetPos(newTile->GetPos());
		m_selectTile = beforeTile;
		SceneMgr::GetInst()->GetCurScene()->AddObject(beforeTile, OBJECT_GROUP::TILE);
		for (auto i : SceneMgr::GetInst()->GetCurScene()->GetGroupObject(OBJECT_GROUP::TILE))
		{
			Tile* tempTile = (Tile*)i;
			if (tempTile->GetposIdx().xidx == (newTile->GetposIdx().yidx % 2 == 0 ?
				newTile->GetposIdx().xidx : newTile->GetposIdx().xidx - 1)
				&& tempTile->GetposIdx().yidx == newTile->GetposIdx().yidx + 1)
			{
				m_to = tempTile;
				Merge();
				return;
			}
		}
	}
		break;
	case TILE_TYPE::MOVERU:
	{
		newTile->ResetVec();

		SetTilePos(newTile);
		m_selectTile = beforeTile;
		SceneMgr::GetInst()->GetCurScene()->AddObject(beforeTile, OBJECT_GROUP::TILE);
		for (auto i : SceneMgr::GetInst()->GetCurScene()->GetGroupObject(OBJECT_GROUP::TILE))
		{
			Tile* tempTile = (Tile*)i;
			if (tempTile->GetposIdx().xidx == (tempTile->GetposIdx().yidx % 2 == 0 ?
				newTile->GetposIdx().xidx : newTile->GetposIdx().xidx + 1)
				&& tempTile->GetposIdx().yidx == newTile->GetposIdx().yidx - 1)
			{
				m_to = tempTile;
				Merge();
				return;
			}
		}
	}
		break;
	case TILE_TYPE::MOVER:
	{
		newTile->ResetVec();

		SetTilePos(newTile);
		beforeTile->SetPos(newTile->GetPos());
		m_selectTile = beforeTile;
		SceneMgr::GetInst()->GetCurScene()->AddObject(beforeTile, OBJECT_GROUP::TILE);
		for (auto i : SceneMgr::GetInst()->GetCurScene()->GetGroupObject(OBJECT_GROUP::TILE))
		{
			Tile* tempTile = (Tile*)i;
			if (tempTile->GetposIdx().xidx == newTile->GetposIdx().xidx + 1
				&& tempTile->GetposIdx().yidx == newTile->GetposIdx().yidx)
			{
				m_to = tempTile;
				Merge();
				return;
			}
		}
	}
		break;
	case TILE_TYPE::MOVERD:
	{
		newTile->ResetVec();
		beforeTile->AddImage(beforeTile->GetCnt(), beforeTile->GetType());

		SetTilePos(newTile);
		beforeTile->SetPos(newTile->GetPos());
		m_selectTile = beforeTile;
		SceneMgr::GetInst()->GetCurScene()->AddObject(beforeTile, OBJECT_GROUP::TILE);
		for (auto i : SceneMgr::GetInst()->GetCurScene()->GetGroupObject(OBJECT_GROUP::TILE))
		{
			Tile* tempTile = (Tile*)i;
			if (tempTile->GetposIdx().xidx == (newTile->GetposIdx().yidx % 2 == 0 ?
				newTile->GetposIdx().xidx + 1 : newTile->GetposIdx().xidx)
				&& tempTile->GetposIdx().yidx == newTile->GetposIdx().yidx + 1)
			{
				m_to = tempTile;
				Merge();
				return;
			}
		}
	}
		break;
	case TILE_TYPE::WIND:
	{
		m_to = nullptr;
		m_selectTile = nullptr;
	}
		break;
	case TILE_TYPE::MINUS:
	{
		XY xy = newTile->GetposIdx();
		newTile = beforeTile;
		newTile->SetCnt(-1);
		if (newTile->GetCnt() <= 0)
		{
			m_to = nullptr;
			m_selectTile = nullptr;
			return;
		}
		newTile->ResetVec();
		newTile->SetposIdx(xy);
		SetTilePos(newTile);
		newTile->AddImage(newTile->GetCnt(), newTile->GetType());
		SceneMgr::GetInst()->GetCurScene()->AddObject(newTile, OBJECT_GROUP::TILE);
		SceneMgr::GetInst()->GetCurScene()->AddObject(new Dotween(newTile, Vec2(17, 17), 0.05f,
			DOTWEEN_TYPE::SCALE), OBJECT_GROUP::DOTWEEN);
		SceneMgr::GetInst()->GetCurScene()->AddObject(new Dotween(newTile, tempScale, 0.05f, 0.05f,
			DOTWEEN_TYPE::SCALE), OBJECT_GROUP::DOTWEEN);

		m_to = nullptr;
		m_selectTile = nullptr;
	}
	break;
	case TILE_TYPE::PLUS:
	{
		XY xy = newTile->GetposIdx();
		newTile = beforeTile;
		newTile->SetCnt(1);
		newTile->ResetVec();
		newTile->SetposIdx(xy);
		SetTilePos(newTile);
		newTile->AddImage(newTile->GetCnt(), newTile->GetType());
		SceneMgr::GetInst()->GetCurScene()->AddObject(newTile, OBJECT_GROUP::TILE);
		SceneMgr::GetInst()->GetCurScene()->AddObject(new Dotween(newTile, Vec2(17, 17), 0.05f,
			DOTWEEN_TYPE::SCALE), OBJECT_GROUP::DOTWEEN);
		SceneMgr::GetInst()->GetCurScene()->AddObject(new Dotween(newTile, tempScale, 0.05f, 0.05f,
			DOTWEEN_TYPE::SCALE), OBJECT_GROUP::DOTWEEN);

		m_to = nullptr;
		m_selectTile = nullptr;
	}
		break;
	}
}

const void SelectManager::SetTilePos(Tile* _tile)
{
	Vec2 pos = Vec2(_tile->GetposIdx().xidx * 128, _tile->GetposIdx().yidx * 105);
	if (_tile->GetposIdx().yidx % 2 == 1)
	{
		pos.x -= 65;
	}
	pos.x += (float)Core::GetInst()->GetResolution().x / 4.f;
	pos.y += (float)Core::GetInst()->GetResolution().y / 12;
	_tile->SetPos(pos);
}
