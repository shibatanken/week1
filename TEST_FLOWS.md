# 🧪 TEST FLOWS - HƯỚNG DẪN CHI TIẾT

## Tài khoản đã tạo:
- **Admin**: admin@test.com / admin
- **Teacher**: teacher@test.com / teacher  
- **Student 1**: student1@test.com / student
- **Student 2**: student2@test.com / student

---

## ✅ FLOW 1: ĐĂNG KÝ & DUYỆT TÀI KHOẢN

### Bước 1: Đăng ký tài khoản mới
1. Mở frontend → Click **Sign Up**
2. Nhập:
   - Email: `newuser@test.com`
   - Password: `newuser`
   - Name: `New User`
   - DOB: `2000-01-01`
   - Role: Chọn **Student**
3. Click **Sign Up** → Thông báo "Đăng ký thành công, chờ admin duyệt"

### Bước 2: Kiểm tra database
```bash
wsl mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT id, email, name, role, status FROM user WHERE email='newuser@test.com';"
```
**Kết quả mong đợi**: status = 'pending'

### Bước 3: Admin duyệt tài khoản
1. Logout → Login bằng **admin@test.com / admin**
2. Click **Admin Dashboard**
3. Tab **Pending Users** → Thấy `newuser@test.com`
4. Click **Approve** → Thông báo thành công

### Bước 4: User login
1. Logout → Login bằng **newuser@test.com / newuser**
2. **Kết quả**: Login thành công, vào được home screen

---

## ✅ FLOW 2: TẠO LỚP HỌC & THÊM SINH VIÊN

### Bước 1: Teacher tạo lớp
1. Login bằng **teacher@test.com / teacher**
2. Click **Create Class**
3. Nhập:
   - Class Name: `Lập trình mạng 2026`
   - Description: `Lớp học lập trình socket`
4. Click **Create** → Thông báo thành công

### Bước 2: Kiểm tra lớp đã tạo
1. Vào **My Classes** → Thấy lớp `Lập trình mạng 2026`
2. Click vào lớp → Xem **Class Detail**

### Bước 3: Thêm sinh viên vào lớp
1. Trong Class Detail → Click **Manage Members**
2. Tab **Add Students** → Thấy danh sách students
3. Chọn **student1@test.com** và **student2@test.com**
4. Click **Add to Class** → Thông báo thành công

### Bước 4: Kiểm tra từ phía Student
1. Logout → Login bằng **student1@test.com / student**
2. Vào **My Classes** → Thấy lớp `Lập trình mạng 2026`
3. **Kết quả**: Student đã được thêm vào lớp

---

## ✅ FLOW 3: TẠO ĐỀ THI

### Bước 1: Teacher tạo đề thi
1. Login bằng **teacher@test.com / teacher**
2. Vào **My Classes** → Chọn lớp `Lập trình mạng 2026`
3. Click **Exam Management** → **Create Exam**
4. Nhập:
   - Exam Name: `Kiểm tra giữa kỳ`
   - Description: `Kiểm tra socket programming`
   - Time Limit: `30` phút
5. Click **Create** → Thông báo thành công

### Bước 2: Thêm câu hỏi vào đề thi
1. Click vào exam vừa tạo → **Manage Questions**
2. Click **Add Question**
3. Nhập câu hỏi:
   - Content: `TCP hoạt động ở tầng nào?`
   - Option A: `Vật lý`
   - Option B: `Liên kết`
   - Option C: `Mạng`
   - Option D: `Giao vận`
   - Correct: `D`
4. Click **Add** → Thêm 3-5 câu nữa

### Bước 3: Publish đề thi
1. Quay lại Exam Management
2. Chọn exam → Click **Update Status** → Chọn **Published**
3. Click **Start Exam** → Cho phép students tham gia

### Bước 4: Kiểm tra
```bash
wsl mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT exam_name, status FROM exam;"
```
**Kết quả mong đợi**: status = 'published' hoặc 'ongoing'

---

## ✅ FLOW 4: HỌC SINH THI

### Bước 1: Student join exam
1. Login bằng **student1@test.com / student**
2. Vào **My Classes** → Chọn lớp
3. Click **Available Exams** → Thấy `Kiểm tra giữa kỳ`
4. Click **Join Exam** → Thông báo "Joined successfully"

### Bước 2: Làm bài thi
1. Click **Start Exam** → Màn hình làm bài hiện ra
2. **Timer** bắt đầu đếm ngược từ 30 phút
3. Đọc câu hỏi → Chọn đáp án A/B/C/D
4. Click **Next** để chuyển câu
5. Có thể quay lại sửa đáp án

### Bước 3: Nộp bài
1. Click **Submit Exam** → Xác nhận
2. Hệ thống tự động chấm điểm
3. Màn hình kết quả hiện ra:
   - Điểm số: X/Y
   - Số câu đúng/sai
   - Thời gian làm bài

