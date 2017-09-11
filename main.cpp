/*
 * Bermuda Syndrome engine rewrite
 * Copyright (C) 2007 Gregory Montoir
 */

#include "game.h"
#include "systemstub.h"

static const char *USAGE =
	"Bermuda Syndrome\n"
	"Usage: bs [OPTIONS]...\n"
	"  --datapath=PATH   Path to data files (default 'DATA')\n"
	"  --savepath=PATH   Path to save files (default '.')\n"
	"  --musicpath=PATH  Path to music files (default 'MUSIC')\n";

static bool parseOption(const char *arg, const char *longCmd, const char **opt) {
	bool handled = false;
	if (arg[0] == '-' && arg[1] == '-') {
		if (strncmp(arg + 2, longCmd, strlen(longCmd)) == 0) {
			*opt = arg + 2 + strlen(longCmd);
			handled = true;
		}
	}
	return handled;
}

#undef main
int main(int argc, char *argv[]) {
	const char *dataPath = "DATA";
	const char *savePath = ".";
	const char *musicPath = "MUSIC";
	for (int i = 1; i < argc; ++i) {
		bool opt = false;
		if (strlen(argv[i]) >= 2) {
			opt |= parseOption(argv[i], "datapath=", &dataPath);
			opt |= parseOption(argv[i], "savepath=", &savePath);
			opt |= parseOption(argv[i], "musicpath=", &musicPath);
		}
		if (!opt) {
			printf(USAGE);
			return 0;
		}
	}
	g_debugMask = DBG_INFO; // | DBG_RES | DBG_GAME | DBG_PARSER | DBG_OPCODES | DBG_DIALOGUE;
	SystemStub *stub = SystemStub_SDL_create();
	Game *g = new Game(stub, dataPath, savePath, musicPath);
	g->mainLoop();
	delete g;
	delete stub;
	return 0;
}

