#include "Vehicle.h"
#include <cmath> // ���ڼ��㺯��
#include <SDL_stdinc.h>

// ���³���λ��
void Vehicle::updatePosition(double deltaTime) {
    posX += velocity * cos(direction) * deltaTime;
    posY += velocity * sin(direction) * deltaTime;
}

// ������ײ���߼�
void Vehicle::avoidCollision(std::vector<std::shared_ptr<Vehicle>>& vehicles) {
    for (auto& v : vehicles) {
        if (v.get() == this) continue; // �����Լ�

        double distanceX = v->getX() - posX;
        double distanceY = v->getY() - posY;
        double distance = sqrt(distanceX * distanceX + distanceY * distanceY);

        if (distance < (width + v->getWidth())) { // ������ײ����Ϊ�������֮��
            // �򵥵ı������ԣ��ı䷽��
            direction += M_PI / 4; // �ı�45�ȷ���
            velocity *= 0.9; // �����ٶ�
        }
    }
}