#if defined _buttons_manager
	#endinput
#endif

#define _buttons_manager

#if AMXX_VERSION_NUM >= 175
	#pragma reqlib buttonsmanager
	#if !defined AMXMODX_NOAUTOLOAD
		#pragma loadlib buttonsmanager
	#endif
#else
	#pragma library buttonsmanager
#endif

/*
* Hook Key pressed/released events.
* Look at IN_* constants for available lists.
*
* @note The function will be called in the following manner:
*
* public KeyHandler(const player, const forward, const bool:pressed)
*
*   player     - Client index
*   forward    - Forward value which was returned from RegisterKeyPressed
*   pressed    - True if button was pressed, otherwise false
*
* @param key		The key(-s) bitsum
* @param callback	The forward to call
*
* @return			Returns a hook handle. Use EnableKeyPressed/DisableKeyPressed to toggle the forward on or off
*/
native RegisterKeyPressed(const key, const handler[]);

/* 
* Unregisters and remove a hook handle.
* Use the return value from RegisterKeyPressed as the parameter here!
*
* @param fwdId		The hook to stop
*
* @return			Returns true if the function is successfully executed, otherwise false
*/
native bool:UnRegisterKeyPressed(const fwdId);

/*
* To set new key(-s) bitsum on existing forward.
* Use the return value from RegisterKeyPressed as the parameter here!
*
* @param hook		The hook to change key(-s) bitsum
*
* @return			Returns true if the function is successfully executed, otherwise false
*/

native bool:SetKeyPressed(const fwdId, const key);

/*
* Stops a hook from triggering.
* Use the return value from RegisterKeyPressed as the parameter here!
*
* @param fwdId		The hook to disable
*
* @return			Returns true if the function is successfully executed, otherwise false
*/
native bool:DisableKeyPressed(const fwdId);

/*
* Starts a hook back up.
* Use the return value from RegisterHookChain as the parameter here!
*
* @param fwdId		The hook to enable
*
* @return			Returns true if the function is successfully executed, otherwise false
*/
native bool:EnableKeyPressed(const fwdId);
