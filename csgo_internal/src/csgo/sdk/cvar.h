#pragma once
#define FORCEINLINE_CVAR inline

enum {
	FCVAR_NONE                  = 0,
	FCVAR_UNREGISTERED          = ( 1 << 0 ),
	FCVAR_DEVELOPMENTONLY       = ( 1 << 1 ),
	FCVAR_GAMEDLL               = ( 1 << 2 ),
	FCVAR_CLIENTDLL             = ( 1 << 3 ),
	FCVAR_HIDDEN                = ( 1 << 4 ),
	FCVAR_PROTECTED             = ( 1 << 5 ),
	FCVAR_SPONLY                = ( 1 << 6 ),
	FCVAR_ARCHIVE               = ( 1 << 7 ),
	FCVAR_NOTIFY                = ( 1 << 8 ),
	FCVAR_USERINFO              = ( 1 << 9 ),
	FCVAR_CHEAT                 = ( 1 << 14 ),
	FCVAR_PRINTABLEONLY         = ( 1 << 10 ),
	FCVAR_UNLOGGED              = ( 1 << 11 ),
	FCVAR_NEVER_AS_STRING       = ( 1 << 12 ),
	FCVAR_REPLICATED            = ( 1 << 13 ),
	FCVAR_DEMO                  = ( 1 << 16 ),
	FCVAR_DONTRECORD            = ( 1 << 17 ),
	FCVAR_NOT_CONNECTED         = ( 1 << 22 ),
	FCVAR_ARCHIVE_XBOX          = ( 1 << 24 ),
	FCVAR_SERVER_CAN_EXECUTE    = ( 1 << 28 ),
	FCVAR_SERVER_CANNOT_QUERY   = ( 1 << 29 ),
	FCVAR_CLIENTCMD_CAN_EXECUTE = ( 1 << 30 )
};

class CCommand;
class ConCommandBase;
class ConCommand;
class ConVar;

typedef void(*FnChangeCallback_t)(ConVar* var, const char* pOldValue, float flOldValue);

typedef void(*FnCommandCallbackV1_t)(void);
typedef void(*FnCommandCallback_t)(const CCommand& command);

#define COMMAND_COMPLETION_MAXITEMS       64
#define COMMAND_COMPLETION_ITEM_LENGTH    64

typedef int(*FnCommandCompletionCallback)(const char* partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH]);

class IConCommandBaseAccessor
{
public:
	// Flags is a combination of FCVAR flags in cvar.h.
	// hOut is filled in with a handle to the variable.
	virtual bool RegisterConCommandBase(ConCommandBase * pVar) = 0;
};

typedef int CVarDLLIdentifier_t;

class ConCommandBase
{
	friend class CCvar;
	friend class ConVar;
	friend class ConCommand;
	friend class CDefaultCvar;
	
	friend void ConVar_Register(int nCVarFlag, IConCommandBaseAccessor* pAccessor);
public:
	ConCommandBase(void);
	ConCommandBase(const char* pName, const char* pHelpString = 0, int flags = 0);

	virtual                     ~ConCommandBase(void);
	virtual bool                IsCommand(void) const;
	virtual bool                IsFlagSet(int flag) const;
	virtual void                AddFlags(int flags);
	virtual void                RemoveFlags(int flags);
	virtual int                 GetFlags() const;
	virtual const char*			GetName(void) const;
	virtual const char*			GetHelpText(void) const;
	const ConCommandBase*		GetNext(void) const;
	ConCommandBase*				GetNext(void);
	virtual bool                IsRegistered(void) const;
	virtual CVarDLLIdentifier_t GetDLLIdentifier() const;
	virtual void                Create(const char* pName, const char* pHelpString = 0, int flags = 0);
	virtual void                Init();
	void                        Shutdown();
	char*						CopyString(const char* from);

	ConVar*						m_pNext;
	bool                        m_bRegistered;
	const char*					m_pszName;
	const char*					m_pszHelpString;
	int                         m_nFlags;

protected:
	// ConVars add themselves to this list for the executable. 
	// Then ConVar_Register runs through  all the console variables 
	// and registers them into a global list stored in vstdlib.dll
	static ConCommandBase* s_pConCommandBases;

