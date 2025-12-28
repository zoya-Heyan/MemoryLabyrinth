#pragma once

//直接将图片set丢进frame即可使用！
/*
e.g.
std::vector<std::string> frames = {
        "assets/walk1.png",
        "assets/walk2.png",
        "assets/walk3.png",
        "assets/walk4.png"
    };

    Animation playerAnimation(frames, 0.2f); // 每帧 0.2 秒
*/
//参数：frames， 帧数

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Animation {
public:
    Animation(const std::vector<std::string>& frameFiles, float frameTime);

    void update();                       // 更新动画（根据时间切换帧）
    void setPosition(float x, float y);  // 设置位置
    void draw(sf::RenderWindow& window); // 绘制到窗口

private:
    std::vector<sf::Texture> m_textures; // 存放所有帧
    sf::Sprite m_sprite;
    int m_currentFrame;
    float m_frameTime;
    sf::Clock m_clock;
};