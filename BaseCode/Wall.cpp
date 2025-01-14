#include "Wall.h"

// Wall类的构造函数
Wall::Wall() {
    _type = ObjectType::objectWall;  // 初始化对象类型为墙体
    _wallType = WallType::lateral;  // 默认墙体类型为侧壁
    _width = 0;  // 初始化墙体宽度为0
    _height = 0;  // 初始化墙体高度为0
    _temp = 0;  // 初始化温度为0
}

// 设置墙体的大小
void Wall::setSize(double w, double h) {
    _width = w;  // 设置墙体的宽度
    _height = h;  // 设置墙体的高度
}

// 获取墙体的大小
void Wall::getSize(double& w, double& h) {
    w = _width;  // 获取墙体的宽度
    h = _height;  // 获取墙体的高度
}

// 设置墙体的热度或温度
void Wall::setHeat(double ht) {
    std::lock_guard<std::mutex> lock(_mutex);  // 锁定互斥锁以保证线程安全
    _temp = ht;  // 设置墙体的温度
}

// 获取墙体的热度或温度
double Wall::getHeat() {
    std::lock_guard<std::mutex> lock(_mutex);  // 锁定互斥锁以保证线程安全
    return _temp;  // 返回墙体的温度
}

// 墙体类型的获取器和设置器
void Wall::setWallType(WallType type) {
    _wallType = type;  // 设置墙体类型
}
WallType Wall::getWallType() {
    return _wallType;  // 返回墙体类型
}
