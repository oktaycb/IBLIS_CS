#pragma once

class Sequence {
public:
	float m_Curtime;
	int   m_RelState;
	int   m_Sequence;

public:
	__forceinline Sequence() : m_Curtime{}, m_RelState{}, m_Sequence{} {};
	__forceinline Sequence(float time, int state, int seq) : m_Curtime{ time }, m_RelState{ state }, m_Sequence{ seq } {};
};

class CFakePing {
public:
	void UpdateIncomingSequences();
public:
	std::deque< Sequence > m_Sequences = {};
};

inline CFakePing g_FakePing;