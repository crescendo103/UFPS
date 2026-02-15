#include "AIEnemyPositionManager.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mutex>

Vector3::Vector3(float xx, float yy, float zz)
    : x(xx), y(yy), z(zz) {
}

Vector3 Vector3::operator+(const Vector3& other) const
{
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3& Vector3::operator+=(const Vector3& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3 Vector3::operator*(float scalar) const
{
    return Vector3(x * scalar, y * scalar, z * scalar);
}

// ================= Manager =================
std::mutex AIMutex;

AIEnemyPositionManager::AIEnemyPositionManager()
{
    CsvPath = "C:\\Users\\admin\\Desktop\\ss\\fps\\NavMeshCenters.csv";
    srand((unsigned)time(nullptr)); // 딱 한 번

    Points = LoadCSV();
    Isclear = true;
}

std::vector<Vector3> AIEnemyPositionManager::LoadCSV()
{
    std::vector<Vector3> points;
    std::ifstream file(CsvPath);

    if (!file.is_open())
    {
        std::cerr << "CSV 열기 실패: " << CsvPath << std::endl;
        return points;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string token;
        Vector3 v;

        std::getline(ss, token, ',');
        v.x = std::stof(token);

        std::getline(ss, token, ',');
        v.y = std::stof(token);

        std::getline(ss, token, ',');
        v.z = std::stof(token);

        points.push_back(v);
    }

    return points;
}

void AIEnemyPositionManager::RandomPositionWithScale()
{
    

    if (Isclear) {    
        int index = rand() % Points.size();
        randPos.push(Points[index]);
        randPos.push(Points[index]);
        Isclear = false;
    }
    else {
        return;
    }
    
    
}

void AIEnemyPositionManager::PrintAllPoints()
{
    std::lock_guard<std::mutex> lock(AIMutex);

    printf("=== NavMesh Points (%zu개) ===\n", Points.size());

    for (size_t i = 0; i < Points.size(); ++i)
    {
        const Vector3& p = Points[i];
        printf("[%zu] x=%.3f, y=%.3f, z=%.3f\n", i, p.x, p.y, p.z);
    }

    printf("=============================\n");
}

Vector3 AIEnemyPositionManager::GetRandPos()
{
    {
        std::lock_guard<std::mutex> lock(AIMutex);

        if (!randPos.empty()) {
            Vector3 randpos = randPos.front();
            randPos.pop();

            if (randPos.empty()) {
                Isclear = true;
            }

            return randpos;
        }
    }
}
