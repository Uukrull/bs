/*
 * Bermuda Syndrome engine rewrite
 * Copyright (C) 2007 Gregory Montoir
 */

#include "decoder.h"
#include "file.h"
#include "game.h"
#include "mixer.h"
#include "str.h"
#include "systemstub.h"

static const char *_gameWindowTitle = "Bermuda Syndrome";

Game::Game(SystemStub *stub, const char *dataPath, const char *savePath, const char *musicPath)
	: _fs(dataPath), _stub(stub), _savePath(savePath), _musicPath(musicPath) {
	_mixer = new Mixer(_stub);
}

Game::~Game() {
	delete _mixer;
}

void Game::restart() {
	_stateSlot = 1;
	_mixerSoundId = Mixer::kDefaultSoundId;
	_mixerMusicId = Mixer::kDefaultSoundId;

	_lifeBarCurrentFrame = 0;
	_bagObjectAreaBlinkCounter = 0;
	_bagWeaponAreaBlinkCounter = 0;

	_lastDialogueEndedId = 0;
	_dialogueEndedFlag = 0;

	memset(_defaultVarsTable, 0, sizeof(_defaultVarsTable));
	memset(_varsTable, 0, sizeof(_varsTable));

	_scriptDialogId = 0;
	_scriptDialogFileName = 0;
	_scriptDialogSprite1 = 0;
	_scriptDialogSprite2 = 0;

	_switchScene = true;
	_clearSceneData = false;
	_gameOver = false;

	_loadDataState = 0;
	_previousBagAction = _currentBagAction = 0;
	_previousBagObject = _currentBagObject = -1;
	_startEndingScene = false;
//	_skipUpdateScreen = false;
	_currentPlayingSoundPriority = 0;
	_lifeBarDisplayed2 = _lifeBarDisplayed = false;

	memset(_keysPressed, 0, sizeof(_keysPressed));
	_musicTrack = 0;
	_musicName[0] = 0;
	_sceneNumber = 0;
	_currentSceneWgp[0] = 0;
	_tempTextBuffer[0] = 0;
	_currentSceneScn[0] = 0;

	_bagPosX = 585;
	_bagPosY = 23;

	memset(_sortedSceneObjectsTable, 0, sizeof(_sortedSceneObjectsTable));
	memset(_sceneObjectsTable, 0, sizeof(_sceneObjectsTable));
	_sceneObjectsCount = 0;
	memset(_animationsTable, 0, sizeof(_animationsTable));
	_animationsCount = 0;
	memset(_soundBuffersTable, 0, sizeof(_soundBuffersTable));
	_soundBuffersCount = 0;
	memset(_boxesTable, 0, sizeof(_boxesTable));
	memset(_boxesCountTable, 0, sizeof(_boxesCountTable));
	memset(_sceneObjectFramesTable, 0, sizeof(_sceneObjectFramesTable));
	_sceneObjectFramesCount = 0;
	memset(_bagObjectsTable, 0, sizeof(_bagObjectsTable));
	_bagObjectsCount = 0;
	memset(_sceneObjectMotionsTable, 0, sizeof(_sceneObjectMotionsTable));
	_sceneObjectMotionsCount = 0;
	memset(_nextScenesTable, 0, sizeof(_nextScenesTable));
	_sceneConditionsCount = 0;
	memset(_sceneObjectStatusTable, 0, sizeof(_sceneObjectStatusTable));
	_sceneObjectStatusCount = 0;
}

void Game::mainLoop() {
	_stub->init(_gameWindowTitle, kGameScreenWidth, kGameScreenHeight);
	restart();
	allocateTables();
	loadCommonSprites();

	_mixer->open();

	strcpy(_tempTextBuffer, "_01.SCN");

	_lastFrameTimeStamp = _stub->getTimeStamp();

	while (!_stub->_quit) {
		if (_switchScene) {
			_switchScene = false;
			if (stringEndsWith(_tempTextBuffer, "SCN")) {
				win31_sndPlaySound(6);
				debug(DBG_GAME, "switch to scene '%s'", _tempTextBuffer);
				strcpy(_currentSceneScn, _tempTextBuffer);
				parseSCN(_tempTextBuffer);
			} else {
				debug(DBG_GAME, "load mov '%s'", _tempTextBuffer);
				loadMOV(_tempTextBuffer);
			}
			assert(_sceneObjectsCount != 0); // else get back to the menu
			if (_currentBagObject == -1) {
				_currentBagObject = _bagObjectsCount - 1;
				if (_currentBagObject > 0) {
					_currentBagObject = 0;
				}
			}
			_gameOver = false;
			if (_loadDataState != 0) {
				setupScreenPalette(_bitmapBuffer0 + kOffsetBitmapPalette);
			}
		}
//		_skipUpdateScreen = true;
		runObjectsScript();
		if (!_switchScene) {
			updateKeyPressedTable();
			_stub->updateScreen();
			uint32 end = _lastFrameTimeStamp + kCycleDelay;
			do {
				_stub->sleep(10);
				_stub->processEvents();
			} while (!_stub->_pi.fastMode && _stub->getTimeStamp() < end);
			_lastFrameTimeStamp = _stub->getTimeStamp();
		}
	}

	clearSceneData(-1);
	deallocateTables();
	unloadCommonSprites();
	_mixer->close();
	_stub->destroy();
}

void Game::updateKeyPressedTable() {
	_keysPressed[13] = _stub->_pi.enter ? 1 : 0;
	_keysPressed[16] = _stub->_pi.shift ? 1 : 0;
	_keysPressed[32] = _stub->_pi.space ? 1 : 0;
	_keysPressed[37] = (_stub->_pi.dirMask & PlayerInput::DIR_LEFT)  ? 1 : 0;
	_keysPressed[38] = (_stub->_pi.dirMask & PlayerInput::DIR_UP)    ? 1 : 0;
	_keysPressed[39] = (_stub->_pi.dirMask & PlayerInput::DIR_RIGHT) ? 1 : 0;
	_keysPressed[40] = (_stub->_pi.dirMask & PlayerInput::DIR_DOWN)  ? 1 : 0;
	if (_stub->_pi.tab) {
		_stub->_pi.tab = false;
		handleBagMenu();
	}
	if (_stub->_pi.ctrl) {
		_stub->_pi.ctrl = false;
		_lifeBarDisplayed = !_lifeBarDisplayed;
	}
	if (_stub->_pi.load) {
		_stub->_pi.load = false;
		loadState(_stateSlot);
	}
	if (_stub->_pi.save) {
		_stub->_pi.save = false;
		saveState(_stateSlot);
	}
	if (_stub->_pi.stateSlot != 0) {
		int slot = _stateSlot + _stub->_pi.stateSlot;
		if (slot >= 1 && slot < 100) {
			_stateSlot = slot;
			debug(DBG_INFO, "Current game state slot is %d", _stateSlot);
		}
		_stub->_pi.stateSlot = 0;
	}
}

