#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>
#include <memory>
#include <cmath> // 用于计算方向和位置
#include <SDL_stdinc.h>

class Vehicle {
private:
    double posX, posY;   // 车辆的位置 (x, y)
    double velocity;      // 车辆的速度
    double direction;     // 车辆的方向 (以角度表示)
    double width, length; // 车辆的宽度和长度

public:
    // 构造函数，初始化车辆的属性
    Vehicle(double x, double y, double vel, double dir, double w, double l)
        : posX(x), posY(y), velocity(vel), direction(dir), width(w), length(l) {
    }

    // 更新车辆位置的方法
    void updatePosition(double deltaTime) {
        // 根据车辆的速度和方向，更新车辆的位置
        posX += velocity * cos(direction * M_PI / 180.0) * deltaTime;
        posY += velocity * sin(direction * M_PI / 180.0) * deltaTime;
    }

    // 避免碰撞的方法
    void avoidCollision(std::vector<std::shared_ptr<Vehicle>>& vehicles) {
        for (auto& vehicle : vehicles) {
            if (this != vehicle.get()) {  // 排除与自身的碰撞检测
                double dx = vehicle->getX() - posX;
                double dy = vehicle->getY() - posY;
                double distance = sqrt(dx * dx + dy * dy);

                // 如果车辆之间的距离小于设定的安全距离，则进行避碰处理
                if (distance < 5.0) {  // 假设安全距离为5.0，可以根据需要调整
                    // 假设避碰策略：减速
                    velocity = std::max(0.0, velocity - 0.5);
                }
            }
        }
    }

    // Getter 和 Setter 方法
    double getX() const { return posX; }
    double getY() const { return posY; }
    double getVelocity() const { return velocity; }
    double getDirection() const { return direction; }
    double getWidth() const { return width; }
    double getLength() const { return length; }

    void setVelocity(double vel) { velocity = vel; }
    void setDirection(double dir) { direction = dir; }
};

#endif
