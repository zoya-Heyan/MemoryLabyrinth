#!/bin/bash

# 下载支持中文的字体文件（思源黑体）
# 这个脚本会下载思源黑体字体并放到assets目录

echo "正在下载支持中文的字体文件..."

ASSETS_DIR="../assets"
FONT_URL="https://github.com/adobe-fonts/source-han-sans/releases/download/2.004R/SourceHanSansCN.zip"

# 创建临时目录
TEMP_DIR=$(mktemp -d)
cd "$TEMP_DIR"

# 下载字体
echo "从GitHub下载字体..."
curl -L -o font.zip "$FONT_URL" 2>/dev/null

if [ $? -eq 0 ]; then
    echo "解压字体文件..."
    unzip -q font.zip
    
    # 查找Regular字体文件
    FONT_FILE=$(find . -name "SourceHanSansCN-Regular.otf" -o -name "*SourceHanSansCN-Regular.ttf" | head -1)
    
    if [ -n "$FONT_FILE" ]; then
        # 复制到assets目录
        cp "$FONT_FILE" "$ASSETS_DIR/SourceHanSansCN-Regular.ttf"
        echo "字体已成功下载到: $ASSETS_DIR/SourceHanSansCN-Regular.ttf"
    else
        echo "错误: 在下载的文件中找不到字体文件"
    fi
else
    echo "错误: 下载失败。请手动下载字体："
    echo "1. 访问: https://github.com/adobe-fonts/source-han-sans/releases"
    echo "2. 下载 SourceHanSansCN-Regular.otf"
    echo "3. 将其重命名为 SourceHanSansCN-Regular.ttf 并放到 assets/ 目录"
fi

# 清理临时文件
cd - > /dev/null
rm -rf "$TEMP_DIR"

