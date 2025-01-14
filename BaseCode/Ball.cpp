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

// ǰ�������Ա���ѭ������
class Wall;

template <typename T>
T MessageQueue<T>::receive()
{
    // ��ȡ�������Ķ�ռ��
    std::unique_lock<std::mutex> uLock(_mutex); // ��Ҫʹ��unique_lock����Ϊ����wait�ڲ�����ʱ����
    // �ȴ�ֱ�����в�Ϊ�գ����������յ�֪ͨ�����»����
    _cond.wait(uLock, [this] { return !_queue.empty(); }); // ����ȴ�״̬���ͷ���������������ʱ�ָ�

    // �Ӷ��е�ĩβ��ȡ��Ϣ
    T msg = std::move(_queue.back());
    _queue.pop_back();

    // std::cout << "��Ϣ " << msg << " �Ѵ������Ϣ�����н���" << std::endl; // ��ѡ�ĵ������

    return msg; // ���ؽ��յ�����Ϣ
}

template <typename T>
void MessageQueue<T>::send(T&& msg)
{
    // ��ס��������ȷ���̰߳�ȫ
    std::lock_guard<std::mutex> uLock(_mutex);

    // std::cout << "��Ϣ " << msg << " �ѷ��͵������Ϣ����" << std::endl; // ��ѡ�ĵ������

    // ����Ϣ��ӵ�����
    _queue.push_back(std::move(msg));

    // ֪ͨһ���ȴ����̣߳���ʾ������Ϣ����
    _cond.notify_one();
}

template <typename T>
int MessageQueue<T>::getSize()
{
    std::lock_guard<std::mutex> uLock(_mutex);  // ��ס��������ȷ���̰߳�ȫ
    return _queue.size();  // ���ض��еĴ�С
}

// ������������  
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

// ����Ĺ��캯��
Ball::Ball() {
    _type = ObjectType::objectBall;  // ���ö�������Ϊ��
    _radius = 0;  // ��ʼ����İ뾶
    _velX = 0;  // ��ʼ��X�����ٶȷ���
    _velY = 0;  // ��ʼ��Y�����ٶȷ���
    _speed = 0;  // ��ʼ������ٶ�
    _shutDown = false;  // ��ʼ���ػ���־Ϊfalse
    _ball_gravity = 0;  // ��ʼ���������
    _gas_mode = false;  // ��ʼ������ģʽ��־Ϊfalse
}

// ���������ض������ϵ��ٶ�
void Ball::setSpecificDirection(double speed, double angle) {
    double vx, vy;  // ���ڴ洢�ٶȵ�X��Y����
    double pi = acos(-1);  // ����Բ�����������Ǻ���
    vx = speed * cos(angle * 2 * pi / 360);  // ����X������ٶȷ���
    vy = -speed * sin(angle * 2 * pi / 360);  // ����Y������ٶȷ���
    setVelocity(vx, vy);  // ʹ�ü�����ķ��������ٶ�
}

// ��������ٶ�Ϊ�������
void Ball::setRandomDirection(double speed) {
    double angle;  // ���ڴ洢����Ƕ�
    std::random_device rd;  // ��������������ӵ�����豸
    std::mt19937 generator(rd());  // ʹ�����ɵ����ӳ�ʼ��÷ɭ��ת����
    std::array<double, 5> intervals{ -60.0, 60.0, 120.0, 240.0, 360.0 };  // ����Ƕȷֲ�������
    std::array<double, 4> weights{ 1.0, 1.0, 1.0, 1.0 };  // Ϊÿ�����������ͬ��Ȩ��
    std::piecewise_constant_distribution<double> distribution(intervals.begin(), intervals.end(), weights.begin());  // ����ֲ�
    angle = distribution(generator);  // ���ݶ���ķֲ���������Ƕ�
    std::cout << "�Ƕ� " << angle << std::endl;  // ������ɵĽǶȵ�����̨
    setSpecificDirection(speed, angle);  // ����������������ϵ��ٶ�
}

// �����Ϣ�������Ƿ�������
bool Ball::dataIsAvailable() {
    return (_msgQueue.getSize() > 0); // �����Ϣ���в�Ϊ�գ�����true
}

