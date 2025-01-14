#include <iostream>
#include <thread>
#include <future>
#include <memory>
#include <random>
#include <algorithm>
#include <cmath>
#include <array>
#include "Wall.h"
#include "Ball.h"

// 前向声明以避免循环引用
class Wall;

template <typename T>
T MessageQueue<T>::receive()
{
    // 获取互斥锁的独占锁
    std::unique_lock<std::mutex> uLock(_mutex); // 需要使用unique_lock，因为锁在wait内部会暂时解锁
    // 等待直到队列不为空，解锁并在收到通知后重新获得锁
    _cond.wait(uLock, [this] { return !_queue.empty(); }); // 进入等待状态，释放锁并在有新数据时恢复

    // 从队列的末尾获取消息
    T msg = std::move(_queue.back());
    _queue.pop_back();

    // std::cout << "消息 " << msg << " 已从球的消息队列中接收" << std::endl; // 可选的调试输出

    return msg; // 返回接收到的消息
}

template <typename T>
void MessageQueue<T>::send(T&& msg)
{
    // 锁住互斥量，确保线程安全
    std::lock_guard<std::mutex> uLock(_mutex);

    // std::cout << "消息 " << msg << " 已发送到球的消息队列" << std::endl; // 可选的调试输出

    // 将消息添加到队列
    _queue.push_back(std::move(msg));

    // 通知一个等待的线程，表示有新消息可用
    _cond.notify_one();
}

template <typename T>
int MessageQueue<T>::getSize()
{
    std::lock_guard<std::mutex> uLock(_mutex);  // 锁住互斥量，确保线程安全
    return _queue.size();  // 返回队列的大小
}

// 辅助函数声明  
double distanceToPoint(double x1, double y1, double x2, double y2);
bool squareCircleCollision(double x1, double y1, double w1, double h1,
    double x2, double y2, double r2);
bool circleCircleCollision(double x1, double y1, double r1,
    double x2, double y2, double r2);
void resolveCollision(double& posX, double& posY, double& velX, double& velY,
    double speed, double mass, double radius,
    double& otherX, double& otherY,
    double& otherVx, double& otherVy,
    double otherSpeed, double otherM, double otherR);

// 球类的构造函数
Ball::Ball() {
    _type = ObjectType::objectBall;  // 设置对象类型为球
    _radius = 0;  // 初始化球的半径
    _velX = 0;  // 初始化X方向速度分量
    _velY = 0;  // 初始化Y方向速度分量
    _speed = 0;  // 初始化球的速度
    _shutDown = false;  // 初始化关机标志为false
    _ball_gravity = 0;  // 初始化球的重力
    _gas_mode = false;  // 初始化气体模式标志为false
}

// 设置球在特定方向上的速度
void Ball::setSpecificDirection(double speed, double angle) {
    double vx, vy;  // 用于存储速度的X和Y分量
    double pi = acos(-1);  // 计算圆周率用于三角函数
    vx = speed * cos(angle * 2 * pi / 360);  // 计算X方向的速度分量
    vy = -speed * sin(angle * 2 * pi / 360);  // 计算Y方向的速度分量
    setVelocity(vx, vy);  // 使用计算出的分量设置速度
}

// 设置球的速度为随机方向
void Ball::setRandomDirection(double speed) {
    double angle;  // 用于存储随机角度
    std::random_device rd;  // 用于生成随机种子的随机设备
    std::mt19937 generator(rd());  // 使用生成的种子初始化梅森旋转引擎
    std::array<double, 5> intervals{ -60.0, 60.0, 120.0, 240.0, 360.0 };  // 定义角度分布的区间
    std::array<double, 4> weights{ 1.0, 1.0, 1.0, 1.0 };  // 为每个区间分配相同的权重
    std::piecewise_constant_distribution<double> distribution(intervals.begin(), intervals.end(), weights.begin());  // 定义分布
    angle = distribution(generator);  // 根据定义的分布生成随机角度
    std::cout << "角度 " << angle << std::endl;  // 输出生成的角度到控制台
    setSpecificDirection(speed, angle);  // 设置球在随机方向上的速度
}

// 检查消息队列中是否有数据
bool Ball::dataIsAvailable() {
    return (_msgQueue.getSize() > 0); // 如果消息队列不为空，返回true
}

// 从消息队列中接收消息
CollisionData Ball::receiveMsg() {
    return _msgQueue.receive(); // 从队列接收并返回消息
}

