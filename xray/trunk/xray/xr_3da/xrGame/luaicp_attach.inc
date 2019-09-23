	HMODULE hLib = GetModuleHandle("luaicp.dll");
	if (hLib)
	{
		Msg("Lua Interceptor found! Attaching :)");

		typedef void(WINAPI *LUA_CAPTURE)(lua_State *L);

		LUA_CAPTURE ExtCapture = (LUA_CAPTURE) GetProcAddress(hLib, "ExtCapture");
		if (NULL != ExtCapture)
			ExtCapture(m_virtual_machine);
		else
			Msg("ExtCapture proc not found in luaicp.dll");
	}
