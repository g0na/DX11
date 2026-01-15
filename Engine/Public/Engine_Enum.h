#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class WINMODE { FULL, WIN };
	enum class PROTOTYPE { GAMEOBJECT, COMPONENT };
	enum class RENDERGROUP { PRIORITY, NONBLEND, NONLIGHT, BLEND, UI, END};
	enum class STATE { RIGHT, UP, LOOK, POSITION, END };
	enum class D3DTS { VIEW, PROJ, END };
	enum class MOUSEKEYSTATE { LB, RB, WHEEL, XB, END };
	enum class MOUSEMOVESTATE { X, Y, WHEEL, END };
	enum class LIGHT { DIRECTIONAL, POINT, END };
	enum class MODEL { NONANIM, ANIM }; 
	enum class CELLPOINT { A, B, C, END };
	enum class LINE { AB, BC, CA, END };
	enum class LAYER { PLAYER, WEAPON, WEAPON_BOSS, MONSTER, OBJECT, MAP, CAMERA, END };
	enum class COLLIDER { SPHERE, AABB, OBB, END };
	enum class DEFERRED { DEBUG, DIRECTIONAL, POINT, COMBINED, END };
	enum class CHANNELID { SOUND_EFFECT, SOUND_WEAPON, SOUND_BOSS, SOUND_BOSS_WEAPON, SOUND_MONSTER, SOUND_BGM, MAXCHANNEL};
}
#endif // Engine_Enum_h__
