#ifndef CylinderObject_H
#define CylinderObject_H

#include <vector>
#include <thread>
#include <mutex>

// 枚举类，定义不同类型的对象
enum class ObjectType {
    noObject,   // 没有对象
    objectBall,  // 球对象
    objectWall   // 墙体对象
};

// 表示模拟中圆柱对象的类
class CylinderObject {
public:
    // 构造函数和析构函数
    CylinderObject();
    ~CylinderObject();

    // 获取器和设置器
    int getID() { return _id; } // 返回对象的ID
    void setPosition(double x, double y); // 设置对象在画布上的位置
    void getPosition(double& x, double& y); // 获取对象的位置
    void setVelocity(double velX, double velY); // 设置对象的速度
    void getVelocity(double& velX, double& velY); // 获取对象的速度
    double getSpeed(); // 计算并返回对象的速度
    void setMass(double m); // 设置对象的质量
    double getMass(); // 返回对象的质量
    ObjectType getType() { return _type; } // 返回对象的类型

    // 模拟行为的虚拟方法
    virtual void simulate() {} // 由子类实现，如Ball类

protected:
    // 成员属性
    ObjectType _type; // 标识对象的类型
    int _id; // 每个圆柱对象的唯一ID
    double _posX, _posY; // 对象的像素位置（中心）
    double _velX, _velY, _speed; // 速度分量和速度（像素/秒）
    double _mass; // 对象的质量

    // 线程管理
    std::vector<std::thread> _threads; // 存储在该对象内启动的所有线程
    std::mutex _mutex; // 保护成员属性的互斥锁

    // 用于共享资源保护的静态成员
    static std::mutex _mtxCout; // 保护cout的共享互斥锁

private:
    // 静态成员属性，用于ID计数
    static int _idCnt; // 用于计数ID的全局变量
};

#endif // CylinderObject_H
