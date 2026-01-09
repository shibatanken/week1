# 🧪 HƯỚNG DẪN TEST HỆ THỐNG

## 🚀 SETUP NHANH

### 1. Chuẩn bị (WSL/Linux)
```bash
# Start MySQL
sudo service mysql start

# Setup database
cd week1/backend
bash setup.sh

# Tạo tài khoản test
mysql -u quizz -pQuizz2003@ quizz_db -e "
INSERT INTO user (email, pass, name, dob, role, status) VALUES
('admin@test.com', 'admin', 'Admin', '1990-01-01', 'teacher', 'approved'),
('teacher@test.com', 'teacher', 'Teacher', '1990-01-01', 'teacher', 'approved'),
('student1@test.com', 'student', 'Student 1', '2000-01-01', 'student', 'approved'),
('student2@test.com', 'student', 'Student 2', '2000-01-01', 'student', 'approved');
"
```

### 2. Chạy Server
```bash
cd week1/backend
make clean && make
./server
```

### 3. Chạy Frontend
```bash
# Windows
cd week1/frontend
build.bat
cd release
frontend.exe

# Linux/Mac
cd week1/frontend
qmake && make
./frontend
```

---

## 📝 TEST CASES

### ✅ TEST 1: ĐĂNG NHẬP
**Mục đích:** Kiểm tra authentication

**Steps:**
1. Mở frontend
2. Login: `teacher@test.com` / `teacher`
3. **Expected:** Vào được home screen

**Verify:**
- ✓ Thông báo login thành công
- ✓ Hiển thị tên user
- ✓ Có menu My Classes, Create Class

---

### ✅ TEST 2: TẠO LỚP HỌC
**Mục đích:** Teacher tạo và quản lý lớp

**Steps:**
1. Login teacher
2. Click **Create Class**
3. Nhập: Name: `Test Class`, Description: `Test`
4. Click **Create**
5. Vào **My Classes**

**Expected:**
- ✓ Thông báo tạo thành công
- ✓ Thấy lớp mới trong danh sách

**Verify DB:**
```bash
mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT * FROM class WHERE class_name='Test Class';"
```

---

### ✅ TEST 3: THÊM CÂU HỎI VÀO NGÂN HÀNG
**Mục đích:** Teacher tạo câu hỏi để tái sử dụng

**Steps:**
1. Vào **My Classes** → Chọn lớp
2. Click **Question Bank**
3. Click **Add Question**
4. Nhập:
   - Content: `Test question?`
   - Options A-D: `Ans A`, `Ans B`, `Ans C`, `Ans D`
   - Correct: `A`
5. Click **Save**

**Expected:**
- ✓ Thông báo thêm thành công
- ✓ Câu hỏi xuất hiện trong list

**Verify DB:**
```bash
mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT * FROM questions WHERE content LIKE 'Test question%';"
```

---

### ✅ TEST 4: TẠO ĐỀ THI
**Mục đích:** Teacher tạo exam và thêm câu hỏi

**Steps:**
1. Vào Class Detail → **Create Exam**
2. Nhập:
   - Name: `Midterm Test`
   - Time: `30` phút
   - Description: `Test exam`
3. Click **Save Exam**
4. Click **Add Question** → Nhập câu hỏi mới
5. Click **Import from Bank** → Chọn câu hỏi → Select
6. Click **Publish Exam**

**Expected:**
- ✓ Exam được tạo
- ✓ Có cả câu hỏi mới và import từ bank
- ✓ Status = published

**Verify DB:**
```bash
mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT * FROM exam WHERE exam_name='Midterm Test';"
mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT COUNT(*) FROM exam_questions WHERE exam_id=X;"
```

---

### ✅ TEST 5: THÊM SINH VIÊN VÀO LỚP
**Mục đích:** Teacher quản lý thành viên lớp

**Steps:**
1. Vào Class Detail → **Manage Members**
2. Tab **Add Students**
3. Chọn `student1@test.com` và `student2@test.com`
4. Click **Add to Class**

**Expected:**
- ✓ Thông báo thêm thành công
- ✓ Students xuất hiện trong Members list

**Verify:**
- Logout → Login `student1@test.com` / `student`
- Vào **My Classes** → Thấy lớp vừa được thêm

---

### ✅ TEST 6: HỌC SINH LÀM BÀI THI
**Mục đích:** Test toàn bộ exam flow

**Steps:**
1. Login `student1@test.com` / `student`
2. Vào **My Classes** → Chọn lớp
3. Click **Available Exams** → Thấy `Midterm Test`
4. Click **Join Exam**
5. Click **Start Exam**
6. Chọn đáp án cho từng câu
7. Click **Submit Exam**

