#pragma once

// pre-declare.
class Entity;

class IClientEntityListener {
public:
	virtual void OnEntityCreated(Entity* ent) = 0;
	virtual void OnEntityDeleted(Entity* ent) = 0;
};

class CEntInfo {
public:
	Entity*		m_pEntity;
	int			m_SerialNumber;
	CEntInfo*	m_pPrev;
	CEntInfo*	m_pNext;
};

class IClientEntityList {
public:
	enum indices : size_t {
		GETCLIENTENTITY = 3,
		GETCLIENTENTITYFROMHANDLE = 4,
		GETHIGHESTENTITYINDEX = 6,
	};

public:
	template< typename t = Entity* >
	__forceinline t GetClientEntity(int index) {
		return util::get_method< t(__thiscall*)(decltype(this), int) >(this, GETCLIENTENTITY)(this, index);
	}

	template< typename t = Entity* >
	__forceinline t GetClientEntityFromHandle(EHANDLE handle) {
		return util::get_method< t(__thiscall*)(decltype(this), EHANDLE) >(this, GETCLIENTENTITYFROMHANDLE)(this, handle);
	}

	template< typename t = Entity* >
	__forceinline t GetClientEntityFromHandle(ulong_t handle) {
		return util::get_method< t(__thiscall*)(decltype(this), ulong_t) >(this, GETCLIENTENTITYFROMHANDLE)(this, handle);
	}

	__forceinline int GetHighestEntityIndex() {
		return util::get_method< int(__thiscall*)(decltype(this)) >(this, GETHIGHESTENTITYINDEX)(this);
	}

public:
	virtual void* unk0(int nIndex) = 0;
	virtual void* unk1(void* hNetworkable) = 0;
	virtual void* unk2(void* hUnknown) = 0;
	virtual void* unk3(int nIndex) = 0;
	virtual void* unk4(void* hEntity) = 0;
	virtual void* unk5(bool bIncludeNonNetworkable) = 0;
	virtual void* unk6() = 0;
	virtual void* unk7(int iMax) = 0;
	virtual void* unk8() = 0;

public:
	__forceinline void AddListenerEntity(IClientEntityListener* pListener) {
		vecEntityListeners.AddToTail(pListener);
	}

	__forceinline void RemoveListenerEntity(IClientEntityListener* pListener) {
		vecEntityListeners.FindAndRemove(pListener);
	}

private:
	CUtlVector<IClientEntityListener*> vecEntityListeners;
};