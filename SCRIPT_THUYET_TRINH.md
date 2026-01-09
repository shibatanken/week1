# SCRIPT THUYẾT TRÌNH - DỰ ÁN LẬP TRÌNH MẠNG
## Hệ Thống Client-Server với Authentication

---

## 1. GIỚI THIỆU (30 giây)

Xin chào thầy/cô và các bạn. Hôm nay nhóm em xin được trình bày về tiến độ dự án môn Lập trình mạng.

**Tên dự án:** Hệ thống Client-Server với chức năng xác thực người dùng

**Công nghệ sử dụng:**
- Backend: Ngôn ngữ C với MySQL Database
- Frontend: Qt Framework (C++)
- Giao thức: TCP Socket
- Kiến trúc: Client-Server với Multi-threading

---

## 2. TỔNG QUAN KIẾN TRÚC HỆ THỐNG (1 phút)

### 2.1. Mô hình tổng thể
Dự án được thiết kế theo mô hình Client-Server cơ bản:

```
┌─────────────┐                    ┌─────────────┐
│   CLIENT    │  ←─── TCP/IP ───→  │   SERVER    │
│  (Qt C++)   │     Socket 8081    │   (C)       │
└─────────────┘                    └──────┬──────┘
                                          │
                                    ┌─────▼──────┐
                                    │   MySQL    │
                                    │  Database  │
                                    └────────────┘
```

### 2.2. Đặc điểm nổi bật
1. **Server Multi-threaded:** Hỗ trợ xử lý đồng thời nhiều client sử dụng pthread
2. **Giao thức tùy chỉnh:** Định dạng message rõ ràng (CONTROL, DATA, NOTIFICATION)
3. **Kiến trúc MVC:** Backend tổ chức theo mô hình Controller-Service-Model
4. **Logging system:** Ghi lại toàn bộ hoạt động của hệ thống

---

## 3. BACKEND - SERVER SIDE (3 phút)

### 3.1. Cấu trúc thư mục Backend

```
backend/
├── src/
│   ├── main.c                 # Entry point
│   ├── routes/                # Routing & Threading
│   ├── controllers/           # Xử lý request
│   │   └── auth/             # Authentication controller
│   ├── services/              # Business logic
│   │   └── auth/             # Authentication service
│   ├── db/                    # Database connection
│   ├── data_structures/       # Protocol definitions
│   └── utils/                 # Helper functions
```

### 3.2. Luồng hoạt động của Server

**Bước 1: Khởi tạo (main.c)**
```c
// Kết nối database
connect_db();

// Tạo socket
server_fd = socket(AF_INET, SOCK_STREAM, 0);

// Thiết lập socket option (SO_REUSEADDR)
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

// Setup routes và bắt đầu listen
setup_routes(server_fd);
```

**Bước 2: Listen và Accept connections (routes.c)**
```c
// Bind socket với PORT 8081
bind(server_fd, (struct sockaddr *)&address, sizeof(address));

// Listen với BACKLOG = 10
listen(server_fd, BACKLOG);

// Vòng lặp chấp nhận kết nối
while (1) {
    new_socket = accept(server_fd, ...);
    
    // Tạo thread mới cho mỗi client
    pthread_create(&pthread, &pthread_attr, pthread_routine, pthread_arg);
}
```

**Bước 3: Xử lý request trong thread riêng biệt**
```c
void *pthread_routine(void *arg) {
    // Đọc dữ liệu từ client
    read(new_socket_fd, buffer, 2048);
    
    // Parse header và body
    char *header = strtok(buffer, "\n");
    char *body = header + strlen(header) + 1;
    
    // Route đến handler tương ứng
    if (strncmp(header, "CONTROL", 7) == 0) {
        handle_control_message(socket, &msg);
    }
    
    return NULL;
}
```

### 3.3. Kiến trúc MVC Backend

**Controller Layer (auth_controller.c):**
- Nhận request từ client
- Parse JSON data
- Gọi service layer
- Trả response về client

**Service Layer (auth_service.c):**
- Thực hiện business logic
- Tương tác với database
- Logging hoạt động

**Database Layer:**
- MySQL connection pool
- Query execution
- Transaction handling

### 3.4. Chức năng Authentication đã hoàn thiện

