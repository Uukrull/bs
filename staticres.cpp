/*
 * Bermuda Syndrome engine rewrite
 * Copyright (C) 2007 Gregory Montoir
 */

#include "game.h"

const GameConditionOpcode Game::_conditionOpTable[] = {
	{    10, &Game::cop_true },
	{   100, &Game::cop_isInRandomRange },
	{   500, &Game::cop_isKeyPressed },
//	{   510, &Game::cop_isKeyNotPressed },
//	{  1000, &Game::cop_testMouseXPos },
//	{  1010, &Game::cop_testMouseYPos },
//	{  1100, &Game::cop_testMouseButtons },
//	{  1200, &Game::cop_isMouseOverObject },
//	{  1300, &Game::cop_testMousePrevObjectTransformXPos },
//	{  1310, &Game::cop_testMousePrevObjectTransformYPos },
//	{  2000, &Game::cop_setTestObjectFromClass },
	{  2500, &Game::cop_isObjectInScene },
	{  3000, &Game::cop_testObjectPrevState },
	{  3010, &Game::cop_testObjectState },
	{  3050, &Game::cop_isObjectInRect },
	{  3100, &Game::cop_testPrevObjectTransformXPos },
	{  3105, &Game::cop_testObjectTransformXPos },
	{  3110, &Game::cop_testPrevObjectTransformYPos },
	{  3150, &Game::cop_testObjectTransformYPos },
//	{  3200, &Game::cop_testObjectPrevZPos },
//	{  3210, &Game::cop_testObjectZPos },
	{  3300, &Game::cop_testObjectPrevFlip },
	{  3310, &Game::cop_testObjectFlip },
	{  3400, &Game::cop_testObjectPrevFrameNum },
	{  3410, &Game::cop_testObjectFrameNum },
	{  3500, &Game::cop_testPrevMotionNum },
	{  3510, &Game::cop_testMotionNum },
	{  3600, &Game::cop_testObjectVar },
	{  3700, &Game::cop_testObjectAndObjectXPos },
	{  3710, &Game::cop_testObjectAndObjectYPos },
//	{  4100, &Game::cop_testObjectMotionXPos },
	{  4110, &Game::cop_testObjectMotionYPos },
	{  6000, &Game::cop_testVar },
	{  6500, &Game::cop_isCurrentBagAction },
	{  7000, &Game::cop_isObjectInBox },
	{  7500, &Game::cop_isObjectNotInBox },
//	{  8000, &Game::cop_isObjectIntersectingBox },
	{  8500, &Game::cop_isObjectNotIntersectingBox },
	{ 10000, &Game::cop_isCurrentBagObject },
//	{ 10100, &Game::cop_isNotCurrentBagObject },
	{ 20000, &Game::cop_isLifeBarDisplayed },
	{ 20010, &Game::cop_isLifeBarNotDisplayed },
	{ 25000, &Game::cop_testLastDialogue },
	{ 30000, &Game::cop_isNextScene }
};

const int Game::_conditionOpCount = ARRAYSIZE(Game::_conditionOpTable);

