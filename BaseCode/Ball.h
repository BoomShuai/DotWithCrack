#ifndef BALL_H
#define BALL_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "CylinderObject.h"
#include "Constants.h"

// 前向声明以避免循环引用
class Wall;

// 消息队列类，用于线程间消息传递
template <typename T>
class MessageQueue {
public:
    T receive();           // 从队列中接收消息
    void send(T&& msg);    // 向队列中发送消息
    int getSize();         // 获取消息队列的大小

private:
    std::mutex _mutex;     // 互斥锁，用于保证线程安全
    std::condition_variable _cond; // 条件变量，用于同步
    std::deque<T> _queue;  // 存储消息的双端队列
};

// 碰撞类型的枚举类
enum class CollisionType {
    wallCollision,        // 与墙壁碰撞
    bottomCollision,      // 与底部碰撞
    pistonCollision       // 与活塞碰撞
};

// 存储碰撞数据的结构体
struct CollisionData {
    CollisionType type;   // 碰撞类型
    double velY;          // 碰撞后Y方向的速度分量
};

// 球类，继承自CylinderObject 
class Ball : public CylinderObject, public std::enable_shared_from_this<Ball> {
public:
    // 构造函数和析构函数
    Ball();

    // 获取器和设置器
    void setWalls(std::vector<std::shared_ptr<Wall>> walls) { _walls = walls; }   // 设置墙壁对象
    void setBalls(std::vector<std::shared_ptr<Ball>> balls) { _balls = balls; }   // 设置球对象

    void setRadius(double r) { _radius = r; }  // 设置球的半径
    double getRadius() { return _radius; }      // 获取球的半径

    // 根据速度和角度设置方向
    void setSpecificDirection(double speed, double angle);

    // 为球选择一个随机方向
    void setRandomDirection(double speed);

    // 设置重力和模拟模式
    void setGravity(double ball_gravity) { _ball_gravity = ball_gravity; } // 设置球的重力
    void setGasMode(bool gas_mode) { _gas_mode = gas_mode; } // 设置是否为气体模式

    // 行为方法
    bool dataIsAvailable();                      // 检查是否有来自其他线程的消息
    CollisionData receiveMsg();                  // 从其他线程接收消息
    void simulate();                            // 处理球的运动
    void setShutdown();                         // 设置标志以退出simulate中的循环

    // 其他方法
    std::shared_ptr<Ball> get_shared_this() { return shared_from_this(); }  // 获取共享指针

private:
    // 行为方法
    void play();                                  // 执行球的模拟
    bool checkWallCollision(double nextX, double nextY, std::shared_ptr<Wall> wall); // 检查与墙壁的碰撞
    bool checkBallCollision(double nextX, double nextY, std::shared_ptr<Ball> ball);  // 检查与另一个球的碰撞
    bool getShutdown();                          // 获取关机标志

    // 成员属性
    double _radius;                                // 球的半径
    double _ball_gravity;                          // 球的重力
    bool _gas_mode;                                // 模拟模式标志
    std::vector<std::shared_ptr<Wall>> _walls;     // 墙壁对象的向量
    std::vector<std::shared_ptr<Ball>> _balls;     // 球对象的向量
    MessageQueue<CollisionData> _msgQueue;         // 用于通信的消息队列
    std::mutex _mutex;                            // 互斥锁，用于线程安全
    bool _shutDown;                               // 结束模拟的标志
};

#endif // BALL_H