#### A. ĐĂNG KÝ (Sign Up)
```c
int signup(const char *email, const char *password, 
           const char *username, const char *dob) {
    // Insert user vào database
    INSERT INTO user (email, pass, name, dob) VALUES (...)
    
    // Log activity vào database
    INSERT INTO log (log_content, log_time) VALUES (...)
    
    // Log ra file log.txt
    log_to_file(log_message, timestamp);
    
    return 1; // Success
}
```

**Đặc điểm:**
- Validate email & password
- Hash password (có thể bổ sung)
- Lưu thông tin người dùng
- Ghi log hoạt động

#### B. ĐĂNG NHẬP (Login)
```c
int login(const char *email, const char *password) {
    // Query user từ database
    SELECT * FROM user WHERE email='...' AND pass='...'
    
    // Kiểm tra kết quả
    if (num_rows > 0) {
        user_id = atoi(row[0]);
        // Log activity
        return user_id;
    }
    
    return -1; // Failure
}
```

**Đặc điểm:**
- Xác thực thông tin
- Trả về user_id nếu thành công
- Ghi log mỗi lần đăng nhập

### 3.5. Hệ thống Logging
Dự án có 2 cơ chế logging:

1. **Database logging:** Lưu vào bảng `log`
2. **File logging:** Ghi ra file `log.txt`

```c
void log_to_file(const char *message, const char *timestamp) {
    FILE *file = fopen("log.txt", "a");
    fprintf(file, "[%s] %s\n", timestamp, message);
    fclose(file);
}
```

---

## 4. FRONTEND - CLIENT SIDE (2 phút)

### 4.1. Công nghệ Frontend
- **Framework:** Qt 5/6 (C++)
- **UI Design:** Qt Designer (.ui files)
- **Network:** QTcpSocket

### 4.2. Cấu trúc Frontend

```
frontend/
├── main.cpp              # Entry point
├── mainwindow.cpp/h      # Main window với stacked widget
├── signin.cpp/h          # Form đăng nhập
├── signup.cpp/h          # Form đăng ký
├── userdata.h            # Singleton lưu thông tin user
├── config.h              # Cấu hình (IP, Port)
└── *.ui                  # UI files từ Qt Designer
```

### 4.3. Giao diện người dùng

**A. Form Đăng Ký (Signup)**
```cpp
Components:
- Email Input (QLineEdit)
- Password Input (QLineEdit)
- Username Input (QLineEdit)
- Date of Birth (QDateTimeEdit)
- Signup Button (QPushButton)
- Link chuyển sang Signin
```

**B. Form Đăng Nhập (Signin)**
```cpp
Components:
- Email Input (QLineEdit)
- Password Input (QLineEdit)
- Signin Button (QPushButton)
- Response Label (hiển thị kết quả)
- Link chuyển sang Signup
```

### 4.4. Xử lý kết nối TCP

**Kết nối đến server:**
```cpp
void on_signinButton_clicked() {
    // Tạo JSON object
    QJsonObject json;
    json["email"] = ui->emailLineEdit->text();
    json["password"] = ui->passwordLineEdit->text();
    
    // Convert sang JSON string
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    
    // Tạo message theo protocol
    QString dataString = QString("CONTROL LOGIN\n%1").arg(QString(jsonData));
    
    // Kết nối và gửi
    tcpSocket->connectToHost(IPADDRESS, 8081);
    if (tcpSocket->waitForConnected()) {
        tcpSocket->write(dataString.toUtf8());
        tcpSocket->flush();
    }
}
```

**Nhận response từ server:**
```cpp
void onReadyRead() {
    QByteArray response = tcpSocket->readAll();
    QString responseString(response);
    
    if (responseString.startsWith("NOTIFICATION LOGIN_SUCCESS")) {
        // Parse JSON để lấy user_id
        QJsonDocument jsonDoc = QJsonDocument::fromJson(...);
        int userId = jsonObj["user_id"].toInt();
        
        // Lưu thông tin user
        UserData::instance().setUserData(jsonObj);
        
        // Hiển thị thông báo
        ui->responseLabel->setText("Đăng nhập thành công!");
        
        // Chuyển màn hình
        emit showHome();
    }
}
```

---

## 5. GIAO THỨC TRUYỀN THÔNG (2 phút)

