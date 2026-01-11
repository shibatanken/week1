#!/usr/bin/env python3
import sys
sys.stdout.reconfigure(encoding='utf-8')

cpp_file = 'F:/LTM/week1/frontend/examtaking.cpp'

with open(cpp_file, 'r', encoding='utf-8') as f:
    lines = f.readlines()

# 1. Add #include <QCloseEvent> after other includes (around line 17)
new_lines = []
include_added = False
for i, line in enumerate(lines):
    new_lines.append(line)
    if not include_added and '#include <QApplication>' in line:
        new_lines.append('#include <QCloseEvent>\n')
        include_added = True
        print("[OK] Added #include <QCloseEvent>")

# 2. Add closeEvent implementation at the end (before final \n)
close_event_code = '''
void ExamTaking::closeEvent(QCloseEvent *event)
{
    // Only warn if exam is in progress (not on result screen)
    if (ui->stackedWidget->currentIndex() == 0 && examTimer->isActive()) {
        QMessageBox::StandardButton reply = QMessageBox::warning(this,
            "Cảnh báo",
            "Bạn chưa hoàn thành bài thi!\\n\\n"
            "Các câu trả lời hiện tại đã được lưu tự động.\\n"
            "Bạn có thể quay lại làm tiếp trong thời gian còn lại.\\n\\n"
            "Nếu hết thời gian mà chưa nộp bài, hệ thống sẽ tự động chấm điểm.\\n\\n"
            "Bạn có chắc muốn thoát?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            event->ignore();
            return;
        }

        // Save all current answers before exiting
        saveAllAnswers();
    }

    event->accept();
}
'''

# Check if closeEvent already exists
has_close_event = any('void ExamTaking::closeEvent' in line for line in new_lines)

if not has_close_event:
    # Add before the last line
    new_lines.insert(-1, close_event_code)
    print("[OK] Added closeEvent implementation")
else:
    print("[SKIP] closeEvent already exists")

with open(cpp_file, 'w', encoding='utf-8') as f:
    f.writelines(new_lines)

print("[DONE] examtaking.cpp updated with exit warning")
