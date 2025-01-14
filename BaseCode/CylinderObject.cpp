#include <algorithm>
#include "CylinderObject.h"

// 初始化静态变量
int CylinderObject::_idCnt = 0; // 全局ID计数器
std::mutex CylinderObject::_mtxCout; // 保护cout的共享互斥锁

// CylinderObject类的构造函数
CylinderObject::CylinderObject() {
    _type = ObjectType::noObject; // 初始化类型为noObject
    _id = _idCnt; // 将当前的ID计数器值赋给该对象的ID
    _posX = 0; // 初始化X坐标为0
    _posY = 0; // 初始化Y坐标为0
    _velX = 0; // 初始化X方向速度为0
    _velY = 0; // 初始化Y方向速度为0
    _speed = 0; // 初始化速度为0
    _mass = 0; // 初始化质量为0
    _idCnt++; // 增加全局ID计数器
}

// 设置对象的位置
void CylinderObject::setPosition(double x, double y) {
    std::lock_guard<std::mutex> lock(_mutex); // 锁定互斥锁以保证线程安全
    _posX = x; // 设置X坐标
    _posY = y; // 设置Y坐标
}

// 获取对象的位置
void CylinderObject::getPosition(double& x, double& y) {
    std::lock_guard<std::mutex> lock(_mutex); // 锁定互斥锁以保证线程安全
    x = _posX; // 获取X坐标
    y = _posY; // 获取Y坐标
}

// 设置对象的速度
void CylinderObject::setVelocity(double velX, double velY) {
    std::lock_guard<std::mutex> lock(_mutex); // 锁定互斥锁以保证线程安全
    _velX = velX; // 设置X方向的速度分量
    _velY = velY; // 设置Y方向的速度分量
    _speed = sqrt(pow(velX, 2) + pow(velY, 2)); // 根据速度计算并设置总速度
}

// 获取对象的速度
void CylinderObject::getVelocity(double& velX, double& velY) {
    std::lock_guard<std::mutex> lock(_mutex); // 锁定互斥锁以保证线程安全
    velX = _velX; // 获取X方向的速度分量
    velY = _velY; // 获取Y方向的速度分量
}

// 获取对象的速度
double CylinderObject::getSpeed() {
    std::lock_guard<std::mutex> lock(_mutex); // 锁定互斥锁以保证线程安全
    _speed = sqrt(pow(_velX, 2) + pow(_velY, 2)); // 根据当前速度重新计算速度
    return _speed; // 返回计算得到的速度
}

// 设置对象的质量
void CylinderObject::setMass(double mass) {
    _mass = mass; // 设置对象的质量
}

// 获取对象的质量
double CylinderObject::getMass() {
    return _mass; // 返回对象的质量
}

// CylinderObject类的析构函数
CylinderObject::~CylinderObject() {
    // 在销毁前，等待与该对象相关的所有线程完成
    std::for_each(_threads.begin(), _threads.end(), [](std::thread& t) {
        t.join(); // 等待线程执行完毕
        });
}
