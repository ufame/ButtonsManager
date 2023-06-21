#include "amxxmodule.h"
#include <vector>
#include <usercmd.h>

void CmdStart(const edict_t* player, const struct usercmd_s* cmd, unsigned int random_seed);

class Key {
public:
    Key() {
        isActive = true;
    }

    ~Key() {
        if (fwdId != -1)
            MF_UnregisterSPForward(fwdId);
    }

    bool isActive;
    int fwdId, KeysBitSum;
};

extern DLL_FUNCTIONS* g_pFunctionTable;
int CmdStartForward = 0;
std::vector<Key*> Keys;

static cell AMX_NATIVE_CALL RegisterKeyPressed(AMX* amx, cell* params) {
    int len;
    Key* p = new Key;
    p->KeysBitSum = params[1];
    p->fwdId = MF_RegisterSPForwardByName(amx, MF_GetAmxString(amx, params[2], 0, &len), FP_CELL, FP_CELL, FP_CELL, FP_DONE);
    Keys.push_back(p);
    if (!g_pFunctionTable->pfnCmdStart)
        g_pFunctionTable->pfnCmdStart = CmdStart;
    return p->fwdId;
}

static cell AMX_NATIVE_CALL UnRegisterKeyPressed(AMX* amx, cell* params) {
    int fwd = params[1];
    for (auto it = Keys.begin(); it != Keys.end(); ++it) {
        Key* p = *it;
        if (p->fwdId == fwd) {
            Keys.erase(it);
            delete p;
            if (Keys.empty())
                g_pFunctionTable->pfnCmdStart = NULL;
            return 1;
        }
    }
    return 0;
}

static cell AMX_NATIVE_CALL DisableKeyPressed(AMX* amx, cell* params) {
    int fwd = params[1];
    for (auto it = Keys.begin(); it != Keys.end(); ++it) {
        Key* p = *it;
        if (p->fwdId == fwd) {
            p->isActive = false;
            return 1;
        }
    }
    return 0;
}

static cell AMX_NATIVE_CALL EnableKeyPressed(AMX* amx, cell* params) {
    int fwd = params[1];
    for (auto it = Keys.begin(); it != Keys.end(); ++it) {
        Key* p = *it;
        if (p->fwdId == fwd) {
            p->isActive = true;
            return 1;
        }
    }
    return 0;
}

static cell AMX_NATIVE_CALL SetKeyPressed(AMX* amx, cell* params) {
    int fwd = params[1];
    for (auto it = Keys.begin(); it != Keys.end(); ++it) {
        Key* p = *it;
        if (p->fwdId == fwd) {
            p->KeysBitSum = params[2];
            return 1;
        }
    }
    return 0;
}

AMX_NATIVE_INFO _Natives[] = {
        {"RegisterKeyPressed", RegisterKeyPressed},
        {"UnRegisterKeyPressed", UnRegisterKeyPressed},
        {"DisableKeyPressed", DisableKeyPressed},
        {"EnableKeyPressed", EnableKeyPressed},
        {"SetKeyPressed", SetKeyPressed},
        {NULL, NULL}
};

void CmdStart(const edict_t* player, const struct usercmd_s* cmd, unsigned int random_seed) {
    auto l = Keys.size();
    if (!l)
        RETURN_META(MRES_IGNORED);
    int i = 0;
    auto index = ENTINDEX(player);
    auto iOldButtons = player->v.oldbuttons;
    auto iButtons = cmd->buttons;

    for (i = 0; i < l; i++) {
        Key* p = Keys[i];
        if (!p->isActive)
            continue;
        if (((iButtons & p->KeysBitSum) == p->KeysBitSum) && ((iOldButtons & p->KeysBitSum) != p->KeysBitSum))
            MF_ExecuteForward(p->fwdId, (cell)index, (cell)p->fwdId, 1);
        else if (((iButtons & p->KeysBitSum) != p->KeysBitSum) && ((iOldButtons & p->KeysBitSum) == p->KeysBitSum))
            MF_ExecuteForward(p->fwdId, (cell)index, (cell)p->fwdId, 0);
    }

    RETURN_META(MRES_IGNORED);
}

void ClearHooks() {
    for (auto it = Keys.begin(); it != Keys.end(); ++it)
        delete *it;
    Keys.clear();
}

void OnAmxxAttach() {
    MF_AddNatives(_Natives);
}

void ServerDeactivate() {
    g_pFunctionTable->pfnCmdStart = NULL;
    ClearHooks();
    RETURN_META(MRES_IGNORED);
}