# Rössler Attractor — 混沌分形可视化

**主题：化学混沌 · Rössler 系统 · 周期倍增路线**

## 核心

Rössler 吸引子，三个一阶 ODE，周期倍增通往混沌。比 Lorenz 更简洁，参数敏感性强。

```
dx/dt = -y - z
dy/dt = x + a*y
dz/dt = b + z*(x - c)
```

## 参数

- a = 0.2, b = 0.2, c = 5.7（混沌态）
- 积分：RK4，dt = 0.01

## 编译

```bash
cd src && g++ -std=c++17 -O2 -o rossler rossler.cpp -lsfml-graphics -lsfml-window -lsfml-system
./rossler
```

## 混沌特性

- 周期倍增路线：c=2→4.23→4.59→混沌
- 螺旋轨 + 奇异性
- Lyapunov ≈ 0.071

## 灵感

Otto Rössler (1976)，化学振荡器的简化模型。