void Game::setupScreenPalette(const uint8 *src) {
	_stub->setPalette(src, 256);
}

void Game::clearSceneData(int anim) {
	debug(DBG_GAME, "Game::clearSceneData(%d)", anim);
	if (anim == -1) {
		_sceneConditionsCount = 0;
		_soundBuffersCount = 0;
		_animationsCount = 0;
		_sceneObjectsCount = 0;
		_sceneObjectMotionsCount = 0;
		_sceneObjectFramesCount = 0;
		_loadDataState = 0;
	} else {
		SceneAnimation *sa = &_animationsTable[anim];
		_animationsCount = anim + 1;
		_sceneObjectMotionsCount = sa->firstMotionIndex + sa->motionsCount;
		SceneObjectMotion *som = &_sceneObjectMotionsTable[_sceneObjectMotionsCount - 1];
		_sceneObjectFramesCount = som->firstFrameIndex + som->count;
		_sceneObjectsCount = sa->firstObjectIndex + sa->objectsCount;
		_soundBuffersCount = sa->firstSoundBufferIndex + sa->soundBuffersCount;
		_sceneConditionsCount = 0;
		_loadDataState = 2;
	}
	win31_sndPlaySound(7);
//	for (int i = NUM_SOUND_BUFFERS - 1; i >= _soundBuffersCount; --i) {
//		SoundBuffer *sb = &_soundBuffersTable[i];
//		if (sb->buffer) {
//			free(sb->buffer);
//			sb->buffer = 0;
//		}
//	}
	for (int i = NUM_SCENE_OBJECT_FRAMES - 1; i >= _sceneObjectFramesCount; --i) {
		if (_sceneObjectFramesTable[i].data) {
			free(_sceneObjectFramesTable[i].data);
			_sceneObjectFramesTable[i].data = 0;
		}
	}
	for (int i = NUM_SCENE_ANIMATIONS - 1; i >= _animationsCount; --i) {
		if (_animationsTable[i].scriptData) {
			free(_animationsTable[i].scriptData);
			_animationsTable[i].scriptData = 0;
		}
	}
	for (int i = NUM_SCENE_OBJECTS - 1; i >= _sceneObjectsCount; --i) {
		SceneObject *so = &_sceneObjectsTable[i];
		so->state = 0;
		memset(so->varsTable, 0, sizeof(so->varsTable));
	}
	for (int i = 0; i < 10; ++i) {
		_boxesCountTable[i] = 0;
	}
}

void Game::reinitializeObjects(int index) {
	debug(DBG_GAME, "Game::reinitializeObjects(%d)", index);
	SceneObject *so = &_sceneObjectsTable[index];
	if (so->state != 1 && so->state != 2) {
		int16 state = 0;
		switch (so->mode) {
		case 1:
			state = 1;
			break;
		case 2: {
				int16 rnd = _rnd.getNumber();
				int t = (rnd * so->modeRndMul) / 0x8000;
				if ((t & 0xFFFF) == 0) {
					state = 1;
				}
			}
			break;
		case 3:
			state = 2;
			break;
		}
		if (state != 0) {
			so->x = so->xInit;
			so->y = so->yInit;
			so->zPrev = so->z = so->zInit;
			so->flipPrev = so->flip = so->flipInit;
			so->motionNum1 = so->motionNum2 = so->motionNum + so->motionInit;
			so->frameNum = _sceneObjectMotionsTable[so->motionNum2].firstFrameIndex + so->motionFrameNum;
			if (so->flip == 2) {
				so->x -= _sceneObjectFramesTable[so->frameNum].hdr.w - 1;
			}
			if (so->flip == 1) {
				so->y -= _sceneObjectFramesTable[so->frameNum].hdr.h - 1;
			}
			if (so->state == 0) {
				so->xPrev = so->x;
				so->yPrev = so->y;
				so->frameNumPrev = so->frameNum;
			}
			so->state = so->statePrev = state;
		}
	}
}

void Game::updateObjects() {
	debug(DBG_GAME, "Game::updateObjects()");
	redrawObjects(false); //redrawObjects(_skipUpdateScreen);
	for (int i = 0; i < _sceneObjectsCount; ++i) {
		SceneObject *so = &_sceneObjectsTable[i];
		if (so->state == -1) {
			so->state = 0;
		}
	}
	for (int i = 0; i < _sceneObjectsCount; ++i) {
		SceneObject *so = &_sceneObjectsTable[i];
		so->statePrev = so->state;
		if (so->state == 1) {
			so->motionNum1 = so->motionNum2;
			so->flipPrev = so->flip;
			so->zPrev = so->z;
			so->xPrev = so->x;
			so->yPrev = so->y;
			so->frameNumPrev = so->frameNum;
			so->frameNum = _sceneObjectFramesTable[so->frameNumPrev].hdr.num + _sceneObjectMotionsTable[so->motionNum2].firstFrameIndex;
			const int dx = _sceneObjectFramesTable[so->frameNumPrev].hdr.xPos - _sceneObjectFramesTable[so->frameNum].hdr.xPos;
			const int dy = _sceneObjectFramesTable[so->frameNumPrev].hdr.yPos - _sceneObjectFramesTable[so->frameNum].hdr.yPos;
			if (so->flip == 2) {
				int ds = _sceneObjectFramesTable[so->frameNumPrev].hdr.w - _sceneObjectFramesTable[so->frameNum].hdr.w;
				so->x += dx + ds;
			} else {
				so->x -= dx;
			}
			so->y -= dy;
		}
	}
}