### 5.1. Định dạng Message

Dự án sử dụng giao thức tùy chỉnh với 3 loại message:

#### A. CONTROL MESSAGE
**Format:**
```
CONTROL <TYPE>
<JSON_BODY>
```

**Example - Login Request:**
```
CONTROL LOGIN
{"email":"user@example.com","password":"123456"}
```

**Example - Signup Request:**
```
CONTROL SIGN_UP
{"email":"user@example.com","password":"123456","name":"John","dob":"2000-01-01"}
```

#### B. NOTIFICATION MESSAGE
**Format:**
```
NOTIFICATION <TYPE> <TIMESTAMP>
<JSON_BODY>
```

**Example - Login Success:**
```
NOTIFICATION LOGIN_SUCCESS 2024-12-01T10:30:00
{"user_id": 123}
```

**Example - Login Failure:**
```
NOTIFICATION LOGIN_FAILURE 2024-12-01T10:30:00
{"message": "User not found or wrong password"}
```

#### C. DATA MESSAGE (Dự kiến cho tương lai)
**Format:**
```
DATA <TYPE> <DATA_TYPE> <SIZE>
<BODY>
```

**Struct definitions:**
```c
// Control Message
typedef struct {
    char type[50];      // LOGIN, SIGN_UP
    char body[1024];    // JSON data
} ControlMessage;

// Notification Message
typedef struct {
    char type[50];      // SUCCESS, FAILURE
    char timestamp[50]; // ISO 8601 format
    char body[1024];    // JSON data
} NotificationMessage;

// Data Message (for future features)
typedef struct {
    char type[50];
    char data_type[50];
    int data_size;
    char body[2048];
} DataMessage;
```

### 5.2. Luồng giao tiếp hoàn chỉnh

**Ví dụ: Quy trình Login**

```
CLIENT                                SERVER
  │                                     │
  │  1. Nhập email & password           │
  │     ↓                               │
  │  2. Tạo JSON                        │
  │     ↓                               │
  │  3. Kết nối TCP Socket              │
  ├─────────────────────────────────────→
  │  CONNECT to 8081                    │
  │                                     │
  │  4. Gửi CONTROL LOGIN               │
  ├─────────────────────────────────────→
  │  CONTROL LOGIN                      │
  │  {"email":"...","password":"..."}   │
  │                                     │
  │                                     ├─ 5. Accept connection
  │                                     │
  │                                     ├─ 6. Create new thread
  │                                     │
  │                                     ├─ 7. Parse message
  │                                     │
  │                                     ├─ 8. Route to handler
  │                                     │
  │                                     ├─ 9. Call service layer
  │                                     │
  │                                     ├─ 10. Query database
  │                                     │     SELECT * FROM user...
  │                                     │
  │                                     ├─ 11. Log activity
  │                                     │
  │  12. Nhận response                  │
  ←─────────────────────────────────────┤
  │  NOTIFICATION LOGIN_SUCCESS         │
  │  {"user_id": 123}                   │
  │                                     │
  │  13. Parse & hiển thị               │
  │     ↓                               │
  │  14. Lưu UserData                   │
  │     ↓                               │
  │  15. Chuyển màn hình                │
  │                                     ├─ 16. Close connection
  │                                     │
  │                                     ├─ 17. Thread exit
```

---

## 6. DATABASE SCHEMA (1 phút)

### 6.1. Bảng User
```sql
CREATE TABLE IF NOT EXISTS user (
    id INT AUTO_INCREMENT PRIMARY KEY,
    email VARCHAR(255) NOT NULL,
    pass VARCHAR(255) NOT NULL,
    name VARCHAR(255) NOT NULL,
    dob DATETIME
);
```

**Các trường:**
- `id`: Primary key, auto increment
- `email`: Email người dùng (unique - có thể thêm)
- `pass`: Mật khẩu (nên hash - TODO)
- `name`: Tên người dùng
- `dob`: Ngày sinh

### 6.2. Bảng Log (Inferred)
```sql
CREATE TABLE IF NOT EXISTS log (
    id INT AUTO_INCREMENT PRIMARY KEY,
    log_content TEXT,
    log_time VARCHAR(50)
);
```

