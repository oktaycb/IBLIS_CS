#pragma once

enum REMOVALS : int
{
    REMOVALS_VISUAL_PUNCH = 0,
    REMOVALS_VISUAL_KICK,
    REMOVALS_VISUAL_SCOPE,
    REMOVALS_VISUAL_SMOKE,
    REMOVALS_VISUAL_FLASH,
    REMOVALS_VISUAL_TEAMMATES,
    REMOVALS_VISUAL_FOG,
    REMOVALS_VISUAL_SLEEVES,
    REMOVALS_VISUAL_HAND_SHAKING,
    REMOVALS_VISUAL_LANDING_BOB,
    REMOVALS_VISUAL_SHADOWS,
    REMOVALS_VISUAL_POSTPROCESS,
};

struct OffScreenDamageData_t {
    float m_Curtime, m_color_step;
    Color m_color;

    __forceinline OffScreenDamageData_t() : m_Curtime{ 0.f }, m_color{ colors::white } {}
    __forceinline OffScreenDamageData_t(float time, float m_color_step, Color color) : m_Curtime{ time }, m_color{ color } {}
};

struct WorldHitmarkerData_t {
    float m_time, m_alpha;
    float m_pos_x, m_pos_y, m_pos_z;

    Vector2D m_world_pos;
    bool m_world_to_screen;
};

class Visuals {
private:
    struct sound_t {
        Vector m_Position;
        float m_time;
        int m_ent;
    };
public:
    std::array< bool, 64 >                  m_draw;
    std::array< float, 2048 >               m_opacities;
    std::array< OffScreenDamageData_t, 64 > m_offscreen_damage;
    std::vector< WorldHitmarkerData_t >     m_world_hitmarkers;
    CUtlVector<IEngineSound::SndInfo_t>     m_utlvecSoundBuffer;
    std::vector< sound_t > m_sounds;
    Vector2D                                  m_crosshair;
    bool                                    m_thirdperson;
    int                                     fakeangels[64];
    float					                m_hit_start, m_hit_end, m_hit_duration;

    // info about planted c4.
    bool                                    planted;
    bool                                    bombexploded;
    bool                                    bombedefused;
    std::string                             m_bombsite;
    bool                                    bombeindefuse;
    float                                   m_planted_c4_defuse_time;
    bool        m_c4_planted;
    Entity*     m_planted_c4;
    float       m_planted_c4_explode_time;
    Vector      m_planted_c4_explosion_origin;
    float       m_planted_c4_damage;
    float       m_planted_c4_radius;
    float       m_planted_c4_radius_scaled;
    float       miss;
    float       miss1;
    float       miss2;
    std::string m_last_bombsite;
};

inline Visuals g_Visuals;


class newvisuals {
public:
    void Init();
    void Draw();
    bool GetBox();
public:
    void DrawBox();
    void DrawFlashEffect();
    void DrawName();
    void DrawHealthBar();
    void DrawBottomBars();
    void DrawFlags();
    void DrawSkeleton();
    void DrawOutOfView();
    void DrawDamage();

    // mostly local related.
    void PenCrosshair();
    void Resolver();
    void DrawHitmarker();

    // functions for world esp.
    void DrawWorldItems();
    void DrawProjectiles();

    // others.
    void Others();
    void DebugScreen();
    void PreverseKillFeed();
    void Glow();
private:
    Player* m_Player = nullptr;
    Weapon* m_Weapon = nullptr;
    player_info_t m_PlayerInfo = {};
    bool m_Enemy = false;
    bool m_Teammate = false;
    bool m_Local = false;
    bool m_Dormant = false;
    int  m_Index = 0;
    int  m_Alpha = 0;
    int  m_GlowAlpha = 0;
    int  m_LowAlpha = 0;
    int  m_BoxLonger = 0;
    struct Rect { int x = 0, y = 0, w = 0, h = 0; };
    Rect m_Box;
    std::array< float, 2048 >  m_Opacities = {};
    std::array< int, 64 > m_MaxEntity = {};
private:
    enum PlayerFlags
    {
        PLAYERFLAG_MONEY,
        PLAYERFLAG_ARMOR,
        PLAYERFLAG_SCOPED,
        PLAYERFLAG_FLASHED,
        PLAYERFLAG_RELOADING,
        PLAYERFLAG_BOMBCARRIER,
        PLAYERFLAG_PING
    };
    enum DroppedItems
    {
        DROPPED_WEAPON_ICON,
        DROPPED_WEAPON_TEXT,
        DROPPED_WEAPON_AMMO,
        DROPPED_WEAPON_DISTANCE
    };
    enum OutOfViewFlags
    {
        OOF_NAME,
        OOF_BOX,
        OOF_HP,
        OOF_AMMO,
        OOF_DANGER
    };
    template< typename t >
    inline t ConfigureTeams(t enemies, t allies, t local, Player* player = nullptr, const bool& bConfigureOther = false)
    {
        if (!bConfigureOther)
            return m_Enemy ? enemies : m_Local ? local : allies;
        else
            return player->enemy(g_Client.m_LocalPlayer) ? enemies : player->IsLocalPlayer() ? local : allies;
    }
public:
    inline int GetAlpha() { return m_Alpha; }
};

inline newvisuals new_visuals;