void Game::runObjectsScript() {
	debug(DBG_GAME, "Game::runObjectsScript()");
	_objectScript.nextScene = -1;
	assert(_loadDataState != 3); // unneeded code
	if (_varsTable[309]) {
		memset(_keysPressed, 0, 128);
	}
	if (_loadDataState == 2) {
		for (int i = 0; i < _sceneObjectsCount; ++i) {
			SceneObject *so = &_sceneObjectsTable[i];
			if (so->statePrev == 0 || so->statePrev == -1) {
				continue;
			}
			debug(DBG_GAME, "Game::runObjectsScript() currentObjectNum=%d", i);
			_objectScript.currentObjectNum = i;
			int anim = _sceneObjectMotionsTable[so->motionNum1].animNum;
			_objectScript.data = _animationsTable[anim].scriptData;
			int endOfDataOffset = _animationsTable[anim].scriptSize; // endOfDataOffset2
			_objectScript.dataOffset = 0;
			int statement = 0;
			while (_objectScript.dataOffset < endOfDataOffset) {
				int statementSize = _objectScript.fetchNextWord(); // endOfDataOffset1
				_objectScript.testObjectNum = -1;
				_objectScript.testDataOffset = _objectScript.dataOffset + statementSize;
				bool loop = true;
				while (loop) {
					int op = _objectScript.fetchNextWord();
					debug(DBG_OPCODES, "statement %d condition %d op %d", statement, i, op);
					if (op == 0) {
						break;
					}
					const GameConditionOpcode *cop = findConditionOpcode(op);
					if (!cop) {
						error("Invalid condition %d", op);
					}
					loop = (this->*(cop->pf))();
				}
				if (loop) {
					while (_objectScript.dataOffset < statementSize) {
						int op = _objectScript.fetchNextWord();
						debug(DBG_OPCODES, "statement %d operator %d op %d", statement, i, op);
						if (op == 100) {
							statementSize = _objectScript.dataOffset = endOfDataOffset;
							break;
						}
						const GameOperatorOpcode *oop = findOperatorOpcode(op);
						if (!oop) {
							error("Invalid operator %d", op);
						}
						(this->*(oop->pf))();
					}
				}
				_objectScript.dataOffset = statementSize;
				++statement;
			}
		}
		_dialogueEndedFlag = 0;
		if (_objectScript.nextScene != -1 && _objectScript.nextScene < _sceneConditionsCount) {
			strcpy(_tempTextBuffer, _nextScenesTable[_objectScript.nextScene].name);
			_switchScene = true;
		}
		for (int i = 0; i < _sceneObjectsCount; ++i) {
			reinitializeObjects(i);
		}
		if (_varsTable[0] >= 10 && !_gameOver) {
			strcpy(_musicName, "..\\midi\\gameover.mid");
			playMusic(_musicName);
			_gameOver = true;
//			_skipUpdateScreen = false;
		}
		if (_loadDataState == 2) {
			updateObjects();
		}
	}
//	_skipUpdateScreen = false;
	if (_varsTable[241] == 1) {
		_startEndingScene = true;
		stopMusic();
		clearSceneData(-1);
		_varsTable[241] = 2;
	}
}

int Game::findBagObjectByName(const char *objectName) const {
	debug(DBG_GAME, "Game::findBagObjectByName()", objectName);
	int index = -1;
	for (int i = 0; i < _bagObjectsCount; ++i) {
		if (strcasecmp(_bagObjectsTable[i].name, objectName) == 0) {
			index = i;
			break;
		}
	}
	return index;
}

int Game::getObjectTranslateXPos(int object, int dx1, int div, int dx2) {
	debug(DBG_GAME, "Game::getObjectTranslateXPos(%d, %d, %d, %d)", object, dx1, div, dx2);
	SceneObject *so = &_sceneObjectsTable[object];
	int16 _di = _sceneObjectMotionsTable[so->motionNum + so->motionInit].firstFrameIndex + so->motionFrameNum;
	int16 _ax, _dx;
	if (so->flip == 2) {
		_ax = _sceneObjectFramesTable[_di].hdr.xPos;
		_ax -= _sceneObjectFramesTable[so->frameNum].hdr.xPos;
		_ax += _sceneObjectFramesTable[_di].hdr.w;
		_ax -= _sceneObjectFramesTable[so->frameNum].hdr.w;
		_ax += dx1;
	} else {
		_ax = _sceneObjectFramesTable[so->frameNum].hdr.xPos;
		_ax -= _sceneObjectFramesTable[_di].hdr.xPos;
	}
	if (so->flipInit == 2) {
		_dx = 1 - _sceneObjectFramesTable[_di].hdr.w - dx1;
	} else {
		_dx = 0;
	}
	_ax = so->x - so->xInit - _dx - _ax - dx2;
	int16 _si = _ax % div;
	if (_si < 0) {
		_si += div;
	}
	return _si;
}

int Game::getObjectTranslateYPos(int object, int dy1, int div, int dy2) {
	debug(DBG_GAME, "Game::getObjectTranslateYPos(%d, %d, %d, %d)", object, dy1, div, dy2);
	SceneObject *so = &_sceneObjectsTable[object];
	int16 _di = _sceneObjectMotionsTable[so->motionNum + so->motionInit].firstFrameIndex + so->motionFrameNum;
	int16 _ax, _dx;
	if (so->flip == 1) {
		_ax = _sceneObjectFramesTable[_di].hdr.yPos;
		_ax -= _sceneObjectFramesTable[so->frameNum].hdr.yPos;
		_ax += _sceneObjectFramesTable[_di].hdr.h;
		_ax -= _sceneObjectFramesTable[so->frameNum].hdr.h;
		_ax += dy1;
	} else {
		_ax = _sceneObjectFramesTable[so->frameNum].hdr.yPos;
		_ax -= _sceneObjectFramesTable[_di].hdr.yPos;
	}
	if (so->flipInit == 1) {
		_dx = 1 - _sceneObjectFramesTable[_di].hdr.h - dy1;
	} else {
		_dx = 0;
	}
	_ax = so->y - so->yInit - _dx - _ax - dy2;
	int16 _si = _ax % div;
	if (_si < 0) {
		_si += div;
	}
	return _si;
}