**Expected:**
- ✓ Timer đếm ngược
- ✓ Có thể chọn/đổi đáp án
- ✓ Submit thành công
- ✓ Hiển thị điểm ngay

**Verify DB:**
```bash
mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT * FROM exam_submissions WHERE user_id=X AND exam_id=Y;"
mysql -u quizz -pQuizz2003@ quizz_db -e "SELECT * FROM exam_answers WHERE submission_id=Z;"
```

---

### ✅ TEST 7: LUYỆN TẬP
**Mục đích:** Student làm bài practice

**Steps:**
1. Login student
2. Click **Practice Mode**
3. Chọn số câu: `5`
4. Click **Start Practice**
5. Làm bài → Submit

**Expected:**
- ✓ Random 5 câu từ question bank
- ✓ Không có timer
- ✓ Xem kết quả ngay
- ✓ Không lưu vào exam history

---

### ✅ TEST 8: KHIẾU NẠI ĐIỂM
**Mục đích:** Student appeal, Teacher review

**Steps - Student:**
1. Login student
2. Click **Appeal Manager**
3. Chọn bài thi đã làm
4. Chọn câu hỏi
5. Nhập lý do: `I think answer B is also correct`
6. Click **Submit Appeal**

**Steps - Teacher:**
1. Login teacher
2. Click **Appeal Manager**
3. Tab **Pending Appeals**
4. Click **Review** trên appeal
5. Nhập comment: `Your answer is incorrect because...`
6. Click **Reject** hoặc **Approve**

**Expected:**
- ✓ Student thấy appeal status = reviewed
- ✓ Student đọc được comment từ teacher

---

### ✅ TEST 9: ADMIN DUYỆT TÀI KHOẢN
**Mục đích:** Admin quản lý users

**Steps:**
1. Tạo user mới qua Sign Up: `newuser@test.com`
2. Login `admin@test.com` / `admin`
3. Click **Admin Dashboard**
4. Tab **Pending Users** → Thấy newuser
5. Click **Approve**
6. Logout → Login `newuser@test.com`

**Expected:**
- ✓ User ban đầu status = pending
- ✓ Sau approve status = approved
- ✓ User login được

---

### ✅ TEST 10: XEM THỐNG KÊ
**Mục đích:** Teacher xem báo cáo

**Steps:**
1. Login teacher
2. Vào Class Detail
3. Click **Statistics**
4. Chọn exam

**Expected:**
- ✓ Hiển thị điểm TB, cao nhất, thấp nhất
- ✓ Số students đã thi
- ✓ Danh sách kết quả

---

## 🐛 CHECKLIST LỖI THƯỜNG GẶP

### Backend không chạy
- [ ] MySQL đã start chưa? `sudo service mysql start`
- [ ] Port 8081 bị chiếm? `netstat -an | grep 8081`
- [ ] Có file server? `ls -la week1/backend/server`

### Frontend không kết nối
- [ ] Server đang chạy? `ps aux | grep server`
- [ ] IP đúng chưa? Check `frontend/config.h`
- [ ] Firewall block? Thử `telnet localhost 8081`

### Database lỗi
- [ ] User/password đúng? Test: `mysql -u quizz -pQuizz2003@ quizz_db`
- [ ] Tables đã tạo? `SHOW TABLES;`
- [ ] Foreign key lỗi? Check constraints

### UI không hiển thị data
- [ ] Check server log xem có request không
- [ ] Response có lỗi? Xem console
- [ ] Reload lại trang/logout-login

---

## 📊 REPORT BUG

Khi tìm thấy bug, ghi lại:
1. **Steps to reproduce** (các bước tái hiện)
2. **Expected result** (kết quả mong đợi)
3. **Actual result** (kết quả thực tế)
4. **Server log** (nếu có)
5. **Screenshot** (nếu cần)

Example:
```
BUG: Không submit được bài thi

Steps:
1. Login student
2. Join exam
3. Chọn đáp án
4. Click Submit Exam

Expected: Hiển thị điểm
Actual: Thông báo lỗi "Failed to submit"

Server log: "Error: exam_id not found"
```

---

## ✅ ACCEPTANCE CRITERIA

Hệ thống PASS khi:
- [x] 10/10 test cases pass
- [x] Không có lỗi critical
- [x] UI responsive và user-friendly
- [x] Data consistency (DB đúng sau mỗi action)
- [x] Server log không có error

