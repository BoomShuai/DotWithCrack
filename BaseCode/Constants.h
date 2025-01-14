#ifndef CONSTANTS_H
#define CONSTANTS_H

// 屏幕尺寸常量
const double SCREEN_WIDTH = 800;       // 屏幕宽度 480
const double SCREEN_HEIGHT = 800;      // 屏幕高度 480

// 圆柱的位置和尺寸
const double CYLINDER_CENTER_POSITION_X = SCREEN_WIDTH / 2; // 圆柱中心的X坐标
const double CYLINDER_CENTER_POSITION_Y = SCREEN_HEIGHT / 2; // 圆柱中心的Y坐标
const double CYLINDER_WIDTH = SCREEN_WIDTH / 2; // 圆柱的宽度
const double CYLINDER_HEIGHT = static_cast<int>(SCREEN_HEIGHT * 0.80); // 圆柱的高度，转换为整数

// 墙体宽度
const double WALL_WIDTH = 30; // 墙体的宽度，默认为20

#endif // CONSTANTS_H
