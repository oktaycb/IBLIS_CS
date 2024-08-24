#pragma once

// padding macro, please use, counts pads in class automaticly
#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )
#define PAD( size ) uint8_t MACRO_CONCAT( _pad, __COUNTER__ )[ size ];
#define Assert( _exp ) ((void)0)

// pre-declares.
class Player;
class Weapon;
class Entity;
class WeaponInfo;
class matrix3x4_t;
class Vector;
class QAngle;

struct BoxTraceInfo_t;
struct CBaseTrace;

#include "utlvector.h"
#include "activity.h"
#include "game.h"
#include "../../util/Math/Math.h"
#include "../../util/math/other/easings.h"
#include "vector2.h"
#include "vector3.h"
#include "angle.h"
#include "matrix.h"
#include "memalloc.h"
#include "basehandle.h"
#include "color.h"
#include "clientclass.h"
#include "datamap.h"
#include "entlist.h"
#include "../../csgo/hooks/game/clientdll.h"
#include "../../csgo/hooks/game/clientmode.h"
#include "engineclient.h"
#include "cvar.h"
#include "globalvars.h"
#include "mdlcache.h"
#include "input.h"
#include "../../csgo/hooks/game/surface.h"
#include "glow.h"
#include "studio.h"
#include "prediction.h"
#include "trace.h"
#include "studiorender.h"
#include "gameevents.h"
#include "hud.h"
#include "localize.h"
#include "beams.h"
#include "networkstringtable.h"
#include "sound.h"