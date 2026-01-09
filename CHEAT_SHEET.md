# CHEAT SHEET - THUYẾT TRÌNH LẬP TRÌNH MẠNG
## Mang theo bên mình khi thuyết trình

---

## 📊 THÔNG TIN CƠ BẢN

**Tên dự án:** Hệ thống Client-Server Authentication

**Tech Stack:**
- Backend: C + MySQL
- Frontend: Qt C++
- Protocol: TCP Socket (Port 8081)
- Architecture: Multi-threaded MVC

**Chức năng hoàn thành:**
✅ Đăng ký (Signup)
✅ Đăng nhập (Login)
✅ Logging system
✅ Multi-threading

---

## 🎯 CẤU TRÚC 3 PHÚT NHANH

### Phút 1: Giới thiệu & Kiến trúc
```
"Dự án Client-Server với authentication.
Backend C multi-threaded, Frontend Qt, 
TCP socket port 8081, MySQL database"
```

### Phút 2: Demo
```
1. Start server → "Server listening on port 8081"
2. Start client → Show GUI
3. Signup → Success
4. Login → Success với user_id
5. Login sai pass → Failure message
```

### Phút 3: Technical & Kết luận
```
"Áp dụng: Socket, pthread, JSON, MySQL C API
Điểm mạnh: Clean architecture, concurrent, logging
Kế hoạch: Security, session, file transfer"
```

---

## 💻 LỆNH DEMO

### Terminal 1: Server
```bash
cd backend
make clean && make
./server
```
**Expected output:**
```
Connect to db successful
Server listening on port 8081
```

### Terminal 2: Client
```bash
cd frontend
make
./frontend.app/Contents/MacOS/frontend
```

### Terminal 3: Check Database
```bash
mysql -u quizz -p
# Password: Quizz2003@

USE quizz_db;
SELECT * FROM user;
SELECT * FROM log ORDER BY id DESC LIMIT 5;
```

### Check Log File
```bash
cat backend/log.txt
tail -f backend/log.txt  # real-time
```

---

## 🧪 TEST CASES

### Test 1: Signup Success ✅
```
Email: test@demo.com
Password: 123456
Name: Test User
DOB: 2000-01-01
→ "Đăng ký thành công"
```

### Test 2: Login Success ✅
```
Email: test@demo.com
Password: 123456
→ "Đăng nhập thành công! User ID: X"
→ Label màu xanh
```

### Test 3: Login Failure ❌
```
Email: test@demo.com
Password: wrongpass
→ "Đăng nhập thất bại"
→ Label màu đỏ
```

### Test 4: Concurrent ⚡
```
Open 2 clients
Login both simultaneously
→ Both work (thanks to pthread)
```

---

## 📡 PROTOCOL FORMAT

### Client → Server (Login)
```
CONTROL LOGIN
{"email":"user@mail.com","password":"123"}
```

### Server → Client (Success)
```
NOTIFICATION LOGIN_SUCCESS 2024-12-01T10:30:00
{"user_id": 123}
```

### Server → Client (Failure)
```
NOTIFICATION LOGIN_FAILURE 2024-12-01T10:30:00
{"message": "User not found or wrong password"}
```

---

## 🏗️ KIẾN TRÚC

```
┌──────────┐         ┌──────────┐         ┌──────────┐
│  CLIENT  │◄───────►│  SERVER  │◄───────►│  MYSQL   │
│  Qt C++  │  TCP    │    C     │  Query  │ Database │
│          │  8081   │ pthread  │         │          │
└──────────┘         └──────────┘         └──────────┘

CLIENT                    SERVER
  │                         │
  ├──→ connect(8081)        │
  │                         ├─ accept()
  │                         ├─ pthread_create()
  ├──→ write("CONTROL...")  │
  │                         ├─ read()
  │                         ├─ parse header/body
  │                         ├─ route to handler
  │                         ├─ call service
  │                         ├─ mysql_query()
  │                         ├─ log activity
  ←──┤ read()               │
      "NOTIFICATION..."     ├─ write()
  │                         ├─ close()
  │                         └─ thread exit
```

---

## 💪 ĐIỂM MẠNH

✅ **Clean Architecture:**
- MVC pattern
- Separation of concerns
- Easy to maintain

✅ **Concurrency:**
- Multi-threaded với pthread
- Thread-safe với mutex
- Xử lý nhiều clients đồng thời