int Game::findObjectByName(int currentObjectNum, int defaultObjectNum, bool *objectFlag) {
	int index = -1;
	*objectFlag = true;
	int16 len = _objectScript.fetchNextWord();
	debug(DBG_GAME, "Game::findObjectByName() len = %d", len);
	if (len == -1) {
		index = defaultObjectNum;
	} else if (len == 0) {
		*objectFlag = false;
		index = currentObjectNum;
	} else {
		debug(DBG_GAME, "Game::findObjectByName() name = '%s' len = %d", _objectScript.getString(), len);
		for (int i = 0; i < _sceneObjectsCount; ++i) {
			if (strcmp(_sceneObjectsTable[i].name, _objectScript.getString()) == 0) {
				index = i;
				break;
			}
		}
		_objectScript.dataOffset += len;
	}
	return index;
}

void Game::sortObjects() {
	for (int i = 0; i < _sceneObjectsCount; ++i) {
		_sortedSceneObjectsTable[i] = &_sceneObjectsTable[i];
	}
	for (int i = _sceneObjectsCount / 2; i > 0; i /= 2) {
		for (int j = i; j < _sceneObjectsCount; ++j) {
			for (int k = j - i; k >= 0; k -= i) {
				if (_sortedSceneObjectsTable[k]->z >= _sortedSceneObjectsTable[k + i]->z) {
					break;
				}
				SWAP(_sortedSceneObjectsTable[k], _sortedSceneObjectsTable[k + i]);
			}
		}
	}
}

void Game::copyBufferToBuffer(int x, int y, int w, int h, SceneBitmap *src, SceneBitmap *dst) {
	const uint8 *p_src = src->bits;

	const int x2 = MIN(src->w, dst->w);
	if (x > x2 || x + w <= 0) {
		return;
	}
	if (x < 0) {
		w += x;
		p_src -= x;
		x = 0;
	}
	if (x + w > x2) {
		w = x2 + 1 - x;
	}

	const int y2 = MIN(src->h, dst->h);
	if (y >= y2 || y + h <= 0) {
		return;
	}
	if (y < 0) {
		h += y;
		p_src -= y * src->pitch;
		y = 0;
	}
	if (y + h > y2) {
		h = y2 + 1 - y;
	}

	p_src += y * src->pitch + x;
	uint8 *p_dst = dst->bits + y * dst->pitch + x;
	while (h--) {
		memcpy(p_dst, p_src, w);
		p_dst += dst->pitch;
		p_src += src->pitch;
	}
}

void Game::drawBox(int x, int y, int w, int h, SceneBitmap *src, SceneBitmap *dst, int startColor, int endColor) {
	const uint8 *p_src = src->bits;

	const int x2 = MIN(src->w, dst->w);
	if (x > x2 || x + w <= 0) {
		return;
	}
	if (x < 0) {
		w += x;
		p_src -= x;
		x = 0;
	}
	if (x + w > x2) {
		w = x2 + 1 - x;
	}

	const int y2 = MIN(src->h, dst->h);
	if (y >= y2 || y + h <= 0) {
		return;
	}
	if (y < 0) {
		h += y;
		p_src -= y * src->pitch;
		y = 0;
	}
	if (y + h > y2) {
		h = y2 + 1 - y;
	}

	p_src += y * src->pitch + x;
	uint8 *p_dst = dst->bits + y * dst->pitch + x;
	while (h--) {
		for (int i = 0; i < w; ++i) {
			if (startColor > p_src[i] || endColor <= p_src[i]) {
				p_dst[i] = p_src[i];
			}
		}
		p_src += src->pitch;
		p_dst += dst->pitch;
	}
}

void Game::drawObject(int x, int y, const uint8 *src, SceneBitmap *dst) {
	int w = READ_LE_UINT16(src) + 1; src += 2;
	int h = READ_LE_UINT16(src) + 1; src += 2;

	int clippedW = w;
	if (x > dst->w || x + clippedW <= 0) {
		return;
	}
	if (x < 0) {
		clippedW += x;
		src -= x;
		x = 0;
	}
	if (x + clippedW > dst->w) {
		clippedW = dst->w + 1 - x;
	}

	int clippedH = h;
	if (y > dst->h || y + clippedH <= 0) {
		return;
	}
	if (y < 0) {
		clippedH += y;
		src -= y * w;
		y = 0;
	}
	if (y + clippedH > dst->h) {
		clippedH = dst->h + 1 - y;
	}

	for (int j = 0; j < clippedH; ++j) {
		for (int i = 0; i < clippedW; ++i) {
			if (src[i]) {
				dst->bits[dst->pitch * (y + j) + (x + i)] = src[i];
			}
		}
		src += w;
	}
}

void Game::drawObjectVerticalFlip(int x, int y, const uint8 *src, SceneBitmap *dst) {
	int w = READ_LE_UINT16(src) + 1; src += 2;
	int h = READ_LE_UINT16(src) + 1; src += 2;

	src += w - 1;

	int clippedW = w;
	if (x > dst->w || x + clippedW <= 0) {
		return;
	}
	if (x < 0) {
		clippedW += x;
		src += x;
		x = 0;
	}
	if (x + clippedW > dst->w) {
		clippedW = dst->w + 1 - x;
	}

	int clippedH = h;
	if (y > dst->h || y + clippedH <= 0) {
		return;
	}
	if (y < 0) {
		clippedH += y;
		src -= y * w;
		y = 0;
	}
	if (y + clippedH > dst->h) {
		clippedH = dst->h + 1 - y;
	}

	for (int j = 0; j < clippedH; ++j) {
		for (int i = 0; i < clippedW; ++i) {
			if (src[-i]) {
				dst->bits[dst->pitch * (y + j) + (x + i)] = src[-i];
			}
		}
		src += w;
	}
}

