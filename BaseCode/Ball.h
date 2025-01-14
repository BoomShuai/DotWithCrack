#ifndef BALL_H
#define BALL_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "CylinderObject.h"
#include "Constants.h"

// ǰ�������Ա���ѭ������
class Wall;

// ��Ϣ�����࣬�����̼߳���Ϣ����
template <typename T>
class MessageQueue {
public:
    T receive();           // �Ӷ����н�����Ϣ
    void send(T&& msg);    // ������з�����Ϣ
    int getSize();         // ��ȡ��Ϣ���еĴ�С

private:
    std::mutex _mutex;     // �����������ڱ�֤�̰߳�ȫ
    std::condition_variable _cond; // ��������������ͬ��
    std::deque<T> _queue;  // �洢��Ϣ��˫�˶���
};

// ��ײ���͵�ö����
enum class CollisionType {
    wallCollision,        // ��ǽ����ײ
    bottomCollision,      // ��ײ���ײ
    pistonCollision       // �������ײ
};

// �洢��ײ���ݵĽṹ��
struct CollisionData {
    CollisionType type;   // ��ײ����
    double velY;          // ��ײ��Y������ٶȷ���
};

// ���࣬�̳���CylinderObject 
class Ball : public CylinderObject, public std::enable_shared_from_this<Ball> {
public:
    // ���캯������������
    Ball();

    // ��ȡ����������
    void setWalls(std::vector<std::shared_ptr<Wall>> walls) { _walls = walls; }   // ����ǽ�ڶ���
    void setBalls(std::vector<std::shared_ptr<Ball>> balls) { _balls = balls; }   // ���������

    void setRadius(double r) { _radius = r; }  // ������İ뾶
    double getRadius() { return _radius; }      // ��ȡ��İ뾶

    // �����ٶȺͽǶ����÷���
    void setSpecificDirection(double speed, double angle);

    // Ϊ��ѡ��һ���������
    void setRandomDirection(double speed);

    // ����������ģ��ģʽ
    void setGravity(double ball_gravity) { _ball_gravity = ball_gravity; } // �����������
    void setGasMode(bool gas_mode) { _gas_mode = gas_mode; } // �����Ƿ�Ϊ����ģʽ

    // ��Ϊ����
    bool dataIsAvailable();                      // ����Ƿ������������̵߳���Ϣ
    CollisionData receiveMsg();                  // �������߳̽�����Ϣ
    void simulate();                            // ��������˶�
    void setShutdown();                         // ���ñ�־���˳�simulate�е�ѭ��

    // ��������
    std::shared_ptr<Ball> get_shared_this() { return shared_from_this(); }  // ��ȡ����ָ��

private:
    // ��Ϊ����
    void play();                                  // ִ�����ģ��
    bool checkWallCollision(double nextX, double nextY, std::shared_ptr<Wall> wall); // �����ǽ�ڵ���ײ
    bool checkBallCollision(double nextX, double nextY, std::shared_ptr<Ball> ball);  // �������һ�������ײ
    bool getShutdown();                          // ��ȡ�ػ���־

    // ��Ա����
    double _radius;                                // ��İ뾶
    double _ball_gravity;                          // �������
    bool _gas_mode;                                // ģ��ģʽ��־
    std::vector<std::shared_ptr<Wall>> _walls;     // ǽ�ڶ��������
    std::vector<std::shared_ptr<Ball>> _balls;     // ����������
    MessageQueue<CollisionData> _msgQueue;         // ����ͨ�ŵ���Ϣ����
    std::mutex _mutex;                            // �������������̰߳�ȫ
    bool _shutDown;                               // ����ģ��ı�־
};

#endif // BALL_H