✅ **Protocol:**
- Clear message format
- JSON body → flexible
- Extensible (3 types: CONTROL, DATA, NOTIFICATION)

✅ **Logging:**
- Database logging
- File logging
- Timestamp mọi operation

✅ **Error Handling:**
- Check return values
- Proper error messages
- User-friendly feedback

---

## ⚠️ HẠN CHẾ & HƯỚNG PHÁT TRIỂN

### Hạn chế hiện tại:
```
❌ Password plaintext (chưa hash)
❌ SQL injection risk (chưa prepare statement)
❌ Không có encryption (plaintext TCP)
❌ Thread-per-connection (không scale)
❌ Single DB connection
❌ Chưa có session management
```

### Sẽ làm tiếp:
```
📅 TUẦN TỚI:
□ bcrypt password hashing
□ Prepared statements
□ Input validation

📅 2 TUẦN TỚI:
□ Session tokens
□ Token-based auth
□ Profile management

📅 1 THÁNG TỚI:
□ File transfer (DATA protocol)
□ Real-time chat
□ Event-driven architecture (epoll)
```

---

## 🎓 KIẾN THỨC ĐÃ ÁP DỤNG

### Network Programming:
```c
socket()    // Tạo socket
bind()      // Bind port 8081
listen()    // Listen connections
accept()    // Accept client
connect()   // Client connect
read()      // Đọc data
write()     // Gửi data
close()     // Đóng connection
```

### Multi-threading:
```c
pthread_create()     // Tạo thread
pthread_mutex_init() // Init mutex
pthread_mutex_lock() // Lock critical section
pthread_mutex_unlock() // Unlock
pthread_attr_setdetachstate() // Detached thread
```

### Database:
```c
mysql_init()         // Init connection
mysql_real_connect() // Connect to DB
mysql_query()        // Execute query
mysql_store_result() // Get result
mysql_fetch_row()    // Fetch row
mysql_free_result()  // Free memory
mysql_close()        // Close connection
```

### Qt Network:
```cpp
QTcpSocket           // TCP client
connectToHost()      // Connect to server
write()              // Send data
readyRead signal     // Data available
readAll()            // Read data
```

---

## ❓ CÂU HỎI THƯỜNG GẶP & TRẢ LỜI

### Q1: "Tại sao không dùng HTTP mà tự thiết kế protocol?"
**A:** "Em muốn học network programming từ cơ bản, hiểu cách protocol hoạt động ở tầng socket. Đây là bài tập lập trình mạng nên em implement raw TCP socket. Protocol tùy chỉnh cũng giúp em linh hoạt mở rộng về sau, như binary protocol cho file transfer."

### Q2: "Xử lý 1000 concurrent users thế nào?"
**A:** "Hiện tại thread-per-connection sẽ không scale tốt với nhiều users. Trong phase tiếp theo em sẽ migrate sang event-driven architecture sử dụng epoll hoặc select, implement connection pooling, và có thể dùng load balancing với multiple server instances."

### Q3: "Password lưu plaintext có an toàn không?"
**A:** "Em nhận thức đây là security vulnerability lớn. Tuần tới em sẽ implement bcrypt hashing với salt. Hiện tại em focus vào việc làm cho hệ thống hoạt động trước, sau đó sẽ tăng cường security."

### Q4: "SQL injection thì sao?"
**A:** "Em đang dùng string concatenation nên có risk. Sẽ chuyển sang prepared statements ngay. Thêm input validation và sanitization ở cả client và server side."

### Q5: "Thread-safe như thế nào?"
**A:** "Em dùng mutex lock để protect critical sections, đặc biệt là khi multiple threads access database connection. Mỗi client có thread riêng, detached state nên tự cleanup khi xong."

### Q6: "Tại sao dùng JSON?"
**A:** "JSON human-readable, dễ debug, và Qt có built-in support. Tuy performance không bằng binary nhưng với traffic nhỏ thì chấp nhận được. Về sau có thể optimize bằng Protocol Buffers hoặc MessagePack."

### Q7: "Server crash thì client sao?"
**A:** "Client sẽ nhận disconnect error. Em có comprehensive logging để debug. Kế hoạch thêm auto-reconnect logic với exponential backoff, graceful shutdown với signal handlers, và health check endpoints."

### Q8: "Database connection pool?"
**A:** "Hiện tại dùng single connection vì simple. Với nhiều concurrent requests thì nên implement connection pool. Có thể dùng thư viện như MySQL++ hoặc tự implement pool với queue."

