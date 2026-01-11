# THIẾT KẾ CÁC TASK - THEO THỨ TỰ ƯU TIÊN

## TỔNG QUAN
Tài liệu này mô tả chi tiết các task cần thực hiện, bao gồm:
- Mô tả tính năng
- Files cần thay đổi
- API endpoints cần thêm/sửa
- Database changes
- UI/UX changes

---

## 📋 TASK 1: THÔNG BÁO KHIẾU NẠI (POPUP) - ƯU TIÊN CAO NHẤT

### Mô tả:
- Khi đăng nhập, nếu có thông báo khiếu nại chưa đọc → hiện popup
- Với học sinh: "Bạn có X thông báo khiếu nại" (khi giáo viên phản hồi)
- Với giáo viên: "Bạn có X khiếu nại mới" (khi học sinh gửi khiếu nại)
- Popup có nút "Xem ngay" → chuyển đến trang khiếu nại

### Files cần thay đổi:

#### Backend:
1. **`backend/src/services/appeal/appeal_service.c`**
   - Thêm function: `get_unread_appeals_count(int user_id, bool is_teacher)`
   - Query: Đếm số appeals có `student_read = FALSE` (nếu student) hoặc `teacher_read = FALSE` (nếu teacher)

2. **`backend/src/controllers/appeal/appeal_controller.c`**
   - Thêm handler: `handle_get_unread_appeals_count()`
   - Route: `GET_UNREAD_APPEALS_COUNT`

3. **`backend/src/routes/routes.c`**
   - Thêm route: `GET_UNREAD_APPEALS_COUNT`

4. **`backend/src/controllers/auth/auth_controller.c`**
   - Sau khi login success, gọi `get_unread_appeals_count()` và trả về trong response
   - Thêm field `unread_appeals_count` vào JSON response của LOGIN_SUCCESS

#### Frontend:
1. **`frontend/signin.cpp`**
   - Sau khi login success, check `unread_appeals_count`
   - Nếu > 0 → hiện QMessageBox với message và nút "Xem ngay"
   - Connect nút "Xem ngay" → emit signal `showAppealManager()`

2. **`frontend/mainwindow.cpp`**
   - Connect signal từ signin → showAppealManager

3. **`frontend/appealmanager.cpp`**
   - Khi load appeals, mark as read sau khi hiển thị
   - Thêm API call: `MARK_APPEAL_AS_READ`

### Database:
- Đã có columns: `student_read`, `teacher_read` trong bảng `appeals` (từ migration `alter_02_appeals_add_read_status.sql`)

### API Endpoints mới:
```
CONTROL GET_UNREAD_APPEALS_COUNT
Body: {"user_id": int, "is_teacher": bool}
Response: DATA JSON UNREAD_APPEALS_COUNT {"count": int}

CONTROL MARK_APPEAL_AS_READ
Body: {"appeal_id": int, "user_id": int, "is_teacher": bool}
Response: NOTIFICATION MARK_APPEAL_AS_READ_SUCCESS
```

---

## 📋 TASK 2: GIÁO VIÊN PHẢN HỒI KHIẾU NẠI - THÊM TEXT FIELD

### Mô tả:
- Thêm QTextEdit (optional) trong AppealManager cho giáo viên
- Khi approve/reject, có thể nhập comment phản hồi
- Comment được lưu vào `teacher_comment` trong database
- Hiển thị comment trong danh sách khiếu nại của học sinh

### Files cần thay đổi:

#### Backend:
1. **`backend/src/services/appeal/appeal_service.c`**
   - Sửa `review_appeal()`: thêm parameter `teacher_comment`
   - Update query: `UPDATE appeals SET ..., teacher_comment = '%s' WHERE id = %d`

2. **`backend/src/controllers/appeal/appeal_controller.c`**
   - Sửa `handle_review_appeal()`: parse `teacher_comment` từ JSON
   - Pass `teacher_comment` vào `review_appeal()`

#### Frontend:
1. **`frontend/appealmanager.h`**
   - Thêm QTextEdit* member: `teacherCommentEdit`

2. **`frontend/appealmanager.cpp`**
   - Trong `displayAppeals()`: Nếu `isTeacherMode`, thêm QTextEdit cho mỗi appeal card
   - Khi approve/reject: lấy text từ QTextEdit và gửi lên server
   - Hiển thị `teacher_comment` trong appeal card của học sinh