// ����Ϣ�����н�����Ϣ
CollisionData Ball::receiveMsg() {
    return _msgQueue.receive(); // �Ӷ��н��ղ�������Ϣ
}

// ��ȡ�ػ���־��״̬
bool Ball::getShutdown() {
    return _shutDown; // ���عػ���־�ĵ�ǰ״̬
}

// ���ùػ���־Ϊtrue
void Ball::setShutdown() {
    _shutDown = true; // ���ùػ���־����ʾģ��Ӧֹͣ
}

// ʵ�����⺯�������߳���ִ�г�Ա����
void Ball::simulate() {
    // ʹ�ó�Ա����"play"����һ�����̣߳�����"this"��Ϊ��������
    // ʹ��emplace_back���������߳���ӵ������_threads�����У��Ա�ʹ���ƶ�����
    _threads.emplace_back(std::thread(&Ball::play, this));
}

// ���߳���ִ�еĺ���
void Ball::play() {
    // ��ӡ���ID���߳�ID
    std::unique_lock<std::mutex> uLock(_mtxCout);
    std::cout << "Ball::simulate �� _id=" << getID() << " �߳�ID=" << std::this_thread::get_id() << std::endl;
    uLock.unlock();

    // ��ʼ������

    // ����ѭ�����ڣ����ڸ������λ�ò����Բ���壩
    int cycleDuration = 10; // ����ģ�����ڵĳ���ʱ�䣨��λ�����룩

    // ��ʼ����ʱ��
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    lastUpdate = std::chrono::system_clock::now();

    // ����ģ��ѭ��
    while (!getShutdown()) {
        // ��������ϴθ���ʱ���ʱ����λ�����룩
        auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();

        // ����ѳ�������ʱ�䣬����λ�ò����Բ����
        if (timeSinceLastUpdate >= cycleDuration) {
            // Ϊ��һ���������ü�ʱ��
            lastUpdate = std::chrono::system_clock::now();

            // ������һ��λ��
            double velX, velY, nextX, nextY;
            double posX, posY, dx, dy;
            getVelocity(velX, velY);
            getPosition(posX, posY);

            // �����������ٶȵ�Ӱ��
            double dVy = _ball_gravity * (timeSinceLastUpdate / 1000.0); // dVy = Ay*dt
            dx = velX * (timeSinceLastUpdate / 1000.0); // dx = Vx * dt
            dy = velY * (timeSinceLastUpdate / 1000.0); // dy = Vy * dt

            nextX = posX + dx;
            nextY = posY + dy;

            // ������Ƿ���ǽ����ײ����ת����
            bool hasCollidedW = false;
            bool hasCollidedP = false;
            bool hasCollidedB = false;

            // ��������ǽ�ڵ���ײ
            for (auto wall : _walls) {
                // �����������ĵ�����ײ��������Y���ƶ�����
                // m0*v0i + m1*v1i = m0*v0f + m1*v1f
                // Ec0i + Ec1i = Ec0f + Ec1f
                if (wall->getWallType() == WallType::piston) {
                    hasCollidedP = checkWallCollision(nextX, nextY, wall);

                    // ������ڻ����ڣ������ײ
                    if (hasCollidedP) {
                        // std::cout << "����-����ײ" << std::endl;

                        double otherVx, otherVy;
                        wall->getVelocity(otherVx, otherVy);
                        double x, y, w, h;
                        wall->getPosition(x, y);
                        wall->getSize(w, h);

                        double velCollisionGas = velY - otherVy;

                        double otherX, otherY;
                        otherX = posX; // ����������ײ����x������ͬ
                        otherY = y;

                        double speed = getSpeed();
                        double otherSpeed = wall->getSpeed();
                        double mass = _mass;
                        double otherM = wall->getMass();

                        double radius = _radius;
                        double otherR = h / 2; // ����������ײ��r = h/2    
                        resolveCollision(posX, posY, velX, velY, speed, mass, radius, otherX, otherY, otherVx,
                            otherVy, otherSpeed, otherM, otherR);

                        // ����ģ��ģ�ͣ�������ģʽ�£������Ĵ��������߳��н���
                        if (!(_gas_mode)) {
                            // ����λ�ú��ٶ�
                            wall->setVelocity(0, otherVy);
                            wall->setPosition(x, otherY);

                            setVelocity(velX, velY);
                            setPosition(posX, posY);
                        }
                        else {
                            // ������Ϣ�����߳��Ը��»�����
                            // F*dt = m0*(v0i-v0f)
                            // F = m1*a                               
                            _msgQueue.send({ CollisionType::pistonCollision, velCollisionGas });

                            // �������λ�ú��ٶ�
                            // m0*v0i + m1*v1i = m0*v0f + m1*v1f
                            // Ec0i + Ec1i = Ec0f + Ec1f                                
                            setVelocity(velX, velY);
                            setPosition(posX, posY);
                        }
                    }
                }
                else {
                    // ��������ǽ�ں͵ײ�����ײ
                    // m0*v0i = m0*v0f

                    // ������Ƿ���ǽ�ڷ�����ײ
                    hasCollidedW = checkWallCollision(nextX, nextY, wall);

                    // �����ǽ�ڷ�����ײ
                    if (hasCollidedW) {
                        // ��֤��ײ�Ƿ����ڵײ����ǲ�ڣ���������Ϣ�����߳�
                        if (wall->getWallType() == WallType::lateral) {
                            // std::cout << "��������ײ" << std::endl << std::endl;

                            // ������Ϣ����ʾ�����˲����ײ��������Y������ٶ�
                            _msgQueue.send({ CollisionType::wallCollision, velY });
                        }
                        else if (wall->getWallType() == WallType::bottom) {
                            // std::cout << "����ײ���ײ" << std::endl << std::endl;

                            // ������Ϣ����ʾ�����˵ײ���ײ��������Y������ٶ�
                            _msgQueue.send({ CollisionType::bottomCollision, velY });
                        }
                    }

                    // �����������ײ���ı���ķ���
                    if (hasCollidedW) {
                        double x, y;
                        wall->getPosition(x, y); // ��ȡǽ�ڵ�λ��

                        // �����ײ������x�᷽������x�������ٶ�                          
                        if (checkWallCollision(nextX, posY, wall)) {
                            if (posX < x) { // ǽ����ǰ��
                                velX = -abs(velX);
                            }
                            else { // ǽ���ں���
                                velX = abs(velX);
                            }
                        }

                        // �����ײ������y�᷽������y�������ٶ� 
                        if (checkWallCollision(posX, nextY, wall)) {
                            if (posY > y) { // ǽ���ں���
                                velY = abs(velY);
                            }
                            else { // ǽ����ǰ��
                                velY = -abs(velY);
                            }

                            // ��������ٶȣ���ģ��ײ����¶�ЧӦ
                            if (wall->getWallType() == WallType::bottom) {
                                if (abs(velY) < 1.0 * wall->getHeat()) {
                                    velY = -1.0 * wall->getHeat();
                                }
                            }
                        }

                        // ��ײ���������ٶȺ�λ��
                        setVelocity(velX, velY);
                        setPosition(nextX, nextY);
                        break; // �˳�ѭ������ײ�Ѵ���
                    }
                }
            }// ��������ǽ�ڵ���ײ 

            // �����������������ײ
            // m0*v0i + m1*v1i = m0*v0f + m1*v1f
            // Ec0i + Ec1i = Ec0f + Ec1f
            if (!hasCollidedW && !hasCollidedP) {
                int thisBallId = getID(); // ��ȡ��ǰ���ID
                for (auto ball : _balls) { // ����������
                    int otherBallId = ball->getID(); // ��ȡ��һ�����ID
                    if (thisBallId != otherBallId) { // ����Ƿ��ǲ�ͬ����
                        hasCollidedB = checkBallCollision(nextX, nextY, ball); // ����Ƿ�����ײ
                        // std::cout << "����ײ id1 " << thisBallId << " id2 " 
                        //<< otherBallId << std::endl;

                        // �����������ײ���ı���ķ��򣨵�����ײ��
                        if (hasCollidedB) {
                            double otherX, otherY;
                            ball->getPosition(otherX, otherY); // ��ȡ��һ�����λ��

                            double otherVx, otherVy;
                            ball->getVelocity(otherVx, otherVy); // ��ȡ��һ������ٶ�

                            double speed = getSpeed(); // ��ȡ��ǰ����ٶ�
                            double otherSpeed = ball->getSpeed(); // ��ȡ��һ������ٶ�

                            double mass = getMass(); // ��ȡ��ǰ�������
                            double otherM = ball->getMass(); // ��ȡ��һ���������

                            double radius = getRadius(); // ��ȡ��ǰ��İ뾶
                            double otherR = ball->getRadius(); // ��ȡ��һ����İ뾶

                            resolveCollision(posX, posY, velX, velY, speed, mass, radius, otherX, otherY,
                                otherVx, otherVy, otherSpeed, otherM, otherR); // �����ײ

                            // ��ײ������ٶ�
                            setVelocity(velX, velY + dVy); // ���µ�ǰ����ٶ�
                            ball->setVelocity(otherVx, otherVy); // ������һ������ٶ�

                            // ��ײ�����λ��
                            setPosition(posX, posY); // ���µ�ǰ���λ��
                            ball->setPosition(otherX, otherY); // ������һ�����λ��

                            break; // �˳�ѭ������ײ�Ѵ���
                        }
                    }
                }
            }// �����µ���������ײ

            // ���û�з�����ײ��ֻ����λ�ú��ٶ�
            if (!hasCollidedW && !hasCollidedB && !hasCollidedP) {
                setPosition(nextX, nextY); // �������λ��
                setVelocity(velX, velY + dVy); // ��������ٶ�
            }
        }// ��������

         // ÿ�ε���˯���Լ���CPUʹ����
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // ��ͣ�߳�1����

    }// ����ģ��ѭ�� 
}