const GameOperatorOpcode Game::_operatorOpTable[] = {
	{  3000, &Game::oop_initializeObject },
	{  3100, &Game::oop_evalCurrentObjectX },
	{  3110, &Game::oop_evalCurrentObjectY },
//	{  3120, &Game::oop_evalObjectX },
//	{  3130, &Game::oop_evalObjectY },
	{  3200, &Game::oop_evalObjectZ },
	{  3300, &Game::oop_setObjectFlip },
	{  3400, &Game::oop_adjustObjectPos_vv0000 },
	{  3410, &Game::oop_adjustObjectPos_vv1v00 },
//	{  3420, &Game::oop_adjustObjectPos_vv001v },
	{  3430, &Game::oop_adjustObjectPos_vv1v1v },
	{  3440, &Game::oop_setupObjectPos_121 },
//	{  3450, &Game::oop_setupObjectPos_112 },
	{  3460, &Game::oop_setupObjectPos_122 },
//	{  3470, &Game::oop_setupObjectPos_132 },
	{  3480, &Game::oop_setupObjectPos_123 },
	{  3500, &Game::oop_adjustObjectPos_1v0000 },
//	{  3510, &Game::oop_adjustObjectPos_1v1v00 },
//	{  3520, &Game::oop_adjustObjectPos_1v001v },
	{  3530, &Game::oop_adjustObjectPos_1v1v1v },
	{  3540, &Game::oop_setupObjectPos_021 },
//	{  3550, &Game::oop_setupObjectPos_012 },
	{  3560, &Game::oop_setupObjectPos_022 },
//	{  3570, &Game::oop_setupObjectPos_032 },
	{  3580, &Game::oop_setupObjectPos_023 },
	{  4000, &Game::oop_evalObjectVar },
	{  4100, &Game::oop_translateObjectXPos },
	{  4200, &Game::oop_translateObjectYPos },
	{  5000, &Game::oop_setObjectMode },
	{  5100, &Game::oop_setObjectInitPos },
	{  5110, &Game::oop_setObjectTransformInitPos },
//	{  5112, &Game::oop_evalObjectXInit },
//	{  5114, &Game::oop_evalObjectYInit },
	{  5200, &Game::oop_evalObjectZInit },
	{  5300, &Game::oop_setObjectFlipInit },
	{  5400, &Game::oop_setObjectCel },
	{  5500, &Game::oop_resetObjectCel },
	{  6000, &Game::oop_evalVar },
	{  6100, &Game::oop_getSceneNumberInVar },
	{  7000, &Game::oop_disableBox },
	{  7010, &Game::oop_enableBox },
	{  7100, &Game::oop_evalBoxesXPos },
	{  7110, &Game::oop_evalBoxesYPos },
	{  7200, &Game::oop_setBoxToObject },
	{  7300, &Game::oop_extendBoxes },
	{  8000, &Game::oop_saveObjectStatus },
	{ 10000, &Game::oop_addObjectToBag },
	{ 11000, &Game::oop_removeObjectFromBag },
	{ 20000, &Game::oop_playSoundLowerEqualPriority },
	{ 20010, &Game::oop_playSoundLowerPriority },
	{ 25000, &Game::oop_startDialogue },
	{ 30000, &Game::oop_switchSceneClearBoxes },
	{ 30010, &Game::oop_switchSceneCopyBoxes }
};

const int Game::_operatorOpCount = ARRAYSIZE(Game::_operatorOpTable);