void Game::redrawObjectBoxes(int previousObject, int currentObject) {
	for (int b = 0; b < 10; ++b) {
		for (int i = 0; i < _boxesCountTable[b]; ++i) {
			Box *box = &_boxesTable[b][i];
			if (box->state == 2 && box->z <= _sortedSceneObjectsTable[previousObject]->z) {
				const int w = box->x2 - box->x1 + 1;
				const int h = box->y2 - box->y1 + 1;
				const int x = box->x1;
				const int y = _bitmapBuffer1.h + 1 - box->y2;
				if (previousObject == currentObject || box->z > _sortedSceneObjectsTable[currentObject]->z) {
					if (box->endColor != 0) {
						drawBox(x, y, w, h, &_bitmapBuffer3, &_bitmapBuffer1, box->startColor, box->startColor + box->endColor - 1);
					} else {
						copyBufferToBuffer(x, y, w, h, &_bitmapBuffer3, &_bitmapBuffer1);
					}
				}
			}
		}
	}
}

bool Game::isObjectInRect(int object) {
	SceneObject *so = _sortedSceneObjectsTable[object];
	if (so->xPrev + _sceneObjectFramesTable[so->frameNumPrev].hdr.w < so->x) {
		return false;
	}
	if (so->x + _sceneObjectFramesTable[so->frameNum].hdr.w < so->xPrev) {
		return false;
	}
	if (so->yPrev + _sceneObjectFramesTable[so->frameNumPrev].hdr.h < so->y) {
		return false;
	}
	if (so->y + _sceneObjectFramesTable[so->frameNum].hdr.h < so->yPrev) {
		return false;
	}
	return true;
}

void Game::redrawObjects(bool skipUpdateScreen) {
	sortObjects();
	int previousObject = -1;
	for (int i = 0; i < _sceneObjectsCount; ++i) {
		SceneObject *so = _sortedSceneObjectsTable[i];
		if (so->state == 1 || so->state == 2) {
			if (previousObject >= 0) {
				redrawObjectBoxes(previousObject, i);
			}
			previousObject = i;
			decodeLzss(_sceneObjectFramesTable[so->frameNum].data, _tempDecodeBuffer);
			if (so->flip == 2) {
				int16 _ax = _bitmapBuffer1.h + 1 - so->y - _sceneObjectFramesTable[so->frameNum].hdr.h;
				drawObjectVerticalFlip(so->x, _ax, _tempDecodeBuffer, &_bitmapBuffer1);
			} else {
				int16 _ax = _bitmapBuffer1.h + 1 - so->y - _sceneObjectFramesTable[so->frameNum].hdr.h;
				drawObject(so->x, _ax, _tempDecodeBuffer, &_bitmapBuffer1);
			}
		}
	}
	if (previousObject >= 0) {
		redrawObjectBoxes(previousObject, previousObject);
	}
	if (_sceneNumber != -1000 && _sceneObjectsCount != 0) {
		if (_gameOver) {
			decodeLzss(_bermudaOvrData + 2, _tempDecodeBuffer);
			drawObject(93, _bitmapBuffer1.h - 230, _tempDecodeBuffer, &_bitmapBuffer1);
		}
		if (_currentBagObject >= 0 && _currentBagObject < _bagObjectsCount && _currentBagAction == 3) {
			drawObject(_bagPosX, _bitmapBuffer1.h + 1 - _bagPosY - getBitmapHeight(_iconBackgroundImage), _iconBackgroundImage, &_bitmapBuffer1);
			int invW = getBitmapWidth(_iconBackgroundImage);
			int invH = getBitmapHeight(_iconBackgroundImage);
			int bagObjW = getBitmapWidth(_bagObjectsTable[_currentBagObject].data);
			int bagObjH = getBitmapHeight(_bagObjectsTable[_currentBagObject].data);
			int y = _bitmapBuffer1.h + 1 - _bagPosY - (invH - bagObjH) / 2 - bagObjH;
			int x = _bagPosX + (invW - bagObjW) / 2;
			drawObject(x, y, _bagObjectsTable[_currentBagObject].data, &_bitmapBuffer1);
		}
		if (_bermudaSprData && _lifeBarDisplayed) {
			drawObject(386, _bitmapBuffer1.h - 18 - getBitmapHeight(_lifeBarImage), _lifeBarImage, &_bitmapBuffer1);
			if (_varsTable[1] == 1) {
				drawObject(150, _bitmapBuffer1.h - 18 - getBitmapHeight(_lifeBarImage), _lifeBarImage, &_bitmapBuffer1);
				drawObject(173, _bitmapBuffer1.h - 18 - getBitmapHeight(_swordIconImage), _swordIconImage, &_bitmapBuffer1);
			} else if (_varsTable[2] == 1) {
				drawObject(150, _bitmapBuffer1.h - 18 - getBitmapHeight(_lifeBarImage), _lifeBarImage, &_bitmapBuffer1);
				int index = MIN(13, 13 - _varsTable[4]);
				drawObject(173, _bitmapBuffer1.h - 31 - getBitmapHeight(_weaponIconImageTable[index]), _weaponIconImageTable[index], &_bitmapBuffer1);
				if (_varsTable[3] < 5) {
					index = (_varsTable[4] <= 0) ? 0 : 1;
					uint8 *p = _ammoIconImageTable[index][_varsTable[3]];
					drawObject(184, _bitmapBuffer1.h - 41 - getBitmapHeight(p), p, &_bitmapBuffer1);
				}
			}
			int index = (_varsTable[0] >= 10) ? 10 : _varsTable[0];
			uint8 *lifeBarFrame = _lifeBarImageTable[index][_lifeBarCurrentFrame];
			drawObject(409, _bitmapBuffer1.h - 36 - getBitmapHeight(lifeBarFrame), lifeBarFrame, &_bitmapBuffer1);
			++_lifeBarCurrentFrame;
			if (_lifeBarCurrentFrame >= 12) {
				_lifeBarCurrentFrame = 0;
			}
		}
		if (_bermudaSprData && (_lifeBarDisplayed || _lifeBarDisplayed2)) {
			if (_varsTable[2] == 1 || _varsTable[1] == 1) {
				win31_stretchBits(&_bitmapBuffer1,
					getBitmapHeight(_lifeBarImage),
					getBitmapWidth(_lifeBarImage),
					_bitmapBuffer1.h - 18 - getBitmapHeight(_lifeBarImage),
					150,
					getBitmapHeight(_lifeBarImage),
					getBitmapWidth(_lifeBarImage),
					19,
					150
				);
			}
			win31_stretchBits(&_bitmapBuffer1,
				getBitmapHeight(_lifeBarImage),
				getBitmapWidth(_lifeBarImage),
				_bitmapBuffer1.h - 18 - getBitmapHeight(_lifeBarImage),
				386,
				getBitmapHeight(_lifeBarImage),
				getBitmapWidth(_lifeBarImage),
				19,
				386
			);
			_lifeBarDisplayed2 = _lifeBarDisplayed;
		}
		if (_previousBagAction == kActionUseObject || _currentBagAction == kActionUseObject) {
			if (_currentBagObject != _previousBagObject || _previousBagAction != _currentBagAction) {
				win31_stretchBits(&_bitmapBuffer1,
					getBitmapHeight(_iconBackgroundImage),
					getBitmapWidth(_iconBackgroundImage),
					_bitmapBuffer1.h + 1 - _bagPosY - getBitmapHeight(_iconBackgroundImage),
					_bagPosX,
					getBitmapHeight(_iconBackgroundImage),
					getBitmapWidth(_iconBackgroundImage),
					_bagPosY,
					_bagPosX
				);
			}
		}
	}
	win31_stretchBits(&_bitmapBuffer1, _bitmapBuffer1.h + 1, _bitmapBuffer1.w + 1, 0, 0, _bitmapBuffer1.h + 1, _bitmapBuffer1.w + 1, 0, 0);
	memcpy(_bitmapBuffer1.bits, _bitmapBuffer3.bits, kGameScreenWidth * kGameScreenHeight);

	if (_bermudaSprData != 0 && _lifeBarDisplayed) {
		copyBufferToBuffer(386,
			_bitmapBuffer1.h + 1 - 19 - getBitmapHeight(_lifeBarImage),
			getBitmapWidth(_lifeBarImage),
			getBitmapHeight(_lifeBarImage),
			&_bitmapBuffer3,
			&_bitmapBuffer1
		);
		copyBufferToBuffer(150,
			_bitmapBuffer1.h + 1 - 19 - getBitmapHeight(_lifeBarImage),
			getBitmapWidth(_lifeBarImage),
			getBitmapHeight(_lifeBarImage),
			&_bitmapBuffer3,
			&_bitmapBuffer1
		);
	}
	_previousBagAction = _currentBagAction;
}