**Mục đích:**
- Ghi lại mọi hoạt động trong hệ thống
- Hỗ trợ audit và debugging

---

## 7. DEMO CHỨC NĂNG (2 phút)

### 7.1. Chuẩn bị Demo

**Terminal 1 - Start Server:**
```bash
cd backend
make
./server
```

**Output mong đợi:**
```
Connect to db successful
Server listening on port 8081
```

**Terminal 2 - Start Client:**
```bash
cd frontend
make
./frontend.app/Contents/MacOS/frontend
```

### 7.2. Demo Flow

**Bước 1: Đăng ký tài khoản mới**
1. Mở form Signup
2. Nhập thông tin:
   - Email: demo@example.com
   - Password: 123456
   - Name: Demo User
   - DOB: 2000-01-01
3. Click "Đăng ký"
4. Kiểm tra message box: "Đăng ký thành công"

**Kiểm tra Log Server:**
```
header: CONTROL SIGN_UP - body: {"email":"demo@example.com",...}
Query: INSERT INTO user...
```

**Kiểm tra Database:**
```bash
mysql> SELECT * FROM user WHERE email='demo@example.com';
```

**Bước 2: Đăng nhập**
1. Click "Đã có tài khoản" → chuyển sang Signin
2. Nhập:
   - Email: demo@example.com
   - Password: 123456
3. Click "Đăng nhập"
4. Thấy label: "Đăng nhập thành công! User ID: X"

**Kiểm tra Log Server:**
```
header: CONTROL LOGIN - body: {"email":"demo@example.com","password":"123456"}
Login successful, user_id: X
```

**Bước 3: Test lỗi**
1. Nhập sai password
2. Thấy message: "Đăng nhập thất bại"

---

## 8. CÔNG NGHỆ & KỸ THUẬT SỬ DỤNG (1 phút)

### 8.1. Backend Technologies

| Công nghệ | Mục đích | Chi tiết |
|-----------|----------|----------|
| **Socket Programming** | Network communication | TCP Socket, bind, listen, accept |
| **Multi-threading** | Concurrent clients | pthread_create, mutex locking |
| **MySQL C API** | Database interaction | mysql_query, mysql_store_result |
| **JSON Parsing** | Data format | Custom parser with cJSON |
| **File I/O** | Logging | fopen, fprintf, fclose |

### 8.2. Frontend Technologies

| Công nghệ | Mục đích | Chi tiết |
|-----------|----------|----------|
| **Qt Framework** | GUI Development | QWidget, QMainWindow |
| **Qt Network** | TCP Client | QTcpSocket |
| **Qt JSON** | JSON handling | QJsonDocument, QJsonObject |
| **Signals & Slots** | Event handling | Qt's signal-slot mechanism |

### 8.3. Design Patterns Implemented

1. **MVC Pattern:** Separation of concerns (Controller-Service-Model)
2. **Singleton Pattern:** UserData class, Database connection
3. **Thread Pool Pattern:** pthread với detached state
4. **Factory Pattern:** Message routing based on type

---

## 9. ĐIỂM MẠNH CỦA DỰ ÁN (1 phút)

### 9.1. Về kỹ thuật

✅ **Multi-threading hiệu quả:**
- Sử dụng pthread để xử lý đồng thời nhiều client
- Mutex để đảm bảo thread-safe khi access shared resources
- Detached threads tự động cleanup

✅ **Protocol rõ ràng:**
- Message format chuẩn, dễ mở rộng
- Hỗ trợ 3 loại message: CONTROL, DATA, NOTIFICATION
- JSON body cho flexibility

✅ **Kiến trúc sạch:**
- Tách biệt concerns: Controller → Service → Database
- Dễ maintain và mở rộng
- Code organization tốt

✅ **Error handling:**
- Check return values của system calls
- Proper error messages
- Logging đầy đủ

### 9.2. Về chức năng

✅ **Authentication hoàn chỉnh:**
- Đăng ký với validation
- Đăng nhập an toàn
- Session management (có thể mở rộng)

✅ **Logging system:**
- Ghi log vào database
- Ghi log ra file
- Timestamp cho mọi operation

✅ **User experience:**
- GUI đẹp, thân thiện
- Response times nhanh
- Error messages rõ ràng

---