const uint16 Game::_fontData[] = {
	0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0033, 0x0033, 0x0033,
	0x0033, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x006C, 0x006C, 0x00FE, 0x006C, 0x006C, 0x0036, 0x0036,
	0x007F, 0x0036, 0x0036, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0018,
	0x003C, 0x0066, 0x0066, 0x0006, 0x003C, 0x0060, 0x0060, 0x0066, 0x003C, 0x0018,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x060E, 0x031B, 0x019B, 0x00DB, 0x006E,
	0x03B0, 0x06D8, 0x06CC, 0x06C6, 0x0383, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0038, 0x006C, 0x006C, 0x006C, 0x0038, 0x001C, 0x00B6, 0x00E6, 0x0066,
	0x00FC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x000C, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x000C, 0x0000, 0x0000, 0x0000, 0x0003,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0003, 0x0000, 0x0000, 0x0000, 0x000C, 0x003F, 0x000C, 0x001E, 0x0012,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0018, 0x0018, 0x0018, 0x007E, 0x0018, 0x0018, 0x0018,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0003, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000C, 0x000C, 0x000C, 0x000C, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0003, 0x0003, 0x0003, 0x0003, 0x0000, 0x0000, 0x0000,
	0x0000, 0x003C, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066,
	0x003C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0018, 0x001E, 0x0018,
	0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x003C, 0x0066, 0x0066, 0x0060, 0x0030, 0x0018, 0x000C,
	0x0006, 0x0006, 0x007E, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x003C,
	0x0066, 0x0060, 0x0060, 0x0038, 0x0060, 0x0060, 0x0060, 0x0066, 0x003C, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0060, 0x0070, 0x0078, 0x0078, 0x006C,
	0x006C, 0x0066, 0x007E, 0x0060, 0x0060, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x007E, 0x0006, 0x0006, 0x0006, 0x003E, 0x0066, 0x0060, 0x0060, 0x0066,
	0x003C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x003C, 0x0066, 0x0006,
	0x0006, 0x003E, 0x0066, 0x0066, 0x0066, 0x0066, 0x003C, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x007E, 0x0060, 0x0030, 0x0030, 0x0018, 0x0018, 0x0018,
	0x000C, 0x000C, 0x000C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x003C,
	0x0066, 0x0066, 0x0066, 0x003C, 0x0066, 0x0066, 0x0066, 0x0066, 0x003C, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x003C, 0x0066, 0x0066, 0x0066, 0x0066,
	0x007C, 0x0060, 0x0060, 0x0066, 0x003C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0003, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0060, 0x0030, 0x0018, 0x000C, 0x0006, 0x000C,
	0x0018, 0x0030, 0x0060, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x007E, 0x0000, 0x0000, 0x007E, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x000C, 0x0018, 0x0030,
	0x0060, 0x0030, 0x0018, 0x000C, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x003C, 0x0066, 0x0066, 0x0060, 0x0030, 0x0018, 0x0018, 0x0000, 0x0018,
	0x0018, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x01E0, 0x0738, 0x0C0C, 0x0DCC,
	0x1B66, 0x1B66, 0x1B36, 0x19B6, 0x19B6, 0x0F6C, 0x000C, 0x0E38, 0x03E0, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0018, 0x0018, 0x003C, 0x003C, 0x0024, 0x0066, 0x0066,
	0x007E, 0x00C3, 0x00C3, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FE,
	0x0186, 0x0186, 0x0186, 0x00FE, 0x0186, 0x0186, 0x0186, 0x0186, 0x00FE, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0078, 0x00CC, 0x0086, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0086, 0x00CC, 0x0078, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x007E, 0x00C6, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x00C6,
	0x007E, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FE, 0x0006, 0x0006,
	0x0006, 0x007E, 0x0006, 0x0006, 0x0006, 0x0006, 0x00FE, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x00FE, 0x0006, 0x0006, 0x0006, 0x007E, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00F8,
	0x018C, 0x0106, 0x0006, 0x0006, 0x01E6, 0x0186, 0x0186, 0x018C, 0x0178, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0186, 0x0186, 0x0186, 0x0186, 0x01FE,
	0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0030, 0x0030, 0x0030,
	0x0030, 0x0030, 0x0030, 0x0030, 0x0033, 0x0033, 0x001E, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x00C6, 0x0066, 0x0036, 0x001E, 0x000E, 0x001E, 0x0036,
	0x0066, 0x00C6, 0x0186, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x00FE, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0606, 0x0606, 0x070E, 0x070E, 0x079E,
	0x079E, 0x06F6, 0x06F6, 0x0666, 0x0666, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0186, 0x018E, 0x019E, 0x019E, 0x01B6, 0x01B6, 0x01E6, 0x01E6, 0x01C6,
	0x0186, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0078, 0x00CC, 0x0186,
	0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x00CC, 0x0078, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x00FE, 0x0186, 0x0186, 0x0186, 0x0186, 0x00FE, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0078,
	0x00CC, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x01E6, 0x00CC, 0x01F8, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FE, 0x0186, 0x0186, 0x0186, 0x0186,
	0x00FE, 0x0186, 0x0186, 0x0186, 0x0306, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x007C, 0x00C6, 0x00C6, 0x0006, 0x001C, 0x0070, 0x00C0, 0x00C6, 0x00C6,
	0x007C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FF, 0x0018, 0x0018,
	0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186,
	0x0186, 0x00CC, 0x0078, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00C3,
	0x00C3, 0x0066, 0x0066, 0x0066, 0x0024, 0x003C, 0x003C, 0x0018, 0x0018, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x30C3, 0x30C3, 0x30C3, 0x19E6, 0x19E6,
	0x0D2C, 0x0F3C, 0x0618, 0x0618, 0x0618, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0183, 0x0183, 0x00C6, 0x006C, 0x0038, 0x0038, 0x006C, 0x00C6, 0x0183,
	0x0183, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0303, 0x0303, 0x0186,
	0x00CC, 0x0078, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x01FF, 0x0180, 0x00C0, 0x0060, 0x0030, 0x0018, 0x000C,
	0x0006, 0x0003, 0x01FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000E,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x000E, 0x0000, 0x0000, 0x0000, 0x0003, 0x0003, 0x0003, 0x0003, 0x0006,
	0x0006, 0x0006, 0x0006, 0x000C, 0x000C, 0x000C, 0x000C, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0007, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0007, 0x0000, 0x0000, 0x0000, 0x0004, 0x000E, 0x001B,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FF, 0x0000, 0x0000, 0x000E, 0x000C,
	0x0018, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x003C, 0x0066,
	0x0078, 0x006C, 0x0066, 0x0066, 0x007C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0006, 0x0006, 0x0006, 0x003E, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066,
	0x003E, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x003C, 0x0066, 0x0006, 0x0006, 0x0006, 0x0066, 0x003C, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0060, 0x0060, 0x0060, 0x007C, 0x0066, 0x0066, 0x0066,
	0x0066, 0x0066, 0x007C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x003C, 0x0066, 0x007E, 0x0006, 0x0006, 0x0066, 0x003C, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000C, 0x0006, 0x0006, 0x000F, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x007C, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066,
	0x007C, 0x0060, 0x0066, 0x003C, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006,
	0x003E, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006,
	0x0006, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0003, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0066, 0x0036,
	0x001E, 0x000E, 0x001E, 0x0036, 0x0066, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x03FE, 0x0666, 0x0666, 0x0666, 0x0666, 0x0666, 0x0666, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x003E, 0x0066, 0x0066, 0x0066,
	0x0066, 0x0066, 0x0066, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x003C, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x003C, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x003E, 0x0066,
	0x0066, 0x0066, 0x0066, 0x0066, 0x003E, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x007C, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066,
	0x007C, 0x0060, 0x0060, 0x0060, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x001E, 0x000E, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x003C, 0x0066, 0x0006, 0x003C,
	0x0060, 0x0066, 0x003C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x0006, 0x000F, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x000C, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0066, 0x0066,
	0x0066, 0x0066, 0x0066, 0x0066, 0x007C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x00C3, 0x00C3, 0x0066, 0x0066, 0x003C, 0x0018,
	0x0018, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0303, 0x0333, 0x01B6, 0x01B6, 0x01FE, 0x00CC, 0x00CC, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00C3, 0x0066, 0x003C, 0x0018,
	0x003C, 0x0066, 0x00C3, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x00C3, 0x00C3, 0x0066, 0x0066, 0x003C, 0x003C, 0x0018, 0x0018,
	0x000C, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x007E, 0x0060,
	0x0030, 0x0018, 0x000C, 0x0006, 0x007E, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0018, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x0006, 0x000C, 0x000C,
	0x000C, 0x000C, 0x000C, 0x0018, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0000, 0x0000, 0x0000, 0x0003, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x000C,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0003, 0x0000, 0x0000, 0x0000, 0x0017,
	0x001D, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x000C, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0003,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006,
	0x0006, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0030, 0x003C, 0x0076,
	0x0016, 0x0016, 0x000E, 0x006E, 0x003C, 0x000C, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x001C, 0x0036, 0x0006, 0x0006, 0x000C, 0x001E, 0x000C, 0x000C, 0x0066,
	0x003E, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0066, 0x003C, 0x0024,
	0x0024, 0x003C, 0x0066, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x00C3, 0x00C3, 0x0066, 0x0066, 0x00FF, 0x0018, 0x00FF,
	0x0018, 0x0018, 0x0018, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x003C, 0x0066, 0x001C, 0x001C, 0x0036,
	0x006C, 0x0038, 0x0038, 0x0066, 0x003C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x001B, 0x001B, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FC, 0x0186, 0x0333,
	0x034B, 0x030B, 0x030B, 0x034B, 0x0333, 0x0186, 0x00FC, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0004, 0x0008, 0x000C, 0x000A, 0x000C, 0x0000, 0x000E,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x006C, 0x0036, 0x001B, 0x0036, 0x006C, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x007E,
	0x0060, 0x0060, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FC, 0x0186, 0x033B,
	0x034B, 0x034B, 0x033B, 0x032B, 0x034B, 0x0186, 0x00FC, 0x0000, 0x0000, 0x0000,
	0x0000, 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000E,
	0x000A, 0x000A, 0x000E, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0018, 0x0018,
	0x007E, 0x0018, 0x0018, 0x0000, 0x007E, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0006, 0x000D, 0x000C, 0x0006, 0x000F, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x000D, 0x0006,
	0x000D, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x001C, 0x000C, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x00FE, 0x0006,
	0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x003C, 0x003E, 0x003E, 0x003E, 0x003E,
	0x003C, 0x0030, 0x0030, 0x0030, 0x0030, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000C, 0x0018, 0x000E,
	0x0000, 0x0000, 0x0000, 0x0006, 0x0007, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000E,
	0x000A, 0x000A, 0x000A, 0x000E, 0x0000, 0x000E, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x001B,
	0x0036, 0x006C, 0x0036, 0x001B, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x060C, 0x030E, 0x018C, 0x00CC, 0x006C, 0x0330, 0x0398, 0x02CC, 0x03C6,
	0x0303, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x060C, 0x030E, 0x018C,
	0x00CC, 0x006C, 0x01B0, 0x0358, 0x030C, 0x0186, 0x03C3, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x060C, 0x031A, 0x018C, 0x00DA, 0x006C, 0x0330, 0x0398,
	0x02CC, 0x03C6, 0x0303, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0018,
	0x0018, 0x0000, 0x0018, 0x0018, 0x000C, 0x0006, 0x0066, 0x0066, 0x003C, 0x0000,
	0x0000, 0x0000, 0x001C, 0x0018, 0x0030, 0x0018, 0x0018, 0x003C, 0x003C, 0x0024,
	0x0066, 0x0066, 0x007E, 0x00C3, 0x00C3, 0x0000, 0x0000, 0x0000, 0x0038, 0x0018,
	0x000C, 0x0018, 0x0018, 0x003C, 0x003C, 0x0024, 0x0066, 0x0066, 0x007E, 0x00C3,
	0x00C3, 0x0000, 0x0000, 0x0000, 0x0018, 0x003C, 0x0066, 0x0018, 0x0018, 0x003C,
	0x003C, 0x0024, 0x0066, 0x0066, 0x007E, 0x00C3, 0x00C3, 0x0000, 0x0000, 0x0000,
	0x005E, 0x007A, 0x0000, 0x0018, 0x0018, 0x003C, 0x003C, 0x0024, 0x0066, 0x0066,
	0x007E, 0x00C3, 0x00C3, 0x0000, 0x0000, 0x0000, 0x0066, 0x0066, 0x0000, 0x0018,
	0x0018, 0x003C, 0x003C, 0x0024, 0x0066, 0x0066, 0x007E, 0x00C3, 0x00C3, 0x0000,
	0x0000, 0x0000, 0x0018, 0x003C, 0x0018, 0x0000, 0x0018, 0x0018, 0x003C, 0x0024,
	0x0066, 0x0066, 0x007E, 0x00C3, 0x00C3, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0FF8, 0x0078, 0x006C, 0x006C, 0x07EC, 0x0066, 0x0066, 0x007E, 0x0063,
	0x0FE3, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0078, 0x00CC, 0x0086,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0086, 0x00CC, 0x0078, 0x0030, 0x0060, 0x0038,
	0x0038, 0x0030, 0x0060, 0x00FE, 0x0006, 0x0006, 0x0006, 0x007E, 0x0006, 0x0006,
	0x0006, 0x0006, 0x00FE, 0x0000, 0x0000, 0x0000, 0x0070, 0x0030, 0x0018, 0x00FE,
	0x0006, 0x0006, 0x0006, 0x007E, 0x0006, 0x0006, 0x0006, 0x0006, 0x00FE, 0x0000,
	0x0000, 0x0000, 0x0030, 0x0078, 0x00CC, 0x00FE, 0x0006, 0x0006, 0x0006, 0x007E,
	0x0006, 0x0006, 0x0006, 0x0006, 0x00FE, 0x0000, 0x0000, 0x0000, 0x00CC, 0x00CC,
	0x0000, 0x00FE, 0x0006, 0x0006, 0x0006, 0x007E, 0x0006, 0x0006, 0x0006, 0x0006,
	0x00FE, 0x0000, 0x0000, 0x0000, 0x0003, 0x0006, 0x0000, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x000C, 0x0006, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0006, 0x0009, 0x0000, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0009, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x007E, 0x00C6, 0x0186, 0x0186, 0x019F, 0x0186, 0x0186, 0x0186, 0x00C6,
	0x007E, 0x0000, 0x0000, 0x0000, 0x00BC, 0x00F4, 0x0000, 0x0186, 0x018E, 0x019E,
	0x019E, 0x01B6, 0x01B6, 0x01E6, 0x01E6, 0x01C6, 0x0186, 0x0000, 0x0000, 0x0000,
	0x0038, 0x0030, 0x0060, 0x0078, 0x00CC, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186,
	0x0186, 0x00CC, 0x0078, 0x0000, 0x0000, 0x0000, 0x0070, 0x0030, 0x0018, 0x0078,
	0x00CC, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x00CC, 0x0078, 0x0000,
	0x0000, 0x0000, 0x0030, 0x0078, 0x00CC, 0x0078, 0x00CC, 0x0186, 0x0186, 0x0186,
	0x0186, 0x0186, 0x0186, 0x00CC, 0x0078, 0x0000, 0x0000, 0x0000, 0x00BC, 0x00F4,
	0x0000, 0x0078, 0x00CC, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x00CC,
	0x0078, 0x0000, 0x0000, 0x0000, 0x00CC, 0x00CC, 0x0000, 0x0078, 0x00CC, 0x0186,
	0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x00CC, 0x0078, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0066, 0x003C, 0x0018,
	0x003C, 0x0066, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x01F8,
	0x00CC, 0x01C6, 0x01E6, 0x01B6, 0x01B6, 0x019E, 0x018E, 0x00CC, 0x007E, 0x0000,
	0x0000, 0x0000, 0x0038, 0x0030, 0x0060, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186,
	0x0186, 0x0186, 0x0186, 0x00CC, 0x0078, 0x0000, 0x0000, 0x0000, 0x0070, 0x0030,
	0x0018, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x00CC,
	0x0078, 0x0000, 0x0000, 0x0000, 0x0030, 0x0078, 0x00CC, 0x0000, 0x0186, 0x0186,
	0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x00CC, 0x0078, 0x0000, 0x0000, 0x0000,
	0x00CC, 0x00CC, 0x0000, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186, 0x0186,
	0x0186, 0x00CC, 0x0078, 0x0000, 0x0000, 0x0000, 0x0070, 0x0030, 0x0018, 0x0303,
	0x0303, 0x0186, 0x00CC, 0x0078, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x007E, 0x00C6, 0x00C6,
	0x00C6, 0x00C6, 0x007E, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x003C, 0x0066, 0x0066, 0x0066, 0x0036, 0x0066, 0x0066, 0x0066, 0x0066,
	0x0036, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x001C, 0x0018, 0x0030, 0x0000,
	0x003C, 0x0066, 0x0078, 0x006C, 0x0066, 0x0066, 0x007C, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0038, 0x0018, 0x000C, 0x0000, 0x003C, 0x0066, 0x0078, 0x006C,
	0x0066, 0x0066, 0x007C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0018, 0x003C,
	0x0066, 0x0000, 0x003C, 0x0066, 0x0078, 0x006C, 0x0066, 0x0066, 0x007C, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x005E, 0x007A, 0x0000, 0x003C, 0x0066,
	0x0078, 0x006C, 0x0066, 0x0066, 0x007C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0066, 0x0066, 0x0000, 0x003C, 0x0066, 0x0078, 0x006C, 0x0066, 0x0066,
	0x007C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0018, 0x003C, 0x0018, 0x0000,
	0x003C, 0x0066, 0x0078, 0x006C, 0x0066, 0x0066, 0x007C, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03FC, 0x0666, 0x07F8, 0x006C,
	0x0066, 0x0666, 0x03FC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x003C, 0x0066, 0x0006, 0x0006, 0x0006, 0x0066, 0x003C, 0x0018,
	0x0030, 0x001C, 0x0000, 0x0000, 0x001C, 0x0018, 0x0030, 0x0000, 0x003C, 0x0066,
	0x007E, 0x0006, 0x0006, 0x0066, 0x003C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0038, 0x0018, 0x000C, 0x0000, 0x003C, 0x0066, 0x007E, 0x0006, 0x0006, 0x0066,
	0x003C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0018, 0x003C, 0x0066, 0x0000,
	0x003C, 0x0066, 0x007E, 0x0006, 0x0006, 0x0066, 0x003C, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0066, 0x0066, 0x0000, 0x003C, 0x0066, 0x007E, 0x0006,
	0x0006, 0x0066, 0x003C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0007, 0x0006,
	0x000C, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x000E, 0x0006, 0x0003, 0x0000, 0x0006, 0x0006,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x000F, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0009, 0x0009, 0x0000,
	0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x006E, 0x0018, 0x0036, 0x0030, 0x007C, 0x0066, 0x0066,
	0x0066, 0x0066, 0x003C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x005E,
	0x007A, 0x0000, 0x003E, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x001C, 0x0018, 0x0030, 0x0000, 0x003C, 0x0066,
	0x0066, 0x0066, 0x0066, 0x0066, 0x003C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0038, 0x0018, 0x000C, 0x0000, 0x003C, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066,
	0x003C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0018, 0x003C, 0x0066, 0x0000,
	0x003C, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x003C, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x005E, 0x007A, 0x0000, 0x003C, 0x0066, 0x0066, 0x0066,
	0x0066, 0x0066, 0x003C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0066,
	0x0066, 0x0000, 0x003C, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x003C, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000C, 0x000C, 0x0000,
	0x001E, 0x0000, 0x000C, 0x000C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x007C, 0x0076, 0x0076, 0x0066, 0x006E, 0x006E,
	0x003E, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x001C, 0x0018, 0x0030, 0x0000,
	0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x007C, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0038, 0x0018, 0x000C, 0x0000, 0x0066, 0x0066, 0x0066, 0x0066,
	0x0066, 0x0066, 0x007C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0018, 0x003C,
	0x0066, 0x0000, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x007C, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0066, 0x0066, 0x0000, 0x0066, 0x0066,
	0x0066, 0x0066, 0x0066, 0x0066, 0x007C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0038, 0x0018, 0x000C, 0x0000, 0x00C3, 0x00C3, 0x0066, 0x0066, 0x003C, 0x003C,
	0x0018, 0x0018, 0x000C, 0x0006, 0x0000, 0x0000, 0x0000, 0x0006, 0x0006, 0x0006,
	0x003E, 0x0066, 0x0066, 0x0066, 0x0066, 0x0066, 0x003E, 0x0006, 0x0006, 0x0006,
	0x0000, 0x0000, 0x0000, 0x0066, 0x0066, 0x0000, 0x00C3, 0x00C3, 0x0066, 0x0066,
	0x003C, 0x003C, 0x0018, 0x0018, 0x000C, 0x0006
};

