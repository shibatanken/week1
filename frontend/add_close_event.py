#!/usr/bin/env python3
import sys
sys.stdout.reconfigure(encoding='utf-8')

# Add closeEvent and exit warning to examtaking.h
h_file = 'F:/LTM/week1/frontend/examtaking.h'

with open(h_file, 'r', encoding='utf-8') as f:
    content = f.read()

# Add protected closeEvent after private section
if 'closeEvent' not in content:
    # Find position before the last "};"
    old = '''private:
    Ui::ExamTaking *ui;'''

    new = '''protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::ExamTaking *ui;'''

    if old in content:
        content = content.replace(old, new)
        with open(h_file, 'w', encoding='utf-8') as f:
            f.write(content)
        print("[OK] Added closeEvent to examtaking.h")
    else:
        print("[SKIP] Could not find insertion point in examtaking.h")
else:
    print("[SKIP] closeEvent already exists in examtaking.h")