3. **`frontend/appealmanager.ui`** (nếu cần)
   - Có thể thêm QTextEdit vào UI hoặc tạo dynamic trong code

### Database:
- Đã có column: `teacher_comment TEXT` trong bảng `appeals`

### API Changes:
```
CONTROL REVIEW_APPEAL
Body: {
  "appeal_id": int,
  "status": "approved" | "rejected",
  "score_adjustment": double,
  "teacher_comment": string (optional)
}
```

---

## 📋 TASK 3: SINH VIÊN KHÔNG ĐƯỢC LÀM LẠI BÀI ĐÃ LÀM

### Mô tả:
- Nếu submission status = "submitted" → không cho làm lại
- Hiển thị điểm thay vì nút "Làm bài"
- Bấm vào → xem bài làm cũ (kết quả chi tiết)

### Files cần thay đổi:

#### Backend:
- Không cần thay đổi (đã có `get_submission_status()`)

#### Frontend:
1. **`frontend/classdetail.cpp`**
   - Trong `loadExamsInClass()`: Với mỗi exam, gọi `get_submission_status()` để check
   - Nếu có submission và status = "submitted":
     - Hiển thị: "✅ [Tên bài] - Điểm: X.X" thay vì "📝 [Tên bài]"
     - Khi click → emit signal `viewExamResult(submission_id)` thay vì `startExamForStudent()`

2. **`frontend/classdetail.h`**
   - Thêm signal: `void viewExamResult(int submissionId);`

3. **`frontend/mainwindow.cpp`**
   - Connect `viewExamResult` → show exam result view
   - Cần tạo method `showExamResult(int submissionId)` để hiển thị kết quả

4. **`frontend/examtaking.cpp`**
   - Trong `startExam()`: Check submission status trước
   - Nếu status = "submitted" → không cho join, hiển thị thông báo và chuyển sang view result

### Logic Flow:
```
Student clicks exam
  ↓
Check get_submission_status()
  ↓
If status == "submitted":
  → Show result view (existing showResult())
Else:
  → Allow startExam()
```

---

## 📋 TASK 4: TẠM DỪNG LÀM BÀI - LƯU TIẾN ĐỘ

### Mô tả:
- Khi thoát ExamTaking (bấm back hoặc close window):
  - Hiện dialog: "Bạn chưa hoàn thành bài thi. Kết quả hiện tại sẽ được lưu tạm thời."
  - Lưu tất cả answers đã chọn vào database
  - Lưu thời gian còn lại vào submission
- Khi quay lại:
  - Nếu còn thời gian → tiếp tục làm bài với answers đã lưu
  - Nếu hết thời gian → tự động submit và chấm điểm

### Files cần thay đổi:

#### Backend:
1. **`backend/src/services/exam/exam_service.c`**
   - Thêm function: `save_exam_progress(int submission_id, int remaining_seconds)`
   - Update: `UPDATE exam_submissions SET remaining_seconds = %d WHERE id = %d`
   - Thêm function: `check_and_auto_submit_expired_exams()`
   - Query: Tìm submissions có `status = 'in_progress'` và `remaining_seconds <= 0` hoặc `started_at + time_limit < NOW()`
   - Auto call `submit_exam()` cho các submissions này

2. **`backend/src/controllers/exam/exam_controller.c`**
   - Thêm handler: `handle_save_exam_progress()`
   - Route: `SAVE_EXAM_PROGRESS`

3. **`backend/src/routes/routes.c`**
   - Thêm route: `SAVE_EXAM_PROGRESS`

4. **`backend/src/migrations/`** (tạo file mới)
   - `alter_14_exam_submissions_add_remaining_seconds.sql`
   - `ALTER TABLE exam_submissions ADD COLUMN remaining_seconds INT DEFAULT NULL;`

#### Frontend:
1. **`frontend/examtaking.h`**
   - Thêm method: `void saveProgress()`
   - Override: `void closeEvent(QCloseEvent *event)`

2. **`frontend/examtaking.cpp`**
   - Trong `closeEvent()`: Hiện QMessageBox confirm, nếu OK → gọi `saveProgress()`
   - `saveProgress()`: Gửi tất cả answers + remaining_seconds lên server
   - Trong `startExam()`: Check submission status
     - Nếu `status = "in_progress"` và có `remaining_seconds`:
       - Load answers đã lưu
       - Set timer với `remaining_seconds`
       - Hiển thị message: "Bạn đang làm dở bài thi này. Tiếp tục?"
   - Trong `updateTimer()`: Mỗi giây gọi `saveProgress()` để auto-save

