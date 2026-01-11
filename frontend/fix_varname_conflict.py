#!/usr/bin/env python3
# Fix variable name conflict in appealmanager.cpp

file_path = 'F:/LTM/week1/frontend/appealmanager.cpp'

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Rename the second scoreLabel to scoreAdjLabel
old = '''    // Score adjustment (optional)
    QLabel *scoreLabel = new QLabel("Điều chỉnh điểm (nếu duyệt, tùy chọn):");
    layout->addWidget(scoreLabel);'''

new = '''    // Score adjustment (optional)
    QLabel *scoreAdjLabel = new QLabel("Điều chỉnh điểm (nếu duyệt, tùy chọn):");
    layout->addWidget(scoreAdjLabel);'''

if old in content:
    content = content.replace(old, new)
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print("[OK] Fixed variable name conflict")
else:
    print("[SKIP] Pattern not found")