void Game::stopMusic() {
	_mixer->stopSound(_mixerMusicId);
}

void Game::playMusic(const char *name) {
	static const struct {
		const char *fileName;
		int digitalTrack;
	} _midiMapping[] = {
		{ "..\\midi\\flyaway.mid", 2 },
		{ "..\\midi\\jungle1.mid", 3 },
		{ "..\\midi\\sadialog.mid", 4 },
		{ "..\\midi\\caves.mid", 5 },
		{ "..\\midi\\jungle2.mid", 6 },
		{ "..\\midi\\darkcave.mid", 7 },
		{ "..\\midi\\waterdiv.mid", 8 },
		{ "..\\midi\\merian1.mid", 9 },
		{ "..\\midi\\telquad.mid", 10 },
		{ "..\\midi\\gameover.mid", 11 },
		{ "..\\midi\\complete.mid", 12 }
	};
	stopMusic();
	assert(_musicTrack == 0);
	for (unsigned int i = 0; i < ARRAYSIZE(_midiMapping); ++i) {
		if (strcasecmp(_midiMapping[i].fileName, name) == 0) {
			char filePath[512];
			sprintf(filePath, "%s/track%02d.ogg", _musicPath, _midiMapping[i].digitalTrack);
			debug(DBG_GAME, "playMusic('%s') track %s", name, filePath);
			File *f = new File;
			if (f->open(filePath)) {
				_mixer->playSoundVorbis(f, &_mixerMusicId);
			} else {
				delete f;
			}
			return;
		}
	}
	warning("Unable to find mapping for midi music '%s'", name);
}

void Game::changeObjectMotionFrame(int object, int object2, int useObject2, int count1, int count2, int useDx, int dx, int useDy, int dy) {
	SceneObject *so = &_sceneObjectsTable[object];
	if (so->statePrev != 0) {
		int num;
		if (useObject2) {
			num = _sceneObjectsTable[object2].motionInit;
		} else {
			num = _animationsTable[_sceneObjectMotionsTable[so->motionNum1].animNum].firstMotionIndex;
		}
		so->motionNum2 = num + count2 - 1;
		so->frameNum = _sceneObjectMotionsTable[so->motionNum2].firstFrameIndex + count1 - 1;
		if (so->flipPrev == 2) {
			int x = so->xPrev + _sceneObjectFramesTable[so->frameNumPrev].hdr.xPos;
			if (useDx) {
				x -= dx;
			} else {
				x -= _sceneObjectFramesTable[so->frameNum].hdr.xPos;
			}
			so->x = x + _sceneObjectFramesTable[so->frameNumPrev].hdr.w - _sceneObjectFramesTable[so->frameNum].hdr.w;
			int y = so->yPrev - _sceneObjectFramesTable[so->frameNumPrev].hdr.yPos;
			if (useDy) {
				y += dy;
			} else {
				y += _sceneObjectFramesTable[so->frameNum].hdr.yPos;
			}
			so->y = y;
		} else {
			int x = so->xPrev - _sceneObjectFramesTable[so->frameNumPrev].hdr.xPos;
			if (useDx) {
				x += dx;
			} else {
				x += _sceneObjectFramesTable[so->frameNum].hdr.xPos;
			}
			so->x = x;
			int y = so->yPrev - _sceneObjectFramesTable[so->frameNumPrev].hdr.yPos;
			if (useDy) {
				y += dy;
			} else {
				y += _sceneObjectFramesTable[so->frameNum].hdr.yPos;
			}
			so->y = y;
		}
	}
}

int16 Game::getObjectTransformXPos(int object) {
	SceneObject *so = &_sceneObjectsTable[object];
	int16 a0 = _objectScript.fetchNextWord();
	int16 a2 = _objectScript.fetchNextWord();
	int16 a4 = _objectScript.fetchNextWord();

	int16 dx = a0 * _sceneObjectFramesTable[so->frameNumPrev].hdr.w / a2 + a4;
	if (so->flipPrev == 2) {
		dx = _sceneObjectFramesTable[so->frameNumPrev].hdr.w - dx - 1;
	}
	return so->xPrev + dx;
}