## 10. HẠN CHẾ VÀ HƯỚNG PHÁT TRIỂN (1 phút)

### 10.1. Hạn chế hiện tại

⚠️ **Security:**
- Password chưa được hash (plaintext trong DB)
- Thiếu SQL injection prevention
- Chưa có HTTPS/TLS encryption

⚠️ **Scalability:**
- Mỗi client = 1 thread → không scale với nhiều users
- Không có connection pooling
- Single database connection

⚠️ **Features:**
- Chưa có session management
- Chưa có "Remember me" feature
- Chưa có password recovery

### 10.2. Kế hoạch phát triển

**Phase 2: Bảo mật**
```
□ Hash password với bcrypt/SHA-256
□ Prepared statements để prevent SQL injection
□ Add SSL/TLS cho TCP connection
□ Input validation và sanitization
□ Rate limiting để prevent brute force
```

**Phase 3: Session Management**
```
□ Generate session tokens
□ Token-based authentication
□ Session timeout
□ Multiple device support
□ Logout functionality
```

**Phase 4: Core Features**
```
□ Triển khai DATA message protocol
□ File transfer functionality
□ Chat/messaging system
□ User profile management
□ Friend list/contacts
```

**Phase 5: Optimization**
```
□ Chuyển từ thread-per-connection sang event-driven (epoll/select)
□ Connection pooling cho database
□ Caching layer (Redis)
□ Load balancing
```

**Phase 6: Advanced Features**
```
□ Real-time notifications
□ Group chat
□ File sharing
□ Video/audio call (nếu có thời gian)
```

---

## 11. PHÂN CÔNG CÔNG VIỆC (30 giây)

### Đã hoàn thành:

| Thành viên | Công việc | Trạng thái |
|------------|-----------|------------|
| Member 1 | Backend server, socket, threading | ✅ Done |
| Member 2 | Database, authentication service | ✅ Done |
| Member 3 | Frontend GUI, Qt interface | ✅ Done |
| Member 4 | Protocol design, integration | ✅ Done |

### Đang làm:
- Testing và bug fixing
- Documentation
- Chuẩn bị demo

---

## 12. DEMO THỰC TẾ (2-3 phút)

### 12.1. Chuẩn bị
```bash
# Terminal 1: Khởi động MySQL
sudo service mysql start

# Terminal 2: Chạy server
cd backend
make clean && make
./server

# Terminal 3: Chạy client
cd frontend  
make
./frontend.app/Contents/MacOS/frontend
```

### 12.2. Các test cases

**Test 1: Đăng ký thành công**
- Email: test1@gmail.com
- Password: 123456
- Name: Test User
- DOB: 2000-01-01
- Expected: "Đăng ký thành công"

**Test 2: Đăng nhập thành công**
- Email: test1@gmail.com
- Password: 123456
- Expected: "Đăng nhập thành công! User ID: X"

**Test 3: Đăng nhập thất bại**
- Email: test1@gmail.com
- Password: wrongpassword
- Expected: "Đăng nhập thất bại"

**Test 4: Kiểm tra concurrent connections**
- Mở 2 client cùng lúc
- Đăng nhập đồng thời
- Expected: Cả 2 đều hoạt động bình thường

### 12.3. Kiểm tra logs
```bash
# Xem server logs
cat backend/log.txt

# Xem database logs
mysql> SELECT * FROM log ORDER BY id DESC LIMIT 10;
```

---

## 13. KẾT LUẬN (1 phút)

### 13.1. Tổng kết

Đến thời điểm hiện tại, nhóm đã hoàn thành:

✅ **Backend Server:**
- Socket programming với TCP
- Multi-threaded server
- MVC architecture
- MySQL integration
- Complete authentication system

✅ **Frontend Client:**
- Qt GUI application
- TCP client implementation
- Forms: Login, Signup
- Response handling

✅ **Protocol:**
- Custom message format
- JSON data exchange
- Error handling

✅ **Database:**
- User table
- Log table
- CRUD operations

### 13.2. Kiến thức đã áp dụng

Qua dự án này, nhóm đã thực hành:

