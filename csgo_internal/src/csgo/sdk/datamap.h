#pragma once

// pre-declare.
class datamap_t;

// prototype.
using inputfunc_t = void( __cdecl* )( void* data );

enum fieldtype_t {
	FIELD_VOID = 0,			// No type or value
	FIELD_FLOAT,			// Any floating point value
	FIELD_STRING,			// A string ID (return from ALLOC_STRING)
	FIELD_VECTOR,			// Any vector, QAngle, or AngularImpulse
	FIELD_QUATERNION,		// A quaternion
	FIELD_INTEGER,			// Any integer or enum
	FIELD_BOOLEAN,			// boolean, implemented as an int, I may use this as a hint for compression
	FIELD_SHORT,			// 2 byte integer
	FIELD_CHARACTER,		// a byte
	FIELD_COLOR32,			// 8-bit per channel r,g,b,a (32bit color)
	FIELD_EMBEDDED,			// an embedded object with a datadesc, recursively traverse and embedded class/structure based on an additional typedescription
	FIELD_CUSTOM,			// special type that contains function pointers to it's read/write/parse functions
	FIELD_CLASSPTR,			// CBaseEntity *
	FIELD_EHANDLE,			// Entity handle
	FIELD_EDICT,			// edict_t *
	FIELD_POSITION_VECTOR,	// A world coordinate (these are fixed up across level transitions automagically)
	FIELD_TIME,				// a floating point time (these are fixed up automatically too!)
	FIELD_TICK,				// an integer tick count( fixed up similarly to time)
	FIELD_MODELNAME,		// Engine string that is a model name (needs precache)
	FIELD_SOUNDNAME,		// Engine string that is a sound name (needs precache)
	FIELD_INPUT,			// a list of inputed data fields (all derived from CMultiInputVar)
	FIELD_FUNCTION,			// A class function pointer (Think, Use, etc)
	FIELD_VMATRIX,			// a vmatrix (output coords are NOT worldspace)
	FIELD_VMATRIX_WORLDSPACE,// A VMatrix that maps some local space to world space (translation is fixed up on level transitions)
	FIELD_MATRIX3X4_WORLDSPACE,	// matrix3x4_t that maps some local space to world space (translation is fixed up on level transitions)
	FIELD_INTERVAL,			// a start and range floating point interval ( e.g., 3.2->3.6 == 3.2 and 0.4 )
	FIELD_MODELINDEX,		// a model index
	FIELD_MATERIALINDEX,	// a material index (using the material precache string table)
	FIELD_VECTOR2D,			// 2 floats
	FIELD_TYPECOUNT,		// MUST BE LAST
};

enum {
	TD_OFFSET_NORMAL = 0,
	TD_OFFSET_PACKED = 1,
	TD_OFFSET_COUNT,
};

class typedescription_t {
public:
	int							m_Type;
	const char*					m_Name;
	int							m_offset;
	unsigned short				m_size;
	short						m_Flags;
	const char*					m_ext_name;
	void*						m_save_restore_ops;
	void*						m_input_func;
	datamap_t*					m_td;
	int							m_bytes;
	class typedescription_t*	m_override_field;
	int							m_override_count;
	float						m_tolerance;
	int							m_flat_offset[TD_OFFSET_COUNT];
	unsigned short				m_flat_group;
};

struct optimized_datamap_t;

class datamap_t {
public:
	typedescription_t*	m_desc;
	int					m_size;
	char const*			m_Name;
	datamap_t*			m_base;
	int			        m_packed_size;
	void*				m_optimized_data_map;
};