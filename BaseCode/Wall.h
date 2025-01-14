#ifndef WALL_H
#define WALL_H

#include "CylinderObject.h"

// 枚举类，定义墙体类型
enum class WallType {
    lateral,  // 侧壁墙体
    piston,   // 活塞墙体
    bottom    // 底部墙体
};

// Wall类，继承自CylinderObject，并启用shared_from_this以进行共享指针管理
class Wall : public CylinderObject, public std::enable_shared_from_this<Wall> {
public:
    // 构造函数和析构函数
    Wall();

    // 获取器和设置器，墙体的大小（宽度和高度）
    void setSize(double w, double h);  // 设置墙体的大小
    void getSize(double& w, double& h);  // 获取墙体的大小

    // 获取器和设置器，墙体的热度或温度
    void setHeat(double ht);  // 设置墙体的热度或温度
    double getHeat();  // 获取墙体的热度或温度

    // 获取器和设置器，墙体的类型
    void setWallType(WallType type);  // 设置墙体的类型
    WallType getWallType();  // 获取墙体的类型

    // 其他功能
    std::shared_ptr<Wall> get_shared_this() { return shared_from_this(); }  // 获取指向当前实例的共享指针

private:
    // 成员属性
    double _width, _height;  // 墙体的宽度和高度（单位：像素）
    double _temp;  // 墙体的温度
    WallType _wallType;  // 墙体的类型
};

#endif // WALL_H