const uint8 Game::_fontCharWidth[] = {
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x04, 0x03, 0x06, 0x08, 0x07, 0x0B, 0x08, 0x03, 0x04, 0x03, 0x06, 0x07, 0x03, 0x04, 0x03, 0x04,
	0x07, 0x05, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x03, 0x07, 0x07, 0x07, 0x07,
	0x0D, 0x08, 0x09, 0x08, 0x09, 0x08, 0x08, 0x09, 0x09, 0x03, 0x06, 0x09, 0x08, 0x0B, 0x09, 0x09,
	0x09, 0x09, 0x0A, 0x08, 0x08, 0x09, 0x08, 0x0E, 0x09, 0x0A, 0x09, 0x04, 0x04, 0x03, 0x05, 0x08,
	0x05, 0x07, 0x07, 0x07, 0x07, 0x07, 0x04, 0x07, 0x07, 0x03, 0x03, 0x07, 0x03, 0x0B, 0x07, 0x07,
	0x07, 0x07, 0x05, 0x07, 0x04, 0x07, 0x08, 0x0A, 0x08, 0x08, 0x07, 0x05, 0x03, 0x04, 0x05, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x04, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x00, 0x03, 0x07, 0x07, 0x07, 0x08, 0x03, 0x07, 0x05, 0x0A, 0x04, 0x07, 0x07, 0x04, 0x0A, 0x08,
	0x04, 0x07, 0x04, 0x04, 0x05, 0x08, 0x06, 0x03, 0x05, 0x03, 0x04, 0x07, 0x0B, 0x0B, 0x0B, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0C, 0x08, 0x08, 0x08, 0x08, 0x08, 0x03, 0x04, 0x04, 0x04,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x07, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A, 0x08, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x0B, 0x07, 0x07, 0x07, 0x07, 0x07, 0x04, 0x04, 0x04, 0x04,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x05, 0x07, 0x07, 0x07, 0x07, 0x07, 0x08, 0x07, 0x08
};
