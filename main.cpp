#include "amxxmodule.h"
#include <vector>
#include <usercmd.h>
#include "Main.h"
int CmdStartForward = 0;
std::vector<Key *> Keys;

static cell AMX_NATIVE_CALL RegisterKeyPressed(AMX *amx, cell *params){
	int len;
	Key *p = new Key;
	p->KeysBitSum = params[1];
	p->fwdId = MF_RegisterSPForwardByName(amx, MF_GetAmxString(amx, params[2], 0, &len), FP_CELL, FP_CELL, FP_CELL, FP_DONE);
	Keys.insert(Keys.end(),p);
	if (!g_pFunctionTable->pfnCmdStart)g_pFunctionTable->pfnCmdStart=CmdStart;
	return p->fwdId;
}
static cell AMX_NATIVE_CALL UnRegisterKeyPressed(AMX *amx, cell *params){
	int fwd = params[1];
	for (auto i = 0; i < Keys.size(); ++i){
		Key *p = Keys.at(i);
		if (p->fwdId == fwd){
			Keys.erase(Keys.begin()+i);delete p;
			if (!Keys.size())g_pFunctionTable->pfnCmdStart = NULL;
			return 1;
		}
	}
	return 0;
}
static cell AMX_NATIVE_CALL DisableKeyPressed(AMX *amx, cell *params){
	int fwd = params[1];
	for (auto i = 0; i < Keys.size(); ++i){
		Key *p = Keys.at(i);
		if (p->fwdId == fwd){
			p->isActive = false;
			return 1;
		}
	}
	return 0;
}
static cell AMX_NATIVE_CALL EnableKeyPressed(AMX *amx, cell *params){
	int fwd = params[1];
	for (auto i = 0; i < Keys.size(); ++i){
		Key *p = Keys.at(i);
		if (p->fwdId == fwd){
			p->isActive = true;
			return 1;
		}
	}
	return 0;
}
static cell AMX_NATIVE_CALL SetKeyPressed(AMX *amx, cell *params){
	int fwd = params[1];
	for (auto i = 0; i < Keys.size(); ++i){
		Key *p = Keys.at(i);
		if (p->fwdId == fwd){
			p->KeysBitSum = params[2];
			return 1;
		}
	}
	return 0;
}
AMX_NATIVE_INFO _Natives[] = {
	{"RegisterKeyPressed", RegisterKeyPressed},
	{"UnRegisterKeyPressed", UnRegisterKeyPressed},
	{"DisableKeyPressed", EnableKeyPressed},
	{"EnableKeyPressed", EnableKeyPressed},
	{"SetKeyPressed", SetKeyPressed},

	{NULL,					NULL}
	 ///////////////////
};


void CmdStart(const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed){
	auto l = Keys.size();
	if(!l)RETURN_META(MRES_IGNORED);
	int i = 0;
	auto index = ENTINDEX(player);
	auto iOldButtons = player->v.oldbuttons;
	auto iButtons = cmd->buttons;
	
	for (i=0; i<l; i++){
		if(!Keys[i]->isActive)continue;
		if (((iButtons&Keys[i]->KeysBitSum)==Keys[i]->KeysBitSum)&&((iOldButtons&Keys[i]->KeysBitSum)!=Keys[i]->KeysBitSum))MF_ExecuteForward(Keys[i]->fwdId, (cell)index, (cell)Keys[i]->fwdId, 1);
		else if (((iButtons&Keys[i]->KeysBitSum)!=Keys[i]->KeysBitSum)&&((iOldButtons&Keys[i]->KeysBitSum)==Keys[i]->KeysBitSum))MF_ExecuteForward(Keys[i]->fwdId, (cell)index, (cell)Keys[i]->fwdId, 0);
	}
	
	RETURN_META(MRES_IGNORED);
}

void ClearHooks(){
	auto l = Keys.size();
	for (auto i=0; i<l; i++)delete Keys[i];
	Keys.clear();
}

void OnAmxxAttach(){ MF_AddNatives(_Natives); }

void ServerDeactivate(){
	g_pFunctionTable->pfnCmdStart=NULL;
	ClearHooks();
	RETURN_META(MRES_IGNORED);
}