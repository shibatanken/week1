# QUẢN LÝ LỚP HỌC - WEEK 1

## 🚀 HƯỚNG DẪN NHANH

### Setup Database:
```bash
mysql -u root -p << EOF
CREATE DATABASE quizz_db;
CREATE USER 'quizz'@'localhost' IDENTIFIED BY 'Quizz2003@';
GRANT ALL PRIVILEGES ON quizz_db.* TO 'quizz'@'localhost';
EOF

cd week1/backend
make migrate
mysql -u quizz -pQuizz2003@ quizz_db < src/seeder/demo_data.sql
```

### Build & Run:
```bash
# Backend
cd week1/backend
make clean && make
./server

# Frontend (terminal mới)
cd week1/frontend
qmake frontend.pro && make
./frontend.app/Contents/MacOS/frontend  # macOS
# hoặc ./frontend                        # Linux
```

### Test Accounts:
- Teacher: `teacher1@test.com` / `123456`
- Student: `student1@test.com` / `123456`

---

## 📋 TÍNH NĂNG

### Teacher có thể:
- Tạo lớp học
- Thêm sinh viên vào lớp
- Xóa sinh viên khỏi lớp
- Xem danh sách lớp và sinh viên

### Student có thể:
- Xem danh sách lớp
- Xem chi tiết lớp (read-only)

---

## 🏗️ KIẾN TRÚC & CÁCH HOẠT ĐỘNG

### Database Schema:
```
user                    class                user_in_class
├── id                 ├── id               ├── id
├── email              ├── class_name       ├── user_id (FK)
├── pass               ├── description      ├── class_id (FK)
├── name               ├── teacher_id (FK)  └── joined_at
├── dob                └── created_at
└── role (NEW)
```

### Backend APIs (8 endpoints mới):
```
CREATE_CLASS              - Tạo lớp (teacher only)
GET_CLASS_LIST            - Lấy danh sách lớp
GET_CLASS_DETAIL          - Chi tiết lớp
ADD_STUDENT_TO_CLASS      - Thêm sinh viên (teacher only)
REMOVE_STUDENT_FROM_CLASS - Xóa sinh viên (teacher only)
GET_STUDENTS_IN_CLASS     - DS sinh viên trong lớp
GET_STUDENTS_NOT_IN_CLASS - DS sinh viên chưa join
DELETE_CLASS              - Xóa lớp (teacher only)
```

### Flow hoạt động:

#### 1. Login với role:
```
Client → CONTROL LOGIN {email, password}
Server → Query: SELECT id, role FROM user WHERE email=? AND pass=?
Server → NOTIFICATION LOGIN_SUCCESS {user_id, role}
Client → Lưu role vào UserData
```

#### 2. Teacher tạo lớp:
```
Client → CONTROL CREATE_CLASS {class_name, description, teacher_id}
Server → INSERT INTO class (...)
Server → Log activity
Server → NOTIFICATION CREATE_CLASS_SUCCESS
Client → Refresh class list
```

#### 3. Teacher thêm sinh viên:
```
Client → CONTROL ADD_STUDENT_TO_CLASS {user_id, class_id}
Server → INSERT INTO user_in_class (user_id, class_id)
Server → Log activity
Server → NOTIFICATION ADD_STUDENT_SUCCESS
Client → Refresh student list
```

#### 4. Student xem lớp (read-only):
```
Client → Check role == "student"
Client → Hide buttons "Tạo", "Thêm", "Xóa"
Client → CONTROL GET_CLASS_LIST
Server → SELECT * FROM class JOIN user_in_class...
Server → DATA JSON GET_CLASS_LIST {classes}
Client → Display cards (no edit buttons)
```

### Code Structure:

#### Backend:
```
routes.c
  └─> handle_control_message()
       ├─> LOGIN → auth_controller → auth_service → DB
       ├─> CREATE_CLASS → class_controller → class_service → DB
       ├─> ADD_STUDENT → class_controller → class_service → DB
       └─> ...
```

#### Frontend:
```
MainWindow (QStackedWidget)
  ├─> Signin → Login → Parse role → Store UserData
  ├─> ClassList
  │     ├─> if (isTeacher()) → show "Tạo lớp" button
  │     └─> GET_CLASS_LIST → Display cards
  └─> ClassDetail
        ├─> GET_CLASS_DETAIL
        ├─> GET_STUDENTS_IN_CLASS
        ├─> if (isTeacher()) → show "Thêm"/"Xóa" buttons
        └─> ADD/REMOVE_STUDENT_TO_CLASS
```

### Phân quyền:
```cpp
// Frontend check
if (UserData::instance().isTeacher()) {
    // Show teacher UI
} else {
    // Show student UI (read-only)
}

// Backend check (TODO)
// Hiện tại chưa verify role trên server
```

---

## 📁 FILES THÊM/SỬA

### Backend:
```
NEW:
  src/controllers/class/class_controller.c  (~250 lines)
  src/services/class/class_service.c        (~300 lines)
  src/migrations/create_02_class_table.sql
  src/migrations/create_03_user_in_class_table.sql
  src/seeder/demo_data.sql

UPDATED:
  src/data_structures/control_message.h     (+ 8 message types)
  src/routes/routes.c                       (+ class routing)
  src/services/auth/auth_service.c          (+ role in login)
  src/migrations/create_01_user_table.sql   (+ role column)
```

### Frontend:
```
NEW:
  classlist.cpp / classlist.h               (~200 lines)
  classdetail.cpp / classdetail.h           (~250 lines)

UPDATED:
  userdata.h                                (+ getRole(), isTeacher())
```

---

## ⚠️ KNOWN ISSUES

- ❌ Backend không verify role (cần thêm middleware)
- ❌ SQL injection risk (dùng string concatenation)
- ❌ Password plaintext (chưa hash)

---

## 🧪 TEST SCENARIOS

### Test 1: Teacher tạo lớp
```
1. Login: teacher1@test.com
2. Click "Tạo lớp mới"
3. Nhập tên lớp → Success
```

### Test 2: Teacher thêm sinh viên
```
1. Click "Xem chi tiết" lớp
2. Chọn sinh viên từ dropdown
3. Click "Thêm" → Sinh viên xuất hiện
```

### Test 3: Teacher xóa sinh viên
```
1. Click "Xóa" bên cạnh sinh viên
2. Confirm → Sinh viên biến mất
```

### Test 4: Student read-only
```
1. Login: student1@test.com
2. Xem lớp → Không có buttons "Thêm"/"Xóa"
```

---

**Tổng code:** ~1400 lines  
**APIs:** 8 endpoints mới  
**Tables:** 2 tables mới