// 获取关机标志的状态
bool Ball::getShutdown() {
    return _shutDown; // 返回关机标志的当前状态
}

// 设置关机标志为true
void Ball::setShutdown() {
    _shutDown = true; // 设置关机标志，表示模拟应停止
}

// 实现虚拟函数，在线程中执行成员函数
void Ball::simulate() {
    // 使用成员函数"play"启动一个新线程，并将"this"作为参数传递
    // 使用emplace_back将创建的线程添加到父类的_threads向量中，以便使用移动语义
    _threads.emplace_back(std::thread(&Ball::play, this));
}

// 在线程中执行的函数
void Ball::play() {
    // 打印球的ID和线程ID
    std::unique_lock<std::mutex> uLock(_mtxCout);
    std::cout << "Ball::simulate 球 _id=" << getID() << " 线程ID=" << std::this_thread::get_id() << std::endl;
    uLock.unlock();

    // 初始化变量

    // 定义循环周期（用于更新球的位置并检查圆柱体）
    int cycleDuration = 10; // 单个模拟周期的持续时间（单位：毫秒）

    // 初始化计时器
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    lastUpdate = std::chrono::system_clock::now();

    // 无限模拟循环
    while (!getShutdown()) {
        // 计算距离上次更新时间的时间差（单位：毫秒）
        auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();

        // 如果已超过周期时间，更新位置并检查圆柱体
        if (timeSinceLastUpdate >= cycleDuration) {
            // 为下一个周期重置计时器
            lastUpdate = std::chrono::system_clock::now();

            // 计算下一个位置
            double velX, velY, nextX, nextY;
            double posX, posY, dx, dy;
            getVelocity(velX, velY);
            getPosition(posX, posY);

            // 计算重力加速度的影响
            double dVy = _ball_gravity * (timeSinceLastUpdate / 1000.0); // dVy = Ay*dt
            dx = velX * (timeSinceLastUpdate / 1000.0); // dx = Vx * dt
            dy = velY * (timeSinceLastUpdate / 1000.0); // dy = Vy * dt

            nextX = posX + dx;
            nextY = posY + dy;

            // 检查球是否与墙壁碰撞并反转方向
            bool hasCollidedW = false;
            bool hasCollidedP = false;
            bool hasCollidedB = false;

            // 处理球与墙壁的碰撞
            for (auto wall : _walls) {
                // 处理活塞与球的弹性碰撞（活塞沿Y轴移动）：
                // m0*v0i + m1*v1i = m0*v0f + m1*v1f
                // Ec0i + Ec1i = Ec0f + Ec1f
                if (wall->getWallType() == WallType::piston) {
                    hasCollidedP = checkWallCollision(nextX, nextY, wall);

                    // 如果球在活塞内：标记碰撞
                    if (hasCollidedP) {
                        // std::cout << "活塞-球碰撞" << std::endl;

                        double otherVx, otherVy;
                        wall->getVelocity(otherVx, otherVy);
                        double x, y, w, h;
                        wall->getPosition(x, y);
                        wall->getSize(w, h);

                        double velCollisionGas = velY - otherVy;

                        double otherX, otherY;
                        otherX = posX; // 类似与球碰撞，但x坐标相同
                        otherY = y;

                        double speed = getSpeed();
                        double otherSpeed = wall->getSpeed();
                        double mass = _mass;
                        double otherM = wall->getMass();

                        double radius = _radius;
                        double otherR = h / 2; // 类似与球碰撞，r = h/2    
                        resolveCollision(posX, posY, velX, velY, speed, mass, radius, otherX, otherY, otherVx,
                            otherVy, otherSpeed, otherM, otherR);

                        // 两种模拟模型：在气体模式下，活塞的处理在主线程中进行
                        if (!(_gas_mode)) {
                            // 更新位置和速度
                            wall->setVelocity(0, otherVy);
                            wall->setPosition(x, otherY);

                            setVelocity(velX, velY);
                            setPosition(posX, posY);
                        }
                        else {
                            // 发送消息到主线程以更新活塞：
                            // F*dt = m0*(v0i-v0f)
                            // F = m1*a                               
                            _msgQueue.send({ CollisionType::pistonCollision, velCollisionGas });

                            // 更新球的位置和速度
                            // m0*v0i + m1*v1i = m0*v0f + m1*v1f
                            // Ec0i + Ec1i = Ec0f + Ec1f                                
                            setVelocity(velX, velY);
                            setPosition(posX, posY);
                        }
                    }
                }
                else {
                    // 处理球与墙壁和底部的碰撞
                    // m0*v0i = m0*v0f

                    // 检查球是否与墙壁发生碰撞
                    hasCollidedW = checkWallCollision(nextX, nextY, wall);

                    // 如果与墙壁发生碰撞
                    if (hasCollidedW) {
                        // 验证碰撞是发生在底部还是侧壁，并发送消息到主线程
                        if (wall->getWallType() == WallType::lateral) {
                            // std::cout << "球与侧壁碰撞" << std::endl << std::endl;

                            // 发送消息，表示发生了侧壁碰撞，并带上Y方向的速度
                            _msgQueue.send({ CollisionType::wallCollision, velY });
                        }
                        else if (wall->getWallType() == WallType::bottom) {
                            // std::cout << "球与底部碰撞" << std::endl << std::endl;

                            // 发送消息，表示发生了底部碰撞，并带上Y方向的速度
                            _msgQueue.send({ CollisionType::bottomCollision, velY });
                        }
                    }

                    // 如果发生了碰撞，改变球的方向
                    if (hasCollidedW) {
                        double x, y;
                        wall->getPosition(x, y); // 获取墙壁的位置

                        // 如果碰撞发生在x轴方向，设置x分量的速度                          
                        if (checkWallCollision(nextX, posY, wall)) {
                            if (posX < x) { // 墙壁在前面
                                velX = -abs(velX);
                            }
                            else { // 墙壁在后面
                                velX = abs(velX);
                            }
                        }

                        // 如果碰撞发生在y轴方向，设置y分量的速度 
                        if (checkWallCollision(posX, nextY, wall)) {
                            if (posY > y) { // 墙壁在后面
                                velY = abs(velY);
                            }
                            else { // 墙壁在前面
                                velY = -abs(velY);
                            }

                            // 调整球的速度，以模拟底部的温度效应
                            if (wall->getWallType() == WallType::bottom) {
                                if (abs(velY) < 1.0 * wall->getHeat()) {
                                    velY = -1.0 * wall->getHeat();
                                }
                            }
                        }

                        // 碰撞后更新球的速度和位置
                        setVelocity(velX, velY);
                        setPosition(nextX, nextY);
                        break; // 退出循环，碰撞已处理
                    }
                }
            }// 结束球与墙壁的碰撞 

            // 处理球与其他球的碰撞
            // m0*v0i + m1*v1i = m0*v0f + m1*v1f
            // Ec0i + Ec1i = Ec0f + Ec1f
            if (!hasCollidedW && !hasCollidedP) {
                int thisBallId = getID(); // 获取当前球的ID
                for (auto ball : _balls) { // 遍历所有球
                    int otherBallId = ball->getID(); // 获取另一个球的ID
                    if (thisBallId != otherBallId) { // 检查是否是不同的球
                        hasCollidedB = checkBallCollision(nextX, nextY, ball); // 检查是否发生碰撞
                        // std::cout << "球碰撞 id1 " << thisBallId << " id2 " 
                        //<< otherBallId << std::endl;

                        // 如果发生了碰撞，改变球的方向（弹性碰撞）
                        if (hasCollidedB) {
                            double otherX, otherY;
                            ball->getPosition(otherX, otherY); // 获取另一个球的位置

                            double otherVx, otherVy;
                            ball->getVelocity(otherVx, otherVy); // 获取另一个球的速度

                            double speed = getSpeed(); // 获取当前球的速度
                            double otherSpeed = ball->getSpeed(); // 获取另一个球的速度

                            double mass = getMass(); // 获取当前球的质量
                            double otherM = ball->getMass(); // 获取另一个球的质量

                            double radius = getRadius(); // 获取当前球的半径
                            double otherR = ball->getRadius(); // 获取另一个球的半径

                            resolveCollision(posX, posY, velX, velY, speed, mass, radius, otherX, otherY,
                                otherVx, otherVy, otherSpeed, otherM, otherR); // 解决碰撞

                            // 碰撞后更新速度
                            setVelocity(velX, velY + dVy); // 更新当前球的速度
                            ball->setVelocity(otherVx, otherVy); // 更新另一个球的速度

                            // 碰撞后更新位置
                            setPosition(posX, posY); // 更新当前球的位置
                            ball->setPosition(otherX, otherY); // 更新另一个球的位置

                            break; // 退出循环，碰撞已处理
                        }
                    }
                }
            }// 结束新的球与球碰撞

            // 如果没有发生碰撞，只更新位置和速度
            if (!hasCollidedW && !hasCollidedB && !hasCollidedP) {
                setPosition(nextX, nextY); // 更新球的位置
                setVelocity(velX, velY + dVy); // 更新球的速度
            }
        }// 结束周期

         // 每次迭代睡眠以减少CPU使用率
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 暂停线程1毫秒

    }// 结束模拟循环 
}