### Bước 4: Xem lại kết quả
1. Vào **My Exam History**
2. Click vào bài thi đã làm
3. Xem chi tiết: từng câu đúng/sai

---

## ✅ FLOW 5: LUYỆN TẬP

### Bước 1: Vào Practice Mode
1. Login bằng **student1@test.com / student**
2. Click **Practice Mode**
3. Chọn số câu hỏi: **10 câu**
4. Click **Start Practice**

### Bước 2: Làm bài luyện tập
1. Hệ thống random 10 câu từ Question Bank
2. Làm bài tương tự như thi
3. Không có timer, không giới hạn thời gian

### Bước 3: Xem kết quả
1. Click **Finish Practice**
2. Xem ngay đáp án đúng/sai
3. Điểm số: X/10
4. **Lưu ý**: Không lưu vào lịch sử thi chính thức

---

## ✅ FLOW 6: KHIẾU NẠI ĐIỂM

### Bước 1: Student gửi appeal
1. Login bằng **student1@test.com / student**
2. Click **Appeal Manager**
3. Chọn bài thi đã làm: `Kiểm tra giữa kỳ`
4. Chọn câu hỏi muốn khiếu nại
5. Nhập lý do: `Đáp án D cũng đúng vì...`
6. Click **Submit Appeal**

### Bước 2: Kiểm tra database
```bash
wsl mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT * FROM appeals WHERE status='pending';"
```

### Bước 3: Teacher xem xét
1. Login bằng **teacher@test.com / teacher**
2. Click **Appeal Manager** → Tab **Pending Appeals**
3. Thấy appeal từ student1
4. Click **Review** → Xem:
   - Câu hỏi
   - Đáp án student chọn
   - Lý do khiếu nại

### Bước 4: Teacher phản hồi
1. Nhập comment: `Đáp án của bạn sai vì...`
2. Chọn **Reject** hoặc **Approve**
3. Click **Submit** → Thông báo thành công

### Bước 5: Student xem kết quả
1. Login lại bằng student1
2. Vào Appeal Manager → Tab **My Appeals**
3. Thấy status: **Reviewed**
4. Đọc comment từ teacher

---

## ✅ FLOW 7: QUẢN TRỊ HỆ THỐNG

### Bước 1: Xem tất cả users
1. Login bằng **admin@test.com / admin**
2. Click **Admin Dashboard**
3. Tab **All Users** → Thấy danh sách đầy đủ

### Bước 2: Thêm user mới
1. Click **Add User**
2. Nhập thông tin đầy đủ
3. Chọn role và status
4. Click **Create**

### Bước 3: Sửa user
1. Chọn user → Click **Edit**
2. Đổi role: Student → Teacher
3. Click **Update**

### Bước 4: Xóa user
1. Chọn user → Click **Delete**
2. Xác nhận → User bị xóa

---

## ✅ FLOW 8: XEM THỐNG KÊ

### Bước 1: Teacher xem thống kê lớp
1. Login bằng **teacher@test.com / teacher**
2. Vào **My Classes** → Chọn lớp
3. Click **Statistics**

### Bước 2: Xem thống kê bài thi
1. Chọn exam: `Kiểm tra giữa kỳ`
2. Xem:
   - Điểm trung bình: X.XX
   - Điểm cao nhất: XX
   - Điểm thấp nhất: XX
   - Số students đã thi: X/Y

### Bước 3: Xem phân bố điểm
1. Biểu đồ phân bố (nếu có QtCharts):
   - 0-2 điểm: X students
   - 2-4 điểm: X students
   - ...
   - 8-10 điểm: X students

### Bước 4: Xem chi tiết từng student
1. Danh sách students và điểm
2. Click vào student → Xem chi tiết bài làm

---

## 🔍 KIỂM TRA DATABASE SAU MỖI FLOW

```bash
# Xem users
wsl mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT id, email, role, status FROM user;"

# Xem classes
wsl mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT * FROM class;"

# Xem students trong class
wsl mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT * FROM user_in_class;"

# Xem exams
wsl mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT id, exam_name, status FROM exam;"

# Xem submissions
wsl mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT * FROM exam_submissions;"

# Xem appeals
wsl mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT * FROM appeals;"
```

---

## ⚠️ LƯU Ý KHI TEST

1. **Luôn kiểm tra server log** để xem request/response
2. **Kiểm tra database** sau mỗi action quan trọng
3. **Test cả trường hợp lỗi**: sai password, không có quyền, etc.
4. **Test đồng thời**: 2 students cùng làm 1 bài thi
5. **Test timer**: Để hết giờ xem có tự động submit không

---

## 🐛 DEBUG

Nếu có lỗi, kiểm tra:
1. **Server log**: Xem terminal đang chạy server
2. **Frontend console**: Xem error messages
3. **Database**: Query trực tiếp để xem data
4. **Network**: Đảm bảo frontend kết nối được server (localhost:8081)

