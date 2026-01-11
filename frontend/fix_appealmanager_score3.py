#!/usr/bin/env python3
import sys
sys.stdout.reconfigure(encoding='utf-8')

file_path = 'F:/LTM/week1/frontend/appealmanager.cpp'

with open(file_path, 'r', encoding='utf-8') as f:
    lines = f.readlines()

# Find line 310 (QVBoxLayout *layout...) and insert score labels after it
new_lines = []
for i, line in enumerate(lines):
    new_lines.append(line)
    # After "QVBoxLayout *layout = new QVBoxLayout(dialog);" and blank line
    if i == 309 and 'QVBoxLayout *layout = new QVBoxLayout(dialog);' in line:
        # Insert score display after this
        new_lines.append('    \n')
        new_lines.append('    // Show exam name\n')
        new_lines.append('    QLabel *examLabel = new QLabel("Bài thi: " + examName);\n')
        new_lines.append('    examLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin-bottom: 5px;");\n')
        new_lines.append('    layout->addWidget(examLabel);\n')
        new_lines.append('    \n')
        new_lines.append('    // Show current score\n')
        new_lines.append('    QLabel *scoreLabel = new QLabel(QString("Điểm hiện tại: %1").arg(currentScore, 0, \'f\', 1));\n')
        new_lines.append('    scoreLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2196F3; margin-bottom: 10px;");\n')
        new_lines.append('    layout->addWidget(scoreLabel);\n')
        print(f"[OK] Inserted score display after line {i+1}")

with open(file_path, 'w', encoding='utf-8') as f:
    f.writelines(new_lines)

print("[DONE] Added score display to appeal review dialog")