// 验证并处理球与圆柱墙壁的碰撞
bool Ball::checkWallCollision(double nextX, double nextY, std::shared_ptr<Wall> wall) {
    double x, y, w, h;  // 墙壁的位置和大小
    bool collision = false; // 碰撞标志

    wall->getPosition(x, y);  // 获取墙壁的位置
    wall->getSize(w, h);  // 获取墙壁的大小
    collision = squareCircleCollision(x, y, w, h, nextX, nextY, _radius);  // 检查碰撞

    return collision;  // 返回碰撞结果
}

// 验证并处理球与球之间的碰撞
bool Ball::checkBallCollision(double nextX, double nextY, std::shared_ptr<Ball> ball) {
    double x, y, r;  // 另一个球的位置和半径
    bool collision = false; // 碰撞标志

    ball->getPosition(x, y);  // 获取另一个球的位置
    r = ball->getRadius();  // 获取另一个球的半径
    collision = circleCircleCollision(nextX, nextY, _radius, x, y, r);  // 检查碰撞

    return collision;  // 返回碰撞结果
}

// 验证一个正方形与圆形之间的碰撞，参数为正方形的x1,y1,w1,h1和圆形的x2,y2,r2
bool squareCircleCollision(double x1, double y1, double w1, double h1, double x2, double y2, double r2) {
    double closestX, closestY;  // 正方形到圆形的最近点
    bool collision = false;  // 碰撞标志

    // 找到墙壁到圆形的最近x坐标
    if (x1 + w1 * 0.5 < x2 - r2) {
        closestX = x1 + w1 * 0.5;
    }
    else if (x1 - w1 * 0.5 > x2 + r2) {
        closestX = x1 - w1 * 0.5;
    }
    else {
        closestX = x2;
    }

    // 找到墙壁到圆形的最近y坐标
    if (y1 + h1 * 0.5 < y2 - r2) {
        closestY = y1 + h1 * 0.5;
    }
    else if (y1 - h1 * 0.5 > y2 + r2) {
        closestY = y1 - h1 * 0.5;
    }
    else {
        closestY = y2;
    }

    // 检查最近点之间的距离是否小于半径
    if (distanceToPoint(x2, y2, closestX, closestY) < r2) {
        collision = true;
    }
    return collision;  // 返回碰撞结果
}