// ��֤����������Բ��ǽ�ڵ���ײ
bool Ball::checkWallCollision(double nextX, double nextY, std::shared_ptr<Wall> wall) {
    double x, y, w, h;  // ǽ�ڵ�λ�úʹ�С
    bool collision = false; // ��ײ��־

    wall->getPosition(x, y);  // ��ȡǽ�ڵ�λ��
    wall->getSize(w, h);  // ��ȡǽ�ڵĴ�С
    collision = squareCircleCollision(x, y, w, h, nextX, nextY, _radius);  // �����ײ

    return collision;  // ������ײ���
}

// ��֤������������֮�����ײ
bool Ball::checkBallCollision(double nextX, double nextY, std::shared_ptr<Ball> ball) {
    double x, y, r;  // ��һ�����λ�úͰ뾶
    bool collision = false; // ��ײ��־

    ball->getPosition(x, y);  // ��ȡ��һ�����λ��
    r = ball->getRadius();  // ��ȡ��һ����İ뾶
    collision = circleCircleCollision(nextX, nextY, _radius, x, y, r);  // �����ײ

    return collision;  // ������ײ���
}

// ��֤һ����������Բ��֮�����ײ������Ϊ�����ε�x1,y1,w1,h1��Բ�ε�x2,y2,r2
bool squareCircleCollision(double x1, double y1, double w1, double h1, double x2, double y2, double r2) {
    double closestX, closestY;  // �����ε�Բ�ε������
    bool collision = false;  // ��ײ��־

    // �ҵ�ǽ�ڵ�Բ�ε����x����
    if (x1 + w1 * 0.5 < x2 - r2) {
        closestX = x1 + w1 * 0.5;
    }
    else if (x1 - w1 * 0.5 > x2 + r2) {
        closestX = x1 - w1 * 0.5;
    }
    else {
        closestX = x2;
    }

    // �ҵ�ǽ�ڵ�Բ�ε����y����
    if (y1 + h1 * 0.5 < y2 - r2) {
        closestY = y1 + h1 * 0.5;
    }
    else if (y1 - h1 * 0.5 > y2 + r2) {
        closestY = y1 - h1 * 0.5;
    }
    else {
        closestY = y2;
    }

    // ��������֮��ľ����Ƿ�С�ڰ뾶
    if (distanceToPoint(x2, y2, closestX, closestY) < r2) {
        collision = true;
    }
    return collision;  // ������ײ���
}