int16 Game::getObjectTransformYPos(int object) {
	SceneObject *so = &_sceneObjectsTable[object];
	int16 a0 = _objectScript.fetchNextWord();
	int16 a2 = _objectScript.fetchNextWord();
	int16 a4 = _objectScript.fetchNextWord();

	int16 dy = a0 * _sceneObjectFramesTable[so->frameNumPrev].hdr.h / a2 + a4;
	if (so->flipPrev == 1) {
		dy = _sceneObjectFramesTable[so->frameNumPrev].hdr.h - dy - 1;
	}
	return so->yPrev + dy;
}

bool Game::comparePrevObjectTransformXPos(int object, bool fetchCmp, int cmpX) {
	SceneObject *so = &_sceneObjectsTable[object];

	int16 a0 = _objectScript.fetchNextWord();
	int16 a2 = _objectScript.fetchNextWord();
	int16 a4 = _objectScript.fetchNextWord();
	int16 a6 = _objectScript.fetchNextWord();
	int16 a8 = _objectScript.fetchNextWord();
	int16 aA = _objectScript.fetchNextWord();
	if (fetchCmp) {
		cmpX = _objectScript.fetchNextWord();
	}

	int16 _si = a0 * _sceneObjectFramesTable[so->frameNumPrev].hdr.w / a2 + a4;
	int16 _di = a6 * _sceneObjectFramesTable[so->frameNumPrev].hdr.w / a8 + aA;

	if (so->flipPrev == 2) {
		_si = _sceneObjectFramesTable[so->frameNumPrev].hdr.w - _si;
		_di = _sceneObjectFramesTable[so->frameNumPrev].hdr.w - _di;
	}
	if (so->statePrev != 0) {
		int16 x = so->xPrev + MIN(_si, _di);
		if (x <= cmpX) {
			x = so->xPrev + MAX(_si, _di);
			if (x >= cmpX) {
				return true;
			}
		}
	}
	return false;
}

bool Game::compareObjectTransformXPos(int object, bool fetchCmp, int cmpX) {
	SceneObject *so = &_sceneObjectsTable[object];

	int16 a0 = _objectScript.fetchNextWord();
	int16 a2 = _objectScript.fetchNextWord();
	int16 a4 = _objectScript.fetchNextWord();
	int16 a6 = _objectScript.fetchNextWord();
	int16 a8 = _objectScript.fetchNextWord();
	int16 aA = _objectScript.fetchNextWord();
	if (fetchCmp) {
		cmpX = _objectScript.fetchNextWord();
	}

	int16 _si = a0 * _sceneObjectFramesTable[so->frameNum].hdr.w / a2 + a4;
	int16 _di = a6 * _sceneObjectFramesTable[so->frameNum].hdr.w / a8 + aA;

	if (so->flip == 2) {
		_si = _sceneObjectFramesTable[so->frameNum].hdr.w - _si;
		_di = _sceneObjectFramesTable[so->frameNum].hdr.w - _di;
	}
	if (so->state != 0) {
		int16 x = so->x + MIN(_si, _di);
		if (x <= cmpX) {
			x = so->x + MAX(_si, _di);
			if (x >= cmpX) {
				return true;
			}
		}
	}
	return false;
}

bool Game::comparePrevObjectTransformYPos(int object, bool fetchCmp, int cmpY) {
	SceneObject *so = &_sceneObjectsTable[object];

	int16 a0 = _objectScript.fetchNextWord();
	int16 a2 = _objectScript.fetchNextWord();
	int16 a4 = _objectScript.fetchNextWord();
	int16 a6 = _objectScript.fetchNextWord();
	int16 a8 = _objectScript.fetchNextWord();
	int16 aA = _objectScript.fetchNextWord();
	if (fetchCmp) {
		cmpY = _objectScript.fetchNextWord();
	}

	int16 _si = a0 * _sceneObjectFramesTable[so->frameNumPrev].hdr.h / a2 + a4;
	int16 _di = a6 * _sceneObjectFramesTable[so->frameNumPrev].hdr.h / a8 + aA;

	if (so->flipPrev == 1) {
		_si = _sceneObjectFramesTable[so->frameNumPrev].hdr.h - _si;
		_di = _sceneObjectFramesTable[so->frameNumPrev].hdr.h - _di;
	}
	if (so->statePrev != 0) {
		int16 y = so->yPrev + MIN(_si, _di);
		if (y <= cmpY) {
			y = so->yPrev + MAX(_si, _di);
			if (y >= cmpY) {
				return true;
			}
		}
	}
	return false;
}

bool Game::compareObjectTransformYPos(int object, bool fetchCmp, int cmpY) {
	SceneObject *so = &_sceneObjectsTable[object];

	int16 a0 = _objectScript.fetchNextWord();
	int16 a2 = _objectScript.fetchNextWord();
	int16 a4 = _objectScript.fetchNextWord();
	int16 a6 = _objectScript.fetchNextWord();
	int16 a8 = _objectScript.fetchNextWord();
	int16 aA = _objectScript.fetchNextWord();
	if (fetchCmp) {
		cmpY = _objectScript.fetchNextWord();
	}

	int16 _si = a0 * _sceneObjectFramesTable[so->frameNum].hdr.h / a2 + a4;
	int16 _di = a6 * _sceneObjectFramesTable[so->frameNum].hdr.h / a8 + aA;

	if (so->flip == 1) {
		_si = _sceneObjectFramesTable[so->frameNum].hdr.h - _si;
		_di = _sceneObjectFramesTable[so->frameNum].hdr.h - _di;
	}
	if (so->state != 0) {
		int16 y = so->y + MIN(_si, _di);
		if (y <= cmpY) {
			y = so->y + MAX(_si, _di);
			if (y >= cmpY) {
				return true;
			}
		}
	}
	return false;
}