// 验证两个圆形之间的碰撞，参数为圆形x1,y1,r1和圆形x2,y2,r2
bool circleCircleCollision(double x1, double y1, double r1, double x2, double y2, double r2) {
    double distance = distanceToPoint(x1, y1, x2, y2);  // 计算两个圆心之间的距离
    bool collision = (distance < (r1 + r2));  // 检查距离是否小于两圆的半径和
    return collision;  // 返回碰撞结果
}

// 计算两个点之间的距离
double distanceToPoint(double x1, double y1, double x2, double y2) {
    double distance = sqrt(pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0));  // 计算欧几里得距离
    return distance;  // 返回距离
}

// 解决两个物体之间的弹性碰撞
void resolveCollision(double& posX, double& posY, double& velX, double& velY,
    double speed, double mass, double radius,
    double& otherX, double& otherY, double& otherVx, double& otherVy,
    double otherSpeed, double otherM, double otherR) {
    if (mass == 0 || otherM == 0) {
        std::cout << "错误：质量为零！" << std::endl;
        return;
    }

    double angleCol = atan2(otherY - posY, otherX - posX);  // 计算碰撞角度
    double direction = atan2(velY, velX);  // 计算第一个物体的方向
    double otherDirection = atan2(otherVy, otherVx);  // 计算第二个物体的方向

    double new_xspeed = speed * cos(direction - angleCol);  // 计算第一个物体的X轴新速度
    double new_yspeed = speed * sin(direction - angleCol);  // 计算第一个物体的Y轴新速度
    double new_xspeedOther = otherSpeed * cos(otherDirection - angleCol);  // 计算第二个物体的X轴新速度
    double new_yspeedOther = otherSpeed * sin(otherDirection - angleCol);  // 计算第二个物体的Y轴新速度

    double final_xspeed = ((mass - otherM) * new_xspeed + (otherM + otherM) * new_xspeedOther) / (mass + otherM);  // 计算最终的X轴速度
    double final_xspeedOther = ((mass + mass) * new_xspeed + (otherM - mass) * new_xspeedOther) / (mass + otherM);  // 计算第二个物体的最终X轴速度
    double final_yspeed = new_yspeed;  // Y轴速度保持不变
    double final_yspeedOther = new_yspeedOther;  // 第二个物体的Y轴速度保持不变

    double cosAngle = cos(angleCol);  // 计算碰撞角度的余弦值
    double sinAngle = sin(angleCol);  // 计算碰撞角度的正弦值

    double newVelX = cosAngle * final_xspeed - sinAngle * final_yspeed;  // 计算第一个物体的X轴新速度
    double newVelY = sinAngle * final_xspeed + cosAngle * final_yspeed;  // 计算第一个物体的Y轴新速度

    double newOtherVelX = cosAngle * final_xspeedOther - sinAngle * final_yspeedOther;  // 计算第二个物体的X轴新速度
    double newOtherVelY = sinAngle * final_xspeedOther + cosAngle * final_yspeedOther;  // 计算第二个物体的Y轴新速度

    // 计算最小位移距离，用于在相交后将球分开
    struct Position {
        double x;
        double y;
        double length() {
            return sqrt(pow(x, 2.0) + pow(y, 2.0));  // 计算位置向量的长度
        }
    } pos1, pos2, posDiff, mtd;

    pos1.x = posX;
    pos1.y = posY;
    pos2.x = otherX;
    pos2.y = otherY;
    posDiff.x = pos1.x - pos2.x;  // 计算X轴位置差
    posDiff.y = pos1.y - pos2.y;  // 计算Y轴位置差

    double d = posDiff.length();  // 计算两位置之间的距离
    double k = (((radius + otherR) - d) / d);  // 计算将球分开的因子
    mtd.x = posDiff.x * k;  // 计算X轴的最小位移距离
    mtd.y = posDiff.y * k;  // 计算Y轴的最小位移距离

    double im = 1 / mass;  // 计算第一个物体的逆质量
    double imOther = 1 / otherM;  // 计算第二个物体的逆质量

    // 根据质量推拉球体，避免它们重叠
    pos1.x = pos1.x + mtd.x * (im / (im + imOther));
    pos1.y = pos1.y + mtd.y * (im / (im + imOther));
    pos2.x = pos2.x - mtd.x * (imOther / (im + imOther));
    pos2.y = pos2.y - mtd.y * (imOther / (im + imOther));

    // 处理球与墙壁的碰撞，避免球体穿越边界
    if ((pos1.x + radius >= CYLINDER_CENTER_POSITION_X + CYLINDER_WIDTH / 2) ||
        (pos1.x - radius <= CYLINDER_CENTER_POSITION_X - CYLINDER_WIDTH / 2)) {
        newVelX = -1.0 * newVelX;
    }
    if ((pos1.y + radius >= CYLINDER_CENTER_POSITION_Y + CYLINDER_HEIGHT / 2) ||
        (pos1.y - radius <= CYLINDER_CENTER_POSITION_Y - CYLINDER_HEIGHT / 2)) {
        newVelY = -1.0 * newVelY;
    }
    if ((pos2.x + otherR >= CYLINDER_CENTER_POSITION_X + CYLINDER_WIDTH / 2) ||
        (pos2.x - otherR <= CYLINDER_CENTER_POSITION_X - CYLINDER_WIDTH / 2)) {
        newOtherVelX = -1.0 * newOtherVelX;
    }
    if ((pos2.y + otherR >= CYLINDER_CENTER_POSITION_Y + CYLINDER_HEIGHT / 2) ||
        (pos2.y - otherR <= CYLINDER_CENTER_POSITION_Y - CYLINDER_HEIGHT / 2)) {
        newOtherVelY = -1.0 * newOtherVelY;
    }

    velX = newVelX;
    velY = newVelY;

    posX = pos1.x;
    posY = pos1.y;

    otherVx = newOtherVelX;
    otherVy = newOtherVelY;

    otherX = pos2.x;
    otherY = pos2.y;
}
