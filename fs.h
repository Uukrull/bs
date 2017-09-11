/*
 * Bermuda Syndrome engine rewrite
 * Copyright (C) 2007 Gregory Montoir
 */

#ifndef FS_H__
#define FS_H__

#include "intern.h"

struct File;
struct FileSystem_impl;

struct FileSystem {
	FileSystem(const char *rootDir);
	~FileSystem();

	File *openFile(const char *path, bool errorIfNotFound = true);
	void closeFile(File *f);

	FileSystem_impl *_impl;
};

struct FileHolder {
	FileHolder(FileSystem &fs, const char *path)
		: _fs(fs), _fp(0) {
		_fp = _fs.openFile(path);
	}

	~FileHolder() {
		if (_fp) {
			_fs.closeFile(_fp);
		}
	}

	FileSystem &_fs;
	File *_fp;
};

#endif
