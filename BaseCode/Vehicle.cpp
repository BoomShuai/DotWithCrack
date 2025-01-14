#include "Vehicle.h"
#include <cmath> // 用于计算函数
#include <SDL_stdinc.h>

// 更新车辆位置
void Vehicle::updatePosition(double deltaTime) {
    posX += velocity * cos(direction) * deltaTime;
    posY += velocity * sin(direction) * deltaTime;
}

// 避免碰撞的逻辑
void Vehicle::avoidCollision(std::vector<std::shared_ptr<Vehicle>>& vehicles) {
    for (auto& v : vehicles) {
        if (v.get() == this) continue; // 跳过自己

        double distanceX = v->getX() - posX;
        double distanceY = v->getY() - posY;
        double distance = sqrt(distanceX * distanceX + distanceY * distanceY);

        if (distance < (width + v->getWidth())) { // 假设碰撞距离为两车宽度之和
            // 简单的避碰策略：改变方向
            direction += M_PI / 4; // 改变45度方向
            velocity *= 0.9; // 减少速度
        }
    }
}