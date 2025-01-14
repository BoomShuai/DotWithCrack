#include "Wall.h"

// Wall��Ĺ��캯��
Wall::Wall() {
    _type = ObjectType::objectWall;  // ��ʼ����������Ϊǽ��
    _wallType = WallType::lateral;  // Ĭ��ǽ������Ϊ���
    _width = 0;  // ��ʼ��ǽ����Ϊ0
    _height = 0;  // ��ʼ��ǽ��߶�Ϊ0
    _temp = 0;  // ��ʼ���¶�Ϊ0
}

// ����ǽ��Ĵ�С
void Wall::setSize(double w, double h) {
    _width = w;  // ����ǽ��Ŀ��
    _height = h;  // ����ǽ��ĸ߶�
}

// ��ȡǽ��Ĵ�С
void Wall::getSize(double& w, double& h) {
    w = _width;  // ��ȡǽ��Ŀ��
    h = _height;  // ��ȡǽ��ĸ߶�
}

// ����ǽ����ȶȻ��¶�
void Wall::setHeat(double ht) {
    std::lock_guard<std::mutex> lock(_mutex);  // �����������Ա�֤�̰߳�ȫ
    _temp = ht;  // ����ǽ����¶�
}

// ��ȡǽ����ȶȻ��¶�
double Wall::getHeat() {
    std::lock_guard<std::mutex> lock(_mutex);  // �����������Ա�֤�̰߳�ȫ
    return _temp;  // ����ǽ����¶�
}

// ǽ�����͵Ļ�ȡ����������
void Wall::setWallType(WallType type) {
    _wallType = type;  // ����ǽ������
}
WallType Wall::getWallType() {
    return _wallType;  // ����ǽ������
}