	// ConVars in this executable use this 'global' to access values.
	static IConCommandBaseAccessor* s_pAccessor;

public:
	// This list will hold all the registered commands.
	// It is not from the official SDK. I've added this so that
	// we can parse all convars we have created if we want to
	// save them to a file later on, for example.
	static ConCommandBase* s_pRegisteredCommands;
};

class ICommandCallback
{
public:
	virtual void CommandCallback(const CCommand& command) = 0;
};

class ICommandCompletionCallback
{
public:
	virtual int CommandCompletionCallback(const char* pPartial, CUtlVector<void*>& commands) = 0;
};

//-----------------------------------------------------------------------------
// Purpose: The console invoked command
//-----------------------------------------------------------------------------
class ConCommand : public ConCommandBase
{
	friend class CCvar;

public:
	typedef ConCommandBase BaseClass;

	ConCommand(const char* pName, FnCommandCallbackV1_t callback,
		const char* pHelpString = 0, int flags = 0, FnCommandCompletionCallback completionFunc = 0);
	ConCommand(const char* pName, FnCommandCallback_t callback,
		const char* pHelpString = 0, int flags = 0, FnCommandCompletionCallback completionFunc = 0);
	ConCommand(const char* pName, ICommandCallback* pCallback,
		const char* pHelpString = 0, int flags = 0, ICommandCompletionCallback* pCommandCompletionCallback = 0);

	virtual         ~ConCommand(void);
	virtual bool    IsCommand(void) const;
	virtual int     AutoCompleteSuggest(const char* partial, CUtlVector<void*>& commands);
	virtual bool    CanAutoComplete(void);
	virtual void    Dispatch(const CCommand& command);

	//private:
	// NOTE: To maintain backward compat, we have to be very careful:
	// All public virtual methods must appear in the same order always
	// since engine code will be calling into this code, which *does not match*
	// in the mod code; it's using slightly different, but compatible versions
	// of this class. Also: Be very careful about adding new fields to this class.
	// Those fields will not exist in the version of this class that is instanced
	// in mod code.

	// Call this function when executing the command
	union
	{
		FnCommandCallbackV1_t       m_fnCommandCallbackV1;
		FnCommandCallback_t         m_fnCommandCallback;
		ICommandCallback* m_pCommandCallback;
	};

	union
	{
		FnCommandCompletionCallback m_fnCompletionCallback;
		ICommandCompletionCallback* m_pCommandCompletionCallback;
	};

	bool m_bHasCompletionCallback : 1;
	bool m_bUsingNewCommandCallback : 1;
	bool m_bUsingCommandCallbackInterface : 1;
};

class IConVar
{
public:
	virtual void		SetValue(const char* pValue) = 0;
	virtual void		SetValue(float flValue) = 0;
	virtual void		SetValue(int nValue) = 0;
	virtual void		SetValue(Color value) = 0;
	virtual const char* GetName(void) const = 0;
	virtual const char* GetBaseName(void) const = 0;
	virtual bool		IsFlagSet(int nFlag) const = 0;
	virtual int			GetSplitScreenPlayerSlot() const = 0;
};

//-----------------------------------------------------------------------------
// Purpose: A console variable
//-----------------------------------------------------------------------------
class ConVar : public ConCommandBase, public IConVar
{
	friend class CCvar;
	friend class ConVarRef;
	friend class SplitScreenConVarRef;

public:
	enum indices : size_t {
		GETNAME = 5,
		GETFLOAT = 12,
		GETINT = 13,
		SETVALUE_SZ = 14,
		SETVALUE_FLOAT = 15,
		SETVALUE_INT = 16,
	};

	typedef ConCommandBase BaseClass;

