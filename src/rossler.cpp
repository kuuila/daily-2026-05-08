/**
 * Rössler Attractor — C++17 + SFML
 * 化学混沌 · 周期倍增路线 · 蝴蝶效应
 *
 * 编译: g++ -std=c++17 -O2 -o rossler rossler.cpp -lsfml-graphics -lsfml-window -lsfml-system
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>

int main(int argc, char** argv) {
    // 混沌参数 (c=5.7 → 混沌态)
    double a = 0.2, b = 0.2, c = 5.7;
    if (argc > 1) {
        if (argv[1][0] == 'p') c = 2.5;      // 周期1
        else if (argv[1][0] == 'd') c = 4.2; // 周期2
        else c = strtod(argv[1], nullptr);
    }

    const double dt = 0.005;
    const int SKIP = 10;      // 跳过瞬态
    const int MAX_PTS = 120000;

    // RK4 积分
    auto dx = [&](double x, double y, double z) { return -y - z; };
    auto dy = [&](double x, double y, double z) { return x + a * y; };
    auto dz = [&](double x, double y, double z) { return b + z * (x - c); };

    auto rk4 = [&](double& x, double& y, double& z) {
        double k1x = dx(x, y, z),       k1y = dy(x, y, z),       k1z = dz(x, y, z);
        double k2x = dx(x+k1x*dt/2, y+k1y*dt/2, z+k1z*dt/2),
               k2y = dy(x+k1x*dt/2, y+k1y*dt/2, z+k1z*dt/2),
               k2z = dz(x+k1x*dt/2, y+k1y*dt/2, z+k1z*dt/2);
        double k3x = dx(x+k2x*dt/2, y+k2y*dt/2, z+k2z*dt/2),
               k3y = dy(x+k2x*dt/2, y+k2y*dt/2, z+k2z*dt/2),
               k3z = dz(x+k2x*dt/2, y+k2y*dt/2, z+k2z*dt/2);
        double k4x = dx(x+k3x*dt, y+k3y*dt, z+k3z*dt),
               k4y = dy(x+k3x*dt, y+k3y*dt, z+k3z*dt),
               k4z = dz(x+k3x*dt, y+k3y*dt, z+k3z*dt);
        x += (k1x + 2*k2x + 2*k3x + k4x) * dt / 6.0;
        y += (k1y + 2*k2y + 2*k3y + k4y) * dt / 6.0;
        z += (k1z + 2*k2z + 2*k3z + k4z) * dt / 6.0;
    };

    // 吸附带计算 (用于颜色映射)
    auto ribbon = [&](double x, double y, double z) -> double {
        double r = std::sqrt(x*x + y*y);
        double theta = std::atan2(y, x);
        return std::fmod(theta + 3.1415926535, 2*M_PI/3.0) / (2*M_PI/3.0);
    };

    // 预热 + 收集点
    std::vector<sf::Vector2f> pts;
    double x = 0.1, y = 0.0, z = 0.0;

    for (int i = 0; i < 50000; ++i) {
        rk4(x, y, z);
        if (i < 1000) continue;
        double px = x * 28.0 + 640.0;
        double py = y * 28.0 + 360.0;
        if (px < 0 || px > 1280 || py < 0 || py > 720) continue;
        pts.emplace_back((float)px, (float)py);
        if ((int)pts.size() >= MAX_PTS) break;
    }

    // SFML 渲染
    sf::RenderWindow window(sf::VideoMode(1280, 720), 
        "Rössler Attractor  c=" + std::to_string(c) + "  (混沌分形)");
    window.setFramerateLimit(60);

    // 预构建颜色查找表 (彩虹渐变)
    std::vector<sf::Color> lut(256);
    for (int i = 0; i < 256; ++i) {
        float t = i / 255.0f;
        // 螺旋色带：蓝→青→绿→黄→红→紫
        sf::Uint8 r = (sf::Uint8)(127.5 + 127.5 * std::sin(6.28318 * t));
        sf::Uint8 g = (sf::Uint8)(127.5 + 127.5 * std::sin(6.28318 * t + 2.094));
        sf::Uint8 b = (sf::Uint8)(127.5 + 127.5 * std::sin(6.28318 * t + 4.189));
        lut[i] = sf::Color(r, g, b, 200);
    }

    sf::VertexArray va(sf::Points, pts.size());

    // 重新计算颜色
    x = 0.1; y = 0.0; z = 0.0;
    int skip = 0, colorIdx = 0;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (skip++ < 10) { va[i].position = pts[i]; va[i].color = sf::Color(60, 60, 60, 120); continue; }
        skip = 0;
        rk4(x, y, z);
        int ri = (int)(ribbon(x, y, z) * 255.0);
        ri = ri < 0 ? 0 : (ri > 255 ? 255 : ri);
        va[i].position = pts[i];
        va[i].color = lut[ri];
    }

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Escape) window.close();
                if (e.key.code == sf::Keyboard::R) {
                    // 重新打点
                    pts.clear();
                    x = 0.1; y = 0.0; z = 0.0;
                    for (int i = 0; i < 50000; ++i) {
                        rk4(x, y, z);
                        if (i < 1000) continue;
                        double px = x * 28.0 + 640.0;
                        double py = y * 28.0 + 360.0;
                        if (px < 0 || px > 1280 || py < 0 || py > 720) continue;
                        pts.emplace_back((float)px, (float)py);
                        if ((int)pts.size() >= MAX_PTS) break;
                    }
                    va = sf::VertexArray(sf::Points, pts.size());
                    x = 0.1; y = 0.0; z = 0.0;
                    skip = 0;
                    for (size_t i = 0; i < pts.size(); ++i) {
                        if (skip++ < 10) { va[i].position = pts[i]; va[i].color = sf::Color(60, 60, 60, 120); continue; }
                        skip = 0;
                        rk4(x, y, z);
                        int ri = (int)(ribbon(x, y, z) * 255.0);
                        ri = ri < 0 ? 0 : (ri > 255 ? 255 : ri);
                        va[i].position = pts[i];
                        va[i].color = lut[ri];
                    }
                }
            }
        }
        window.clear(sf::Color(8, 4, 12));
        window.draw(va);
        window.display();
    }
    return 0;
}
