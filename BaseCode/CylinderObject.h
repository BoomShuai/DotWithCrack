#ifndef CylinderObject_H
#define CylinderObject_H

#include <vector>
#include <thread>
#include <mutex>

// ö���࣬���岻ͬ���͵Ķ���
enum class ObjectType {
    noObject,   // û�ж���
    objectBall,  // �����
    objectWall   // ǽ�����
};

// ��ʾģ����Բ���������
class CylinderObject {
public:
    // ���캯������������
    CylinderObject();
    ~CylinderObject();

    // ��ȡ����������
    int getID() { return _id; } // ���ض����ID
    void setPosition(double x, double y); // ���ö����ڻ����ϵ�λ��
    void getPosition(double& x, double& y); // ��ȡ�����λ��
    void setVelocity(double velX, double velY); // ���ö�����ٶ�
    void getVelocity(double& velX, double& velY); // ��ȡ������ٶ�
    double getSpeed(); // ���㲢���ض�����ٶ�
    void setMass(double m); // ���ö��������
    double getMass(); // ���ض��������
    ObjectType getType() { return _type; } // ���ض��������

    // ģ����Ϊ�����ⷽ��
    virtual void simulate() {} // ������ʵ�֣���Ball��

protected:
    // ��Ա����
    ObjectType _type; // ��ʶ���������
    int _id; // ÿ��Բ�������ΨһID
    double _posX, _posY; // ���������λ�ã����ģ�
    double _velX, _velY, _speed; // �ٶȷ������ٶȣ�����/�룩
    double _mass; // ���������

    // �̹߳���
    std::vector<std::thread> _threads; // �洢�ڸö����������������߳�
    std::mutex _mutex; // ������Ա���ԵĻ�����

    // ���ڹ�����Դ�����ľ�̬��Ա
    static std::mutex _mtxCout; // ����cout�Ĺ�������

private:
    // ��̬��Ա���ԣ�����ID����
    static int _idCnt; // ���ڼ���ID��ȫ�ֱ���
};

#endif // CylinderObject_H
