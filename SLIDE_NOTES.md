# SLIDE NOTES - THUYẾT TRÌNH NHANH
## Chú Thích Cho Từng Slide

---

## SLIDE 1: TITLE
**Nói:**
"Xin chào thầy/cô. Nhóm em xin phép trình bày tiến độ dự án môn Lập trình mạng"

**Hiển thị:**
```
HỆ THỐNG CLIENT-SERVER
VỚI AUTHENTICATION

Backend: C + MySQL
Frontend: Qt C++
Protocol: TCP Socket
```

---

## SLIDE 2: KIẾN TRÚC TỔNG QUAN
**Nói:**
"Dự án được thiết kế theo mô hình Client-Server. Client viết bằng Qt C++ kết nối đến Server C qua TCP port 8081. Server tương tác với MySQL database."

**Hiển thị:**
```
[Qt Client] ←→ TCP 8081 ←→ [C Server] ←→ [MySQL DB]
```

**Key points:**
- Multi-threaded server
- Custom protocol
- MVC architecture

---

## SLIDE 3: BACKEND STRUCTURE
**Nói:**
"Backend được tổ chức theo kiến trúc MVC với routes xử lý connection, controllers xử lý request, services chứa business logic"

**Hiển thị:**
```
Routes (Threading) → Controllers → Services → Database
```

**Demo code:**
- `main.c`: Socket setup
- `routes.c`: Thread handling
- `auth_controller.c`: Request processing
- `auth_service.c`: DB operations

---

## SLIDE 4: SERVER FLOW
**Nói:**
"Server khởi tạo socket, bind port 8081, listen connections. Mỗi client kết nối được xử lý bởi thread riêng biệt sử dụng pthread"

**Hiển thị:**
```
1. socket() → create
2. bind() → port 8081
3. listen() → queue = 10
4. accept() → while loop
5. pthread_create() → new thread per client
```

---

## SLIDE 5: PROTOCOL DESIGN
**Nói:**
"Em thiết kế protocol với 3 loại message. CONTROL cho login/signup, DATA cho file transfer, NOTIFICATION cho response"

**Hiển thị:**
```
CONTROL LOGIN
{"email":"user@mail.com","password":"123"}

↓

NOTIFICATION LOGIN_SUCCESS 2024-12-01T10:30:00
{"user_id": 123}
```

**Format:**
- Header: Type + Info
- Body: JSON data
- Separator: \n

---

## SLIDE 6: AUTHENTICATION
**Nói:**
"Hiện tại em đã hoàn thành 2 chức năng: Đăng ký và đăng nhập. Cả 2 đều có logging vào database và file"

**Hiển thị:**
```
SIGNUP:
- Validate input
- Insert vào user table
- Log activity
- Return success/failure

LOGIN:
- Query user by email+password
- Return user_id if found
- Log activity
- Send notification
```

---

## SLIDE 7: FRONTEND STRUCTURE
**Nói:**
"Frontend sử dụng Qt với 2 forms chính: Signin và Signup. Dùng QTcpSocket để kết nối server"

**Hiển thị:**
```
MainWindow (QStackedWidget)
├── Signup Form
│   ├── Email, Password, Name, DOB
│   └── QTcpSocket → send CONTROL SIGN_UP
└── Signin Form
    ├── Email, Password
    └── QTcpSocket → send CONTROL LOGIN
```

---

## SLIDE 8: CLIENT-SERVER COMMUNICATION
**Nói:**
"Khi user click đăng nhập, client tạo JSON, kết nối đến server port 8081, gửi message theo protocol. Server nhận, parse, xử lý, query DB, rồi trả response"

**Hiển thị:**
```
CLIENT                    SERVER
  │                         │
  ├──→ Connect 8081         │
  │                         ├─ Accept
  │                         ├─ New Thread
  ├──→ CONTROL LOGIN        │
  │    {"email","pass"}     ├─ Parse
  │                         ├─ Query DB
  │                         ├─ Log
  ←──┤ NOTIFICATION         │
      LOGIN_SUCCESS         │
```