---

## 🚨 TROUBLESHOOTING

### Server không start:
```bash
# Port bị chiếm
netstat -tlnp | grep 8081
kill -9 <PID>

# MySQL không chạy
sudo service mysql start
sudo service mysql status
```

### Client không connect:
```bash
# Check server running
ps aux | grep server

# Test connection
telnet localhost 8081
nc localhost 8081

# Check firewall
sudo ufw status
sudo ufw allow 8081
```

### Compilation errors:
```bash
# Backend
cd backend
make clean
make

# Frontend
cd frontend
make clean
qmake && make
```

### Database errors:
```sql
-- Check user
SELECT User, Host FROM mysql.user WHERE User='quizz';

-- Check permissions
SHOW GRANTS FOR 'quizz'@'localhost';

-- Grant all if needed
GRANT ALL PRIVILEGES ON quizz_db.* TO 'quizz'@'localhost';
FLUSH PRIVILEGES;
```

---

## 📋 CHECKLIST TRƯỚC GIỜ G

### 1 Ngày trước:
□ Test compile backend
□ Test compile frontend
□ Test MySQL connection
□ Test full flow: signup → login
□ Chuẩn bị slides
□ Record video backup demo

### 1 Giờ trước:
□ Laptop đầy pin / mang sạc
□ MySQL đang chạy
□ Port 8081 available
□ Test data có trong DB
□ Code backup trên USB
□ Script in ra giấy
□ Nước uống

### 5 Phút trước:
□ Khởi động server
□ Test 1 lần login/signup
□ Mở slides
□ Mở terminal sẵn
□ Hít thở sâu, tự tin! 💪

---

## 🎤 OPENING & CLOSING

### Opening (30s):
```
"Xin chào thầy/cô và các bạn.

Hôm nay nhóm em xin được trình bày tiến độ dự án 
môn Lập trình mạng.

Đề tài: Hệ thống Client-Server với chức năng 
xác thực người dùng.

Công nghệ: Backend C với MySQL, Frontend Qt C++, 
giao thức TCP Socket, kiến trúc multi-threaded.

Em xin phép bắt đầu."
```

### Closing (30s):
```
"Tóm lại, đến thời điểm hiện tại nhóm em đã hoàn thành:

✅ Backend server với multi-threading
✅ Frontend client với Qt GUI  
✅ Custom protocol hoàn chỉnh
✅ Authentication system
✅ Database integration
✅ Logging system

Kế hoạch tiếp theo em sẽ tăng cường security 
với password hashing, implement session management, 
và phát triển thêm các features như file transfer.

Em xin cảm ơn thầy/cô đã theo dõi. 
Nhóm em rất mong nhận được góp ý để hoàn thiện 
dự án tốt hơn!

Em xin phép kết thúc phần trình bày. 
Cảm ơn thầy/cô!"
```

---

## 🎯 KEY NUMBERS ĐỂ NHỚ

```
8081    → Port server
10      → Backlog queue
2048    → Buffer size
3       → Message types (CONTROL, DATA, NOTIFICATION)
2       → Auth functions (login, signup)
2       → DB tables (user, log)
✅ ✅   → Signup + Login done
🔄 🔄   → Session + File transfer in progress
```

---

## 💡 TIP CUỐI CÙNG

1. **Nói chậm, rõ ràng** - Better slow & clear than fast & confusing
2. **Tự tin** - You built this, you understand it!
3. **Demo fails? No panic** - Show logs, explain what should happen, use backup video
4. **Interaction** - Nhìn audience, not just slides
5. **Time management** - 15 phút, practice beforehand
6. **Enthusiasm** - Show you're proud of your work!
7. **Smile** 😊 - It helps!

---

## 🎬 THỜI GIAN TIMELINE

```
00:00 - 00:30  Giới thiệu
00:30 - 02:00  Kiến trúc & Backend
02:00 - 03:00  Frontend & Protocol
03:00 - 05:00  DEMO
05:00 - 07:00  Technical deep dive
07:00 - 08:00  Điểm mạnh & hạn chế
08:00 - 09:00  Hướng phát triển
09:00 - 10:00  Kết luận
10:00 - 15:00  Q&A
```

---

**YOU GOT THIS! 💪🔥**

*In ra giấy, mang theo, đọc trước giờ thuyết trình!*


