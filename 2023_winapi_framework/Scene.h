#pragma once
class Object;
class Scene
{
public:
	Scene();
	virtual ~Scene();
	virtual void Init() abstract;
	virtual void Update();
	virtual void FinalUpdate();
	virtual void Render(HDC _dc);
	virtual void Release();
public:
	vector<Object*>& GetGroupObject(OBJECT_GROUP _etype)
	{
		return m_vecObj[(UINT)_etype];
	}
public:
	void AddObject(Object* _pObj, OBJECT_GROUP _eType)
	{
		m_vecObj[(UINT)_eType].push_back(_pObj);
	}
private:
	vector<Object*> m_vecObj[(UINT)OBJECT_GROUP::END];
};