---

## SLIDE 9: DATABASE SCHEMA
**Nói:**
"Database có 2 bảng chính: user lưu thông tin người dùng, log lưu toàn bộ activity"

**Hiển thị:**
```sql
user:
- id (PK, AUTO_INCREMENT)
- email
- pass
- name
- dob

log:
- id (PK, AUTO_INCREMENT)
- log_content
- log_time
```

---

## SLIDE 10: DEMO PREPARATION
**Nói:**
"Bây giờ em xin phép demo. Em sẽ start server trước, sau đó mở client"

**Terminal commands:**
```bash
# Terminal 1
cd backend && make && ./server

# Terminal 2  
cd frontend && make && ./frontend.app/Contents/MacOS/frontend
```

**Check:**
- "Server listening on port 8081"
- GUI hiện lên

---

## SLIDE 11: DEMO SIGNUP
**Nói:**
"Em sẽ demo đăng ký tài khoản mới"

**Steps:**
1. Nhập email: demo@test.com
2. Nhập password: 123456
3. Nhập name: Demo User
4. Chọn DOB: 2000-01-01
5. Click "Đăng ký"
6. → Hiện "Đăng ký thành công"

**Kiểm tra:**
- Server log: "CONTROL SIGN_UP..."
- Database: `SELECT * FROM user WHERE email='demo@test.com'`
- File log: `cat backend/log.txt`

---

## SLIDE 12: DEMO LOGIN
**Nói:**
"Bây giờ em đăng nhập với tài khoản vừa tạo"

**Steps:**
1. Click "Đã có tài khoản"
2. Nhập email: demo@test.com
3. Nhập password: 123456
4. Click "Đăng nhập"
5. → Hiện "Đăng nhập thành công! User ID: X"

**Kiểm tra:**
- Server log: "CONTROL LOGIN..."
- Response label màu xanh

---

## SLIDE 13: DEMO ERROR CASE
**Nói:**
"Em sẽ demo trường hợp nhập sai password"

**Steps:**
1. Email: demo@test.com
2. Password: wrongpassword
3. Click "Đăng nhập"
4. → Hiện "Đăng nhập thất bại" màu đỏ

---

## SLIDE 14: CONCURRENT TEST
**Nói:**
"Em test khả năng xử lý đồng thời bằng cách mở 2 client"

**Steps:**
1. Mở client thứ 2
2. Đăng nhập ở cả 2 cùng lúc
3. → Cả 2 đều success

**Giải thích:**
"Đây là nhờ multi-threading với pthread. Mỗi client có thread riêng"

---

## SLIDE 15: TECHNICAL HIGHLIGHTS
**Nói:**
"Về mặt kỹ thuật, em đã áp dụng các kiến thức..."

**Liệt kê:**
✅ Socket Programming (socket, bind, listen, accept)
✅ Multi-threading (pthread, mutex)
✅ Protocol Design (custom message format)
✅ JSON Processing (cJSON, Qt JSON)
✅ Database Integration (MySQL C API)
✅ MVC Architecture
✅ Error Handling
✅ Logging System

---

## SLIDE 16: STRENGTHS
**Nói:**
"Điểm mạnh của dự án..."

**Liệt kê:**
✅ **Architecture:** Clean, maintainable, scalable
✅ **Concurrency:** Multi-threaded, thread-safe
✅ **Protocol:** Clear, extensible, JSON-based
✅ **Logging:** Comprehensive (DB + file)
✅ **UX:** User-friendly GUI, clear feedback

---

## SLIDE 17: LIMITATIONS
**Nói:**
"Em cũng nhận thức được một số hạn chế..."

**Liệt kê:**
⚠️ **Security:**
- Password chưa hash
- Chưa có SQL injection prevention
- Không có encryption

⚠️ **Scalability:**
- Thread-per-connection không tốt với nhiều users
- Single DB connection

