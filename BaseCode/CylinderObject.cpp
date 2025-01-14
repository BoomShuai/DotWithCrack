#include <algorithm>
#include "CylinderObject.h"

// ��ʼ����̬����
int CylinderObject::_idCnt = 0; // ȫ��ID������
std::mutex CylinderObject::_mtxCout; // ����cout�Ĺ�������

// CylinderObject��Ĺ��캯��
CylinderObject::CylinderObject() {
    _type = ObjectType::noObject; // ��ʼ������ΪnoObject
    _id = _idCnt; // ����ǰ��ID������ֵ�����ö����ID
    _posX = 0; // ��ʼ��X����Ϊ0
    _posY = 0; // ��ʼ��Y����Ϊ0
    _velX = 0; // ��ʼ��X�����ٶ�Ϊ0
    _velY = 0; // ��ʼ��Y�����ٶ�Ϊ0
    _speed = 0; // ��ʼ���ٶ�Ϊ0
    _mass = 0; // ��ʼ������Ϊ0
    _idCnt++; // ����ȫ��ID������
}

// ���ö����λ��
void CylinderObject::setPosition(double x, double y) {
    std::lock_guard<std::mutex> lock(_mutex); // �����������Ա�֤�̰߳�ȫ
    _posX = x; // ����X����
    _posY = y; // ����Y����
}

// ��ȡ�����λ��
void CylinderObject::getPosition(double& x, double& y) {
    std::lock_guard<std::mutex> lock(_mutex); // �����������Ա�֤�̰߳�ȫ
    x = _posX; // ��ȡX����
    y = _posY; // ��ȡY����
}

// ���ö�����ٶ�
void CylinderObject::setVelocity(double velX, double velY) {
    std::lock_guard<std::mutex> lock(_mutex); // �����������Ա�֤�̰߳�ȫ
    _velX = velX; // ����X������ٶȷ���
    _velY = velY; // ����Y������ٶȷ���
    _speed = sqrt(pow(velX, 2) + pow(velY, 2)); // �����ٶȼ��㲢�������ٶ�
}

// ��ȡ������ٶ�
void CylinderObject::getVelocity(double& velX, double& velY) {
    std::lock_guard<std::mutex> lock(_mutex); // �����������Ա�֤�̰߳�ȫ
    velX = _velX; // ��ȡX������ٶȷ���
    velY = _velY; // ��ȡY������ٶȷ���
}

// ��ȡ������ٶ�
double CylinderObject::getSpeed() {
    std::lock_guard<std::mutex> lock(_mutex); // �����������Ա�֤�̰߳�ȫ
    _speed = sqrt(pow(_velX, 2) + pow(_velY, 2)); // ���ݵ�ǰ�ٶ����¼����ٶ�
    return _speed; // ���ؼ���õ����ٶ�
}

// ���ö��������
void CylinderObject::setMass(double mass) {
    _mass = mass; // ���ö��������
}

// ��ȡ���������
double CylinderObject::getMass() {
    return _mass; // ���ض��������
}

// CylinderObject�����������
CylinderObject::~CylinderObject() {
    // ������ǰ���ȴ���ö�����ص������߳����
    std::for_each(_threads.begin(), _threads.end(), [](std::thread& t) {
        t.join(); // �ȴ��߳�ִ�����
        });
}