1. **Socket Programming:** TCP socket, bind, listen, accept, connect
2. **Multi-threading:** pthread, mutex, concurrent programming
3. **Network Protocol:** Thiết kế và implement custom protocol
4. **Client-Server Model:** Communication patterns
5. **Database Integration:** MySQL C API
6. **GUI Programming:** Qt framework
7. **JSON Processing:** Data serialization
8. **Error Handling:** Network errors, system call failures
9. **Logging & Debugging:** Troubleshooting distributed systems

### 13.3. Thời gian hoàn thành tiếp theo

**Tuần tới:**
- Implement password hashing
- Add input validation
- Complete documentation
- More comprehensive testing

**2 tuần tới:**
- Session management
- Token-based auth
- Profile management
- Deploy on cloud

### 13.4. Lời cảm ơn

Em xin cảm ơn thầy/cô đã theo dõi. Nhóm em rất mong nhận được góp ý từ thầy/cô để hoàn thiện dự án tốt hơn!

---

## PHỤ LỤC: TROUBLESHOOTING

### A. Server không khởi động
```bash
# Check port có bị chiếm không
netstat -tlnp | grep 8081

# Kill process nếu cần
kill -9 <PID>

# Check MySQL running
sudo service mysql status
```

### B. Client không kết nối được
```bash
# Check server đang chạy
ps aux | grep server

# Check firewall
sudo ufw status

# Test connection
telnet localhost 8081
```

### C. Database connection failed
```sql
-- Check user permissions
SHOW GRANTS FOR 'quizz'@'localhost';

-- Check database exists
SHOW DATABASES;
USE quizz_db;
SHOW TABLES;
```

---

## CÂU HỎI DỰ KIẾN TỪ GIẢNG VIÊN

### Q1: "Tại sao không dùng HTTP/REST API mà lại tự thiết kế protocol?"

**Trả lời:** 
- Em muốn hiểu sâu về cách hoạt động của network protocol ở tầng thấp
- Đây là bài tập lập trình mạng nên em implement raw socket
- Protocol tùy chỉnh giúp em học về message format, parsing, và handling
- Trong tương lai có thể mở rộng thêm binary protocol cho file transfer

### Q2: "Làm thế nào để xử lý khi nhiều client cùng đăng nhập?"

**Trả lời:**
- Server sử dụng multi-threading với pthread
- Mỗi client connection được xử lý trong 1 thread riêng
- Mutex lock bảo vệ critical sections (database access)
- Threads chạy detached nên tự cleanup khi xong

### Q3: "Password lưu plaintext có an toàn không?"

**Trả lời:**
- Em nhận thức được đây là security vulnerability
- Trong phase tiếp theo em sẽ implement password hashing với bcrypt
- Hiện tại em focus vào việc làm cho hệ thống hoạt động trước
- Em cũng sẽ thêm salt để tăng security

### Q4: "Làm sao scale khi có 1000+ concurrent users?"

**Trả lời:**
- Hiện tại với thread-per-connection sẽ không scale tốt
- Em có kế hoạch migrate sang event-driven model (epoll/select)
- Có thể implement connection pooling
- Load balancing với multiple server instances
- Horizontal scaling với message queue (RabbitMQ/Redis)

### Q5: "JSON parsing performance có vấn đề không?"

**Trả lời:**
- Với traffic nhỏ, JSON parsing performance chấp nhận được
- JSON dễ debug và human-readable
- Nếu cần optimize có thể chuyển sang binary protocol (Protocol Buffers)
- Hoặc dùng MessagePack cho smaller payload size

### Q6: "Error recovery khi server crash?"

**Trả lời:**
- Hiện tại client sẽ nhận disconnect error
- Em có log đầy đủ để debug khi server crash
- Kế hoạch thêm auto-reconnect logic ở client
- Implement graceful shutdown với signal handlers
- Health check endpoints để monitor server status

---

## TÀI LIỆU THAM KHẢO

1. **Beej's Guide to Network Programming** - Socket programming basics
2. **MySQL C API Documentation** - Database integration
3. **Qt Documentation** - GUI development
4. **POSIX Threads Programming** - Multi-threading
5. **RFC 6455** - WebSocket Protocol (tham khảo cho protocol design)

---

**END OF SCRIPT**

*Lưu ý: Thời gian thuyết trình ước tính: 15-20 phút (tùy tốc độ nói và demo)*
*Chuẩn bị sẵn backup video demo phòng trường hợp technical issues*