	ConVar(const char* pName, const char* pDefaultValue, int flags = 0);

	ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString);
	ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, bool bMin, float fMin, bool bMax, float fMax);
	ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, FnChangeCallback_t callback);
	ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, bool bMin, float fMin, bool bMax, float fMax, FnChangeCallback_t callback);

	virtual                     ~ConVar(void);
	virtual bool                IsFlagSet(int flag) const;
	virtual const char*			GetHelpText(void) const;
	virtual bool                IsRegistered(void) const;
	virtual const char*			GetName(void) const;
	virtual const char*			GetBaseName(void) const;
	virtual int                 GetSplitScreenPlayerSlot() const;

	virtual void                AddFlags(int flags);
	virtual int                 GetFlags() const;
	virtual bool                IsCommand(void) const;

	// Install a change callback (there shouldn't already be one....)
	void				InstallChangeCallback(FnChangeCallback_t callback, bool bInvoke = true);
	void				RemoveChangeCallback(FnChangeCallback_t callbackToRemove);

	int					GetChangeCallbackCount() const { return m_pParent->m_fnChangeCallbacks.Count(); }
	FnChangeCallback_t	GetChangeCallback(int slot) const { return m_pParent->m_fnChangeCallbacks[slot]; }

	// Retrieve value
	virtual float                   GetFloat(void) const;
	virtual int                     GetInt(void) const;
	FORCEINLINE_CVAR Color          GetColor(void) const;
	FORCEINLINE_CVAR bool           GetBool() const { return !!GetInt(); }
	FORCEINLINE_CVAR char const*	GetString(void) const;

	// Compiler driven selection for template use
	template <typename T> T Get(void) const;
	template <typename T> T Get(T*) const;

	// Any function that allocates/frees memory needs to be virtual or else you'll have crashes
	//  from alloc/free across dll/exe boundaries.

	// These just call into the IConCommandBaseAccessor to check flags and Set the var (which ends up calling InternalSetValue).
	virtual void                    SetValue(const char* value);
	virtual void                    SetValue(float value);
	virtual void                    SetValue(int value);
	virtual void                    SetValue(Color value);

	// Reset to default value
	void                            Revert(void);
	bool                            HasMin() const;
	bool                            HasMax() const;
	bool                            GetMin(float& minVal) const;
	bool                            GetMax(float& maxVal) const;
	float                           GetMinValue() const;
	float                           GetMaxValue() const;
	const char*						GetDefault(void) const;

	struct CVValue_t
	{
		char* m_pszString;
		int     m_StringLength;
		float   m_fValue;
		int     m_nValue;
	};

	FORCEINLINE_CVAR CVValue_t& GetRawValue()
	{
		return m_Value;
	}

	FORCEINLINE_CVAR const CVValue_t& GetRawValue() const
	{
		return m_Value;
	}

	//private:
	bool							InternalSetColorFromString(const char* value);
	virtual void					InternalSetValue(const char* value);
	virtual void					InternalSetFloatValue(float fNewValue);
	virtual void					InternalSetIntValue(int nValue);
	virtual void					InternalSetColorValue(Color value);
	virtual bool					ClampValue(float& value);
	virtual void					ChangeStringValue(const char* tempVal, float flOldValue);
	virtual void					Create(const char* pName, const char* pDefaultValue, int flags = 0, const char* pHelpString = 0, bool bMin = false, float fMin = 0.0, bool bMax = false, float fMax = false, FnChangeCallback_t callback = 0);

	// Used internally by OneTimeInit to Initialize.
	virtual void					Init();

	//protected:
	ConVar*							m_pParent;
	const char*						m_pszDefaultValue;
	CVValue_t						m_Value;
	bool							m_bHasMin;
	float							m_fMinVal;
	bool							m_bHasMax;
	float							m_fMaxVal;

	// Call this function when ConVar changes
	CUtlVector<FnChangeCallback_t>	m_fnChangeCallbacks;
};

class ICvar {
public:
	enum indices {
		FINDVAR            = 14,
		CONSOLECOLORPRINTF = 25,
	};

	// note - dex; this used to be a virtual in old games but i can't find it anymore ( at least when i looked last time ).
	__forceinline ConVar* GetCommands( ) {
		return *( ConVar** )( ( uintptr_t )this + 0x30 );
	}

	// FindVar but iterates manually.
	__forceinline ConVar* FindVar(const hash_t hash) {
		ConVar *list;

		list = GetCommands( );
		if( !list )
			return nullptr;

		// iterate all convars.
		for( auto it = list; it != nullptr; it = it->m_pNext ) {
			// compare hash.
			if (RT_HASH(it->m_pszName) == hash)
				return it;
		}

		return nullptr;
	}

	template< typename... arguments >
	void ConsoleColorPrintf( const Color& color, const char* format, arguments ... args ) {
		return util::get_method< void( __cdecl* )( decltype( this ), const Color&, const char*, ... )>( this, CONSOLECOLORPRINTF )( this, color, format, args... );
	}
};