void Game::setupObjectPos(int object, int object2, int useObject2, int useData, int type1, int type2) {
	SceneObject *so = &_sceneObjectsTable[object];
	if (so->statePrev != 0) {
		int16 _ax, varE, varC, var4, var6, var8, varA;
		if (type1 == 2) {
			int16 a0 = _objectScript.fetchNextWord();
			int16 a2 = _objectScript.fetchNextWord();
			int16 a4 = _objectScript.fetchNextWord();
			var4 = (a0 * _sceneObjectFramesTable[_sceneObjectsTable[object].frameNumPrev].hdr.w) / a2 + a4;
			a0 = _objectScript.fetchNextWord();
			a2 = _objectScript.fetchNextWord();
			a4 = _objectScript.fetchNextWord();
			var6 = (a0 * _sceneObjectFramesTable[_sceneObjectsTable[object].frameNumPrev].hdr.w) / a2 + a4;
			if (var6 < var4) {
				SWAP(var4, var6);
			}
			varC = var6 - var4;
		}
		if (type2 == 2) {
			int16 a0 = _objectScript.fetchNextWord();
			int16 a2 = _objectScript.fetchNextWord();
			int16 a4 = _objectScript.fetchNextWord();
			var8 = (a0 * _sceneObjectFramesTable[_sceneObjectsTable[object].frameNumPrev].hdr.h) / a2 + a4;
			a0 = _objectScript.fetchNextWord();
			a2 = _objectScript.fetchNextWord();
			a4 = _objectScript.fetchNextWord();
			varA = (a0 * _sceneObjectFramesTable[_sceneObjectsTable[object].frameNumPrev].hdr.w) / a2 + a4;
			if (varA < var8) {
				SWAP(var8, varA);
			}
			varE = varA - var8;
		}
		if (useObject2 == 0) {
			_ax = _animationsTable[_sceneObjectMotionsTable[_sceneObjectsTable[object].motionNum1].animNum].firstMotionIndex;
		} else {
			_ax = _sceneObjectsTable[object2].motionInit;
		}
		_sceneObjectsTable[object].motionNum2 = _ax + _objectScript.fetchNextWord() - 1;
		if (useData == 0) {
			_sceneObjectsTable[object].frameNum = _sceneObjectMotionsTable[_sceneObjectsTable[object].motionNum2].firstFrameIndex;
		} else {
			_ax = _sceneObjectMotionsTable[_sceneObjectsTable[object].motionNum2].firstFrameIndex;
			_sceneObjectsTable[object].frameNum = _ax + _objectScript.fetchNextWord() - 1;
		}
		int16 _si = _sceneObjectFramesTable[_sceneObjectsTable[object].frameNum].hdr.xPos;
		_si -= _sceneObjectFramesTable[_sceneObjectsTable[object].frameNumPrev].hdr.xPos;
		if (type1 == 2) {
			 _si = ((var4 - varC + 1) / varC) * varC + _si % varC; // XXX var4 _ var6
			if (_si < var4) {
				_si += varC;
			}
		} else if (type1 == 3) {
			int16 a0 = _objectScript.fetchNextWord();
			_objectScript.fetchNextWord();
			_si = a0 - _sceneObjectFramesTable[_sceneObjectsTable[object].frameNumPrev].hdr.xPos;
		}
		int16 _di = _sceneObjectFramesTable[_sceneObjectsTable[object].frameNum].hdr.yPos;
		_di -= _sceneObjectFramesTable[_sceneObjectsTable[object].frameNumPrev].hdr.yPos;
		if (type2 == 2) {
			_di = ((var8 - varE + 1) / varE) * varE + _di % varE;
			if (_di < var8) {
				_di += varE;
			}
		} else if (type2 == 3) {
			_objectScript.fetchNextWord();
			int16 a2 = _objectScript.fetchNextWord();
			_di = a2 - _sceneObjectFramesTable[_sceneObjectsTable[object].frameNumPrev].hdr.yPos;
		}
		if (_sceneObjectsTable[object].flipPrev == 2) {
			_ax = _sceneObjectsTable[object].xPrev - _si;
			_ax += _sceneObjectFramesTable[_sceneObjectsTable[object].frameNumPrev].hdr.w;
			_ax -= _sceneObjectFramesTable[_sceneObjectsTable[object].frameNum].hdr.w;
		} else {
			_ax = _sceneObjectsTable[object].xPrev + _si;
		}
		_sceneObjectsTable[object].x = _ax;
		_sceneObjectsTable[object].y = _sceneObjectsTable[object].yPrev + _di;
	}
}

bool Game::intersectsBox(int box1, int box2, int x1, int y1, int x2, int y2) {
	const Box *b = derefBox(box1, box2);
	if (b->state == 1) {
		if (b->x1 <= x1 && b->x2 >= x1 && b->y1 <= y1 && b->y2 >= y1) {
			return 1;
		}
		if (b->x1 <= x2 && b->x1 >= x2 && b->y1 <= y2 && b->y2 >= y2) {
			return 1;
		}
		if (b->x2 >= MIN(x1, x2) && b->x1 <= MAX(x1, x2) && b->y1 >= MIN(y1, y2) && b->y2 <= MAX(y1, y2)) {
			if (x1 == x2 || y1 == y2) {
				return 1;
			}
			int iy = y1 - (y1 - y2) * (x1 - b->x1) / (x1 - x2);
			if (b->y1 <= iy && b->y2 >= iy && MIN(y1, y2) <= iy && MAX(y1, y2) >= iy) {
				return 1;
			}
			iy = y1 - (y1 - y2) * (x1 - b->x2) / (x1 - x2);
			if (b->y1 <= iy && b->y2 >= iy && MIN(y1, y2) <= iy && MAX(y1, y2) >= iy) {
				return 1;
			}
			int ix = x1 - (x1 - x2) * (y1 - b->y1) / (y1 - y2);
			if (b->x1 <= ix && b->x2 >= ix && MIN(x1, x2) <= ix && MAX(x1, x2) >= ix) {
				return 1;
			}
			ix = x1 - (x1 - x2) * (y1 - b->y2) / (y1 - y2);
			if (b->x1 <= ix && b->x2 >= ix && MIN(x1, x2) <= ix && MAX(x1, x2) >= ix) {
				return 1;
			}
		}
	}
	return 0;
}
