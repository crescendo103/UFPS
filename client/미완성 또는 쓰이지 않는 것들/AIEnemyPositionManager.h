/*
#pragma once
#include <vector>
#include <string>

struct Vector3
{
    float x, y, z;

    Vector3(float xx = 0, float yy = 0, float zz = 0);

    Vector3 operator+(const Vector3& other) const;
    Vector3& operator+=(const Vector3& other);
    Vector3 operator*(float scalar) const;
};

std::vector<Vector3> LoadCSV();
Vector3 RandomPositionWirhScale(Vector3 Pos);
*/
#pragma once
#include <vector>
#include <string>
#include <queue>
struct Vector3
{
    float x, y, z;

    Vector3(float xx = 0, float yy = 0, float zz = 0);

    Vector3 operator+(const Vector3& other) const;
    Vector3& operator+=(const Vector3& other);
    Vector3 operator*(float scalar) const;
};

class AIEnemyPositionManager
{
public:
    AIEnemyPositionManager();

    std::vector<Vector3> LoadCSV();
    void RandomPositionWithScale();
    void PrintAllPoints();
    Vector3 GetRandPos();
    Vector3 GetRandomMapPoint();

    std::vector<Vector3> Points;
    std::queue<Vector3> randPos;
private:
    std::string CsvPath;
    bool Isclear;
};
