#pragma once

class Chams {
public:
	enum class ModelType_t : size_t {
		Invalid = 0u,
		Player,
		Weapon,
		Arms,
		Sleeves,
		ViewWeapon,
		Shadow
	};

	void OnModelRender(IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix);
	void DrawModel(Player* pPlayer, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix, bool bForceNull, bool bXQZ);
	Chams::ModelType_t GetModelType(const ModelRenderInfo_t& info);
public:
	void SetColor(float flAlpha, Color col, IMaterial* pMaterial = nullptr);
	bool SetupMaterial(float flAlpha, Color cColor, bool bFlag, int iPlayerChamType);
	void Reset();
	bool GenerateLerpedMatrix(int index, matrix3x4a_t* out);
	void GenerateMaterials();
private:
	IMaterial* m_RegularMaterial = nullptr;
	IMaterial* m_FlatMaterial = nullptr;
	IMaterial* m_GlowMaterial = nullptr;
	IMaterial* m_PulseMaterial = nullptr;
	IMaterial* m_GlassMaterial = nullptr;
	IMaterial* m_GhostMaterial = nullptr;
	IMaterial* m_HaloMaterial = nullptr;
};

inline Chams g_Chams;