⚠️ **Features:**
- Chưa có session management
- Chưa có password recovery

---

## SLIDE 18: FUTURE WORK
**Nói:**
"Kế hoạch phát triển tiếp theo..."

**Phase 2:**
□ Password hashing (bcrypt)
□ SQL injection prevention
□ Input validation

**Phase 3:**
□ Session tokens
□ Token-based auth
□ Logout function

**Phase 4:**
□ File transfer (DATA protocol)
□ Chat messaging
□ Profile management

**Phase 5:**
□ Event-driven architecture (epoll)
□ Connection pooling
□ Load balancing

---

## SLIDE 19: TEAM WORK
**Nói:**
"Về phân công công việc..."

**Hiển thị:**
```
Member 1: Backend socket, threading      ✅
Member 2: Database, auth service         ✅
Member 3: Frontend GUI, Qt               ✅
Member 4: Protocol, integration          ✅

Current: Testing, documentation          🔄
```

---

## SLIDE 20: LESSONS LEARNED
**Nói:**
"Qua dự án này, nhóm em học được rất nhiều..."

**Kiến thức:**
- Network programming ở tầng thấp
- Concurrent programming challenges
- Client-Server architecture design
- Protocol design decisions
- Database integration
- GUI event handling
- Debugging distributed systems

**Kỹ năng mềm:**
- Teamwork & collaboration
- Code organization
- Documentation
- Problem solving

---

## SLIDE 21: CONCLUSION
**Nói:**
"Tóm lại, đến thời điểm hiện tại nhóm em đã hoàn thành được:"

**Checklist:**
✅ Backend server với multi-threading
✅ Frontend client với Qt GUI
✅ Custom protocol hoàn chỉnh
✅ Authentication system (login/signup)
✅ Database integration
✅ Logging system
✅ Error handling

**Timeline:**
- Tuần tới: Security improvements
- 2 tuần tới: Session management & more features

---

## SLIDE 22: Q&A
**Nói:**
"Em xin cảm ơn thầy/cô đã theo dõi. Nhóm em rất mong nhận được góp ý!"

**Sẵn sàng trả lời:**
- Technical details
- Design decisions
- Future improvements
- Challenges faced

---

## CÂU TRẢLỜI SẴN CHO CÂU HỎI THƯỜNG GẶP

### "Tại sao tự thiết kế protocol?"
→ Để học network programming từ cơ bản, hiểu cách protocol hoạt động ở tầng thấp

### "Performance với nhiều users?"
→ Hiện tại thread-per-connection. Sẽ migrate sang epoll/select model

### "Security concerns?"
→ Đang implement bcrypt, prepared statements, input validation

### "Error recovery?"
→ Có logging đầy đủ. Sẽ thêm auto-reconnect, graceful shutdown

### "Scale strategy?"
→ Event-driven model, connection pooling, load balancing, message queue

---

## TIPS THUYẾT TRÌNH

✅ **Trước khi trình bày:**
- Test server + client trước 1 ngày
- Chuẩn bị video backup demo
- Check DB có data test sẵn
- Check port 8081 không bị chiếm

✅ **Trong lúc trình bày:**
- Nói chậm, rõ ràng
- Tự tin khi demo
- Nếu bug, giải thích calmly
- Show enthusiasm về project

✅ **Xử lý sự cố:**
- Server crash → restart, show logs
- Client không connect → check port, firewall
- Demo fails → show video backup

---

## CHECKLIST TRƯỚC GIỜ THUYẾT TRÌNH

□ Compile được backend (make clean && make)
□ Compile được frontend (make)
□ MySQL đang chạy
□ Port 8081 available
□ Test signup flow
□ Test login flow (success + failure)
□ Database có ít nhất 1 user test
□ Log file accessible
□ Slides chuẩn bị xong
□ Laptop đầy pin / có sạc
□ Backup code trên USB
□ Video demo backup
□ Script in ra giấy (just in case)

---

**GOOD LUCK! 🍀**


