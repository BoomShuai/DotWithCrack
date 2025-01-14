#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>
#include <memory>
#include <cmath> // ���ڼ��㷽���λ��
#include <SDL_stdinc.h>

class Vehicle {
private:
    double posX, posY;   // ������λ�� (x, y)
    double velocity;      // �������ٶ�
    double direction;     // �����ķ��� (�ԽǶȱ�ʾ)
    double width, length; // �����Ŀ�Ⱥͳ���

public:
    // ���캯������ʼ������������
    Vehicle(double x, double y, double vel, double dir, double w, double l)
        : posX(x), posY(y), velocity(vel), direction(dir), width(w), length(l) {
    }

    // ���³���λ�õķ���
    void updatePosition(double deltaTime) {
        // ���ݳ������ٶȺͷ��򣬸��³�����λ��
        posX += velocity * cos(direction * M_PI / 180.0) * deltaTime;
        posY += velocity * sin(direction * M_PI / 180.0) * deltaTime;
    }

    // ������ײ�ķ���
    void avoidCollision(std::vector<std::shared_ptr<Vehicle>>& vehicles) {
        for (auto& vehicle : vehicles) {
            if (this != vehicle.get()) {  // �ų����������ײ���
                double dx = vehicle->getX() - posX;
                double dy = vehicle->getY() - posY;
                double distance = sqrt(dx * dx + dy * dy);

                // �������֮��ľ���С���趨�İ�ȫ���룬����б�������
                if (distance < 5.0) {  // ���谲ȫ����Ϊ5.0�����Ը�����Ҫ����
                    // ����������ԣ�����
                    velocity = std::max(0.0, velocity - 0.5);
                }
            }
        }
    }

    // Getter �� Setter ����
    double getX() const { return posX; }
    double getY() const { return posY; }
    double getVelocity() const { return velocity; }
    double getDirection() const { return direction; }
    double getWidth() const { return width; }
    double getLength() const { return length; }

    void setVelocity(double vel) { velocity = vel; }
    void setDirection(double dir) { direction = dir; }
};

#endif