// ��֤����Բ��֮�����ײ������ΪԲ��x1,y1,r1��Բ��x2,y2,r2
bool circleCircleCollision(double x1, double y1, double r1, double x2, double y2, double r2) {
    double distance = distanceToPoint(x1, y1, x2, y2);  // ��������Բ��֮��ľ���
    bool collision = (distance < (r1 + r2));  // �������Ƿ�С����Բ�İ뾶��
    return collision;  // ������ײ���
}

// ����������֮��ľ���
double distanceToPoint(double x1, double y1, double x2, double y2) {
    double distance = sqrt(pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0));  // ����ŷ����þ���
    return distance;  // ���ؾ���
}

// �����������֮��ĵ�����ײ
void resolveCollision(double& posX, double& posY, double& velX, double& velY,
    double speed, double mass, double radius,
    double& otherX, double& otherY, double& otherVx, double& otherVy,
    double otherSpeed, double otherM, double otherR) {
    if (mass == 0 || otherM == 0) {
        std::cout << "��������Ϊ�㣡" << std::endl;
        return;
    }

    double angleCol = atan2(otherY - posY, otherX - posX);  // ������ײ�Ƕ�
    double direction = atan2(velY, velX);  // �����һ������ķ���
    double otherDirection = atan2(otherVy, otherVx);  // ����ڶ�������ķ���

    double new_xspeed = speed * cos(direction - angleCol);  // �����һ�������X�����ٶ�
    double new_yspeed = speed * sin(direction - angleCol);  // �����һ�������Y�����ٶ�
    double new_xspeedOther = otherSpeed * cos(otherDirection - angleCol);  // ����ڶ��������X�����ٶ�
    double new_yspeedOther = otherSpeed * sin(otherDirection - angleCol);  // ����ڶ��������Y�����ٶ�

    double final_xspeed = ((mass - otherM) * new_xspeed + (otherM + otherM) * new_xspeedOther) / (mass + otherM);  // �������յ�X���ٶ�
    double final_xspeedOther = ((mass + mass) * new_xspeed + (otherM - mass) * new_xspeedOther) / (mass + otherM);  // ����ڶ������������X���ٶ�
    double final_yspeed = new_yspeed;  // Y���ٶȱ��ֲ���
    double final_yspeedOther = new_yspeedOther;  // �ڶ��������Y���ٶȱ��ֲ���

    double cosAngle = cos(angleCol);  // ������ײ�Ƕȵ�����ֵ
    double sinAngle = sin(angleCol);  // ������ײ�Ƕȵ�����ֵ

    double newVelX = cosAngle * final_xspeed - sinAngle * final_yspeed;  // �����һ�������X�����ٶ�
    double newVelY = sinAngle * final_xspeed + cosAngle * final_yspeed;  // �����һ�������Y�����ٶ�

    double newOtherVelX = cosAngle * final_xspeedOther - sinAngle * final_yspeedOther;  // ����ڶ��������X�����ٶ�
    double newOtherVelY = sinAngle * final_xspeedOther + cosAngle * final_yspeedOther;  // ����ڶ��������Y�����ٶ�

    // ������Сλ�ƾ��룬�������ཻ����ֿ�
    struct Position {
        double x;
        double y;
        double length() {
            return sqrt(pow(x, 2.0) + pow(y, 2.0));  // ����λ�������ĳ���
        }
    } pos1, pos2, posDiff, mtd;

    pos1.x = posX;
    pos1.y = posY;
    pos2.x = otherX;
    pos2.y = otherY;
    posDiff.x = pos1.x - pos2.x;  // ����X��λ�ò�
    posDiff.y = pos1.y - pos2.y;  // ����Y��λ�ò�

    double d = posDiff.length();  // ������λ��֮��ľ���
    double k = (((radius + otherR) - d) / d);  // ���㽫��ֿ�������
    mtd.x = posDiff.x * k;  // ����X�����Сλ�ƾ���
    mtd.y = posDiff.y * k;  // ����Y�����Сλ�ƾ���

    double im = 1 / mass;  // �����һ�������������
    double imOther = 1 / otherM;  // ����ڶ��������������

    // ���������������壬���������ص�
    pos1.x = pos1.x + mtd.x * (im / (im + imOther));
    pos1.y = pos1.y + mtd.y * (im / (im + imOther));
    pos2.x = pos2.x - mtd.x * (imOther / (im + imOther));
    pos2.y = pos2.y - mtd.y * (imOther / (im + imOther));

    // ��������ǽ�ڵ���ײ���������崩Խ�߽�
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
