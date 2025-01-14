#ifndef WALL_H
#define WALL_H

#include "CylinderObject.h"

// ö���࣬����ǽ������
enum class WallType {
    lateral,  // ���ǽ��
    piston,   // ����ǽ��
    bottom    // �ײ�ǽ��
};

// Wall�࣬�̳���CylinderObject��������shared_from_this�Խ��й���ָ�����
class Wall : public CylinderObject, public std::enable_shared_from_this<Wall> {
public:
    // ���캯������������
    Wall();

    // ��ȡ������������ǽ��Ĵ�С����Ⱥ͸߶ȣ�
    void setSize(double w, double h);  // ����ǽ��Ĵ�С
    void getSize(double& w, double& h);  // ��ȡǽ��Ĵ�С

    // ��ȡ������������ǽ����ȶȻ��¶�
    void setHeat(double ht);  // ����ǽ����ȶȻ��¶�
    double getHeat();  // ��ȡǽ����ȶȻ��¶�

    // ��ȡ������������ǽ�������
    void setWallType(WallType type);  // ����ǽ�������
    WallType getWallType();  // ��ȡǽ�������

    // ��������
    std::shared_ptr<Wall> get_shared_this() { return shared_from_this(); }  // ��ȡָ��ǰʵ���Ĺ���ָ��

private:
    // ��Ա����
    double _width, _height;  // ǽ��Ŀ�Ⱥ͸߶ȣ���λ�����أ�
    double _temp;  // ǽ����¶�
    WallType _wallType;  // ǽ�������
};

#endif // WALL_H