3. **`frontend/mainwindow.cpp`**
   - Khi show ExamTaking: Check submission status trước
   - Nếu có submission in_progress → hiện dialog confirm tiếp tục

### Database:
- Thêm column: `remaining_seconds INT` vào `exam_submissions`

### API Endpoints mới:
```
CONTROL SAVE_EXAM_PROGRESS
Body: {
  "submission_id": int,
  "remaining_seconds": int,
  "answers": [{"question_id": int, "answer": string}, ...]
}
Response: NOTIFICATION SAVE_EXAM_PROGRESS_SUCCESS
```

---

## 📋 TASK 5: HIỂN THỊ TRẠNG THÁI "ĐANG LÀM DỞ" TRONG DANH SÁCH BÀI THI

### Mô tả:
- Trong danh sách bài thi của sinh viên:
  - Nếu có submission với status = "in_progress":
    - Hiển thị: "⏳ [Tên bài] - Đang làm (Còn lại: X phút Y giây)"
    - Đếm ngược thời gian real-time
    - Thay vì hiển thị điểm

### Files cần thay đổi:

#### Backend:
- Không cần thay đổi (đã có `get_submission_status()`)

#### Frontend:
1. **`frontend/classdetail.cpp`**
   - Trong `loadExamsInClass()`: Với mỗi exam, gọi `get_submission_status()`
   - Nếu status = "in_progress":
     - Hiển thị với icon ⏳ và đếm ngược
     - Tính `remaining_seconds` từ `started_at + time_limit - NOW()`
     - Tạo QTimer để update đếm ngược mỗi giây
   - Nếu status = "submitted":
     - Hiển thị điểm như Task 3

2. **`frontend/classdetail.h`**
   - Thêm QTimer* member: `examStatusTimer`
   - Thêm method: `void updateExamStatusDisplay()`

### Logic:
```
For each exam:
  Get submission_status()
  If status == "in_progress":
    Calculate remaining_time = time_limit - (NOW() - started_at)
    Display: "⏳ [Name] - Đang làm (Còn lại: X phút Y giây)"
    Start timer to update every second
  Else if status == "submitted":
    Display: "✅ [Name] - Điểm: X.X"
  Else:
    Display: "📝 [Name] - Làm bài"
```

---

## 📋 TASK 6: FIX PRACTICE MODE - NÚT QUAY LẠI

### Mô tả:
- Fix nút "Quay lại" trong PracticeMode không hoạt động
- Khi bấm "Quay lại" → quay về ClassDetail

### Files cần thay đổi:

#### Frontend:
1. **`frontend/practicemode.cpp`**
   - Kiểm tra signal `backToClassDetail` có được emit không
   - Kiểm tra connection trong mainwindow.cpp

2. **`frontend/practicemode.h`**
   - Đảm bảo có signal: `void backToClassDetail();`

3. **`frontend/mainwindow.cpp`**
   - Kiểm tra connection: `connect(practiceModeForm, &PracticeMode::backToClassDetail, ...)`
   - Đảm bảo gọi `showClassDetail(currentClassId)`

4. **`frontend/practicemode.ui`**
   - Kiểm tra button "Quay lại" có được connect đúng không

### Debug Steps:
1. Check signal/slot connection
2. Check button click handler
3. Check navigation logic

---

## TỔNG KẾT THỨ TỰ ƯU TIÊN:

1. **Task 1**: Thông báo khiếu nại (popup) - **ƯU TIÊN CAO**
2. **Task 2**: Giáo viên phản hồi khiếu nại - **ƯU TIÊN CAO**
3. **Task 3**: Không cho làm lại bài đã làm - **ƯU TIÊN TRUNG BÌNH**
4. **Task 4**: Tạm dừng làm bài - **ƯU TIÊN TRUNG BÌNH** (phức tạp)
5. **Task 5**: Hiển thị trạng thái "đang làm dở" - **ƯU TIÊN THẤP**
6. **Task 6**: Fix Practice Mode - **ƯU TIÊN THẤP** (bug fix)

---

## LƯU Ý:
- Mỗi task sẽ được implement riêng biệt sau khi được duyệt
- Có thể có dependencies giữa các task (ví dụ: Task 4 cần Task 3)
- Cần test kỹ từng task trước khi chuyển sang task tiếp theo



