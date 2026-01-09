# 📊 VISUAL DIAGRAMS - LẬP TRÌNH MẠNG
## Các sơ đồ minh họa cho thuyết trình

---

## 1. KIẾN TRÚC TỔNG QUAN (High-Level Architecture)

```
┌─────────────────────────────────────────────────────────────┐
│                      HỆ THỐNG TỔNG QUAN                      │
└─────────────────────────────────────────────────────────────┘

        USER
         │
         ├─ Input: Email, Password
         │
         ▼
   ┌──────────┐
   │  CLIENT  │  Qt C++ Application
   │  ◆ GUI   │  - QTcpSocket
   │  ◆ Forms │  - JSON handling
   │  ◆ Socket│  - Signal/Slots
   └─────┬────┘
         │
         │ TCP Connection
         │ Port: 8081
         │ Protocol: Custom (CONTROL/DATA/NOTIFICATION)
         │
         ▼
   ┌──────────┐
   │  SERVER  │  C Application
   │  ◆ Socket│  - Multi-threaded (pthread)
   │  ◆ Routes│  - MVC Architecture
   │  ◆ Auth  │  - JSON Parser
   └─────┬────┘
         │
         │ MySQL Connection
         │ Library: libmysqlclient
         │
         ▼
   ┌──────────┐
   │ DATABASE │  MySQL
   │  ◆ Users │  - user table
   │  ◆ Logs  │  - log table
   └──────────┘
```

---

## 2. LUỒNG DỮ LIỆU (Data Flow)

```
┌───────────────────────────────────────────────────────────┐
│                    LUỒNG ĐĂNG NHẬP                         │
└───────────────────────────────────────────────────────────┘

CLIENT                  NETWORK              SERVER              DATABASE
  │                        │                    │                   │
  │ 1. User nhập           │                    │                   │
  │    email & password    │                    │                   │
  ├────────────────────────┤                    │                   │
  │                        │                    │                   │
  │ 2. Tạo JSON            │                    │                   │
  │    {"email":"...",     │                    │                   │
  │     "password":"..."}  │                    │                   │
  ├────────────────────────┤                    │                   │
  │                        │                    │                   │
  │ 3. Gửi qua TCP         │                    │                   │
  ├───────────────────────→│                    │                   │
  │    CONTROL LOGIN       │                    │                   │
  │    {JSON data}         │                    │                   │
  │                        │                    │                   │
  │                        │ 4. Accept conn     │                   │
  │                        ├───────────────────→│                   │
  │                        │                    │                   │
  │                        │                    │ 5. Create thread  │
  │                        │                    ├──────────────┐    │
  │                        │                    │              │    │
  │                        │                    │ 6. Read data │    │
  │                        │                    │◄─────────────┘    │
  │                        │                    │                   │
  │                        │                    │ 7. Parse header   │
  │                        │                    │    → "CONTROL     │
  │                        │                    │       LOGIN"      │
  │                        │                    │                   │
  │                        │                    │ 8. Parse body     │
  │                        │                    │    → email, pass  │
  │                        │                    │                   │
  │                        │                    │ 9. Route to       │
  │                        │                    │    auth_controller│
  │                        │                    │                   │
  │                        │                    │ 10. Call          │
  │                        │                    │     auth_service  │
  │                        │                    │                   │
  │                        │                    │ 11. Query DB      │
  │                        │                    ├──────────────────→│
  │                        │                    │  SELECT * FROM    │
  │                        │                    │  user WHERE...    │
  │                        │                    │                   │
  │                        │                    │ 12. Result        │
  │                        │                    │◄──────────────────┤
  │                        │                    │  user_id: 123     │
  │                        │                    │                   │
  │                        │                    │ 13. Log activity  │
  │                        │                    ├──────────────────→│
  │                        │                    │  INSERT INTO log  │
  │                        │                    │                   │
  │                        │                    │ 14. Build response│
  │                        │                    │     NOTIFICATION  │
  │                        │                    │     LOGIN_SUCCESS │
  │                        │                    │                   │
  │                        │ 15. Send response  │                   │
  │                        │◄───────────────────┤                   │
  │                        │  NOTIFICATION      │                   │
  │                        │  LOGIN_SUCCESS     │                   │
  │                        │  {"user_id":123}   │                   │
  │                        │                    │                   │
  │ 16. Receive            │                    │                   │
  │◄───────────────────────┤                    │                   │
  │                        │                    │                   │
  │ 17. Parse response     │                    │                   │
  │     → LOGIN_SUCCESS    │                    │                   │
  │                        │                    │                   │
  │ 18. Update UI          │                    │                   │
  │     → Show success     │                    │                   │
  │     → Display user_id  │                    │                   │
  │                        │                    │ 19. Close socket  │
  │                        │                    │                   │
  │                        │                    │ 20. Thread exit   │
  │                        │                    │                   │
```

---

## 3. KIẾN TRÚC BACKEND (Backend Architecture)

```
┌───────────────────────────────────────────────────────────────┐
│                    BACKEND MVC ARCHITECTURE                    │
└───────────────────────────────────────────────────────────────┘

                           ┌─────────────┐
                           │   main.c    │
                           │  ─────────  │
                           │ • socket()  │
                           │ • bind()    │
                           │ • connect_db│
                           └──────┬──────┘
                                  │
                                  │ calls
                                  ▼
                         ┌────────────────┐
                         │   routes.c     │
                         │  ────────────  │
                         │ • listen()     │
                         │ • accept()     │
                         │ • pthread      │
                         │   management   │
                         └────────┬───────┘
                                  │
                     ┌────────────┼────────────┐
                     │            │            │
                     ▼            ▼            ▼
           ┌─────────────┐ ┌───────────┐ ┌──────────────┐
           │  CONTROL    │ │   DATA    │ │ NOTIFICATION │
           │  Messages   │ │  Messages │ │  Messages    │
           └──────┬──────┘ └─────┬─────┘ └──────┬───────┘
                  │                │              │
                  │ routes to      │              │
                  ▼                ▼              ▼
        ┌──────────────────────────────────────────────┐
        │            CONTROLLER LAYER                   │
        ├──────────────────────────────────────────────┤
        │  • auth_controller.c                         │
        │    ├─ handle_login()                         │
        │    └─ handle_signup()                        │
        │  • (future: game_controller.c)               │
        │  • (future: file_controller.c)               │
        └─────────────────┬────────────────────────────┘
                          │
                          │ calls
                          ▼
        ┌──────────────────────────────────────────────┐
        │             SERVICE LAYER                     │
        ├──────────────────────────────────────────────┤
        │  • auth_service.c                            │
        │    ├─ signup() - Business logic              │
        │    └─ login() - Validation                   │
        │  • (future: game_service.c)                  │
        └─────────────────┬────────────────────────────┘
                          │
                          │ uses
                          ▼
        ┌──────────────────────────────────────────────┐
        │              DATABASE LAYER                   │
        ├──────────────────────────────────────────────┤
        │  • connect-db.c                              │
        │    ├─ connect_db()                           │
        │    ├─ get_db_connection()                    │
        │    └─ close_db()                             │
        │  • mysql_utils.c (helpers)                   │
        └─────────────────┬────────────────────────────┘
                          │
                          │ queries
                          ▼
                    ┌──────────┐
                    │  MySQL   │
                    │ Database │
                    └──────────┘

        ┌──────────────────────────────────────────────┐
        │              UTILITY LAYER                    │
        ├──────────────────────────────────────────────┤
        │  • json_utils.c  - Parse JSON                │
        │  • log_utils.c   - File logging              │
        │  • time_utils.c  - Timestamps                │
        │  • mysql_utils.c - DB helpers                │
        └──────────────────────────────────────────────┘
```

---

## 4. MULTI-THREADING MODEL

```
┌─────────────────────────────────────────────────────────────┐
│                    MULTI-THREADING                           │
└─────────────────────────────────────────────────────────────┘

                    ┌─────────────┐
                    │ MAIN THREAD │
                    │  (routes.c) │
                    └──────┬──────┘
                           │
                           │ listen(server_fd)
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
        │              while(1) {             │
        │                  │                  │
        │         accept() ├─────┐            │
        │                  │     │            │
        └──────────────────┼─────┼────────────┘
                           │     │
                           │     │ Client 1 connected
                           │     ▼
                           │  ┌─────────────────┐
                           │  │ pthread_create()│
                           │  └────────┬────────┘
                           │           │
                           │           ▼
                           │  ┌──────────────────┐
                           │  │   THREAD 1       │
                           │  ├──────────────────┤
                           │  │ • Read request   │
                           │  │ • Parse message  │
                           │  │ • Handle login   │
                           │  │ • Query DB       │
                           │  │   (mutex lock)   │
                           │  │ • Send response  │
                           │  │ • Close socket   │
                           │  │ • Exit (detached)│
                           │  └──────────────────┘
                           │
                           │ accept() ←─────┐
                           │                │ Client 2 connected
                           │                ▼
                           │       ┌─────────────────┐
                           │       │ pthread_create()│
                           │       └────────┬────────┘
                           │                │
                           │                ▼
                           │       ┌──────────────────┐
                           │       │   THREAD 2       │
                           │       ├──────────────────┤
                           │       │ • Read request   │
                           │       │ • Parse message  │
                           │       │ • Handle signup  │
                           │       │ • Query DB       │
                           │       │   (mutex lock)   │
                           │       │ • Send response  │
                           │       │ • Close socket   │
                           │       │ • Exit (detached)│
                           │       └──────────────────┘
                           │
                           │ accept() ←─────┐
                           │                │ Client 3 connected
                           │                ▼
                           │       ┌─────────────────┐
                           │       │ pthread_create()│
                           │       └────────┬────────┘
                           │                │
                           │                ▼
                           │       ┌──────────────────┐
                           │       │   THREAD 3       │
                           │       ├──────────────────┤
                           │       │ • ...            │
                           │       └──────────────────┘
                           │
                           ▼
                    (continues...)


┌─────────────────────────────────────────────────────────────┐
│                  THREAD SYNCHRONIZATION                      │
└─────────────────────────────────────────────────────────────┘

        THREAD 1              MUTEX              THREAD 2
            │                   │                    │
            │ Need DB access    │                    │
            ├──────────────────→│                    │
            │ pthread_mutex_    │                    │
            │   lock(&lock)     │                    │
            │                   │                    │
            ├─ LOCKED ──────────┤                    │
            │                   │                    │
            │ Query database    │                    │ Need DB access
            │ INSERT INTO...    │                    ├────────────┐
            │                   │                    │  Waiting.. │
            │ Finish query      │                    │            │
            │                   │                    │◄───────────┘
            │ pthread_mutex_    │                    │
            │   unlock(&lock)   │                    │
            │                   │                    │
            ├─ UNLOCKED ────────┤                    │
            │                   │                    │
            │                   │◄───────────────────┤
            │                   │ pthread_mutex_     │
            │                   │   lock(&lock)      │
            │                   │                    │
            │                   ├──── LOCKED ────────┤
            │                   │                    │
            │                   │                    │ Query DB
            │                   │                    │ SELECT...
            │                   │                    │
            │                   │                    │ Finish
            │                   │                    │
            │                   │◄───────────────────┤
            │                   │ pthread_mutex_     │
            │                   │   unlock(&lock)    │
            │                   │                    │
            │                   ├──── UNLOCKED ──────┤
```

---

## 5. PROTOCOL MESSAGE FORMAT

```
┌─────────────────────────────────────────────────────────────┐
│                   MESSAGE STRUCTURE                          │
└─────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────┐
│              TCP PACKET                  │
├──────────────────────────────────────────┤
│                                          │
│  ┌────────────────────────────────────┐  │
│  │         HEADER LINE                │  │
│  │  (Message Type + Metadata)         │  │
│  │                                    │  │
│  │  Examples:                         │  │
│  │  • CONTROL LOGIN                   │  │
│  │  • CONTROL SIGN_UP                 │  │
│  │  • NOTIFICATION LOGIN_SUCCESS ...  │  │
│  │  • DATA UPLOAD FILE 1024           │  │
│  └────────────────────────────────────┘  │
│                                          │
│  ┌────────────────────────────────────┐  │
│  │      NEWLINE SEPARATOR (\n)        │  │
│  └────────────────────────────────────┘  │
│                                          │
│  ┌────────────────────────────────────┐  │
│  │         BODY (JSON)                │  │
│  │                                    │  │
│  │  {                                 │  │
│  │    "email": "user@mail.com",       │  │
│  │    "password": "123456"            │  │
│  │  }                                 │  │
│  └────────────────────────────────────┘  │
│                                          │
└──────────────────────────────────────────┘


┌─────────────────────────────────────────────────────────────┐
│              MESSAGE TYPE HIERARCHY                          │
└─────────────────────────────────────────────────────────────┘

                    ┌─────────────┐
                    │   MESSAGE   │
                    └──────┬──────┘
                           │
          ┌────────────────┼────────────────┐
          │                │                │
          ▼                ▼                ▼
    ┌──────────┐    ┌──────────┐    ┌────────────┐
    │ CONTROL  │    │   DATA   │    │NOTIFICATION│
    └────┬─────┘    └────┬─────┘    └─────┬──────┘
         │               │                 │
    ┌────┴────┐     ┌────┴────┐      ┌────┴────┐
    │         │     │         │      │         │
    ▼         ▼     ▼         ▼      ▼         ▼
  LOGIN   SIGN_UP  UPLOAD  DOWNLOAD SUCCESS FAILURE
                   FILE     FILE


┌─────────────────────────────────────────────────────────────┐
│                 EXAMPLE MESSAGES                             │
└─────────────────────────────────────────────────────────────┘

1. LOGIN REQUEST (Client → Server)
┌────────────────────────────────┐
│ CONTROL LOGIN                  │ ← Header
│ ───────────────────────────    │
│ {"email":"user@mail.com",      │ ← Body (JSON)
│  "password":"123456"}          │
└────────────────────────────────┘

2. LOGIN SUCCESS (Server → Client)
┌────────────────────────────────┐
│ NOTIFICATION LOGIN_SUCCESS     │ ← Header
│ 2024-12-01T10:30:00            │ ← Timestamp
│ ───────────────────────────    │
│ {"user_id": 123}               │ ← Body (JSON)
└────────────────────────────────┘

3. SIGNUP REQUEST (Client → Server)
┌────────────────────────────────┐
│ CONTROL SIGN_UP                │
│ ───────────────────────────    │
│ {"email":"new@mail.com",       │
│  "password":"pass123",         │
│  "name":"John Doe",            │
│  "dob":"2000-01-01"}           │
└────────────────────────────────┘

4. LOGIN FAILURE (Server → Client)
┌────────────────────────────────┐
│ NOTIFICATION LOGIN_FAILURE     │
│ 2024-12-01T10:30:00            │
│ ───────────────────────────    │
│ {"message": "Invalid password"}│
└────────────────────────────────┘
```

---

## 6. DATABASE SCHEMA

```
┌─────────────────────────────────────────────────────────────┐
│                      DATABASE: quizz_db                      │
└─────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────┐
│              TABLE: user                 │
├────────────┬─────────────┬───────────────┤
│  Column    │  Type       │  Constraint   │
├────────────┼─────────────┼───────────────┤
│  id        │  INT        │  PRIMARY KEY  │
│            │             │  AUTO_INC     │
├────────────┼─────────────┼───────────────┤
│  email     │  VARCHAR    │  NOT NULL     │
│            │  (255)      │  (UNIQUE?)    │
├────────────┼─────────────┼───────────────┤
│  pass      │  VARCHAR    │  NOT NULL     │
│            │  (255)      │               │
├────────────┼─────────────┼───────────────┤
│  name      │  VARCHAR    │  NOT NULL     │
│            │  (255)      │               │
├────────────┼─────────────┼───────────────┤
│  dob       │  DATETIME   │               │
└────────────┴─────────────┴───────────────┘

Example Data:
┌────┬────────────────┬──────────┬──────────┬────────────┐
│ id │ email          │ pass     │ name     │ dob        │
├────┼────────────────┼──────────┼──────────┼────────────┤
│ 1  │ user1@test.com │ 123456   │ User One │ 2000-01-01 │
│ 2  │ user2@test.com │ pass123  │ User Two │ 1999-05-15 │
└────┴────────────────┴──────────┴──────────┴────────────┘


┌──────────────────────────────────────────┐
│              TABLE: log                  │
├────────────┬─────────────┬───────────────┤
│  Column    │  Type       │  Constraint   │
├────────────┼─────────────┼───────────────┤
│  id        │  INT        │  PRIMARY KEY  │
│            │             │  AUTO_INC     │
├────────────┼─────────────┼───────────────┤
│  log_      │  TEXT       │               │
│  content   │             │               │
├────────────┼─────────────┼───────────────┤
│  log_time  │  VARCHAR    │               │
│            │  (50)       │               │
└────────────┴─────────────┴───────────────┘

Example Data:
┌────┬─────────────────────────────────┬─────────────────────┐
│ id │ log_content                     │ log_time            │
├────┼─────────────────────────────────┼─────────────────────┤
│ 1  │ User signed up: user1@test.com  │ 2024-12-01T10:00:00 │
│ 2  │ User logged in: user1@test.com  │ 2024-12-01T10:05:00 │
│ 3  │ User logged in: user2@test.com  │ 2024-12-01T10:10:00 │
└────┴─────────────────────────────────┴─────────────────────┘


┌─────────────────────────────────────────────────────────────┐
│                    RELATIONSHIPS                             │
└─────────────────────────────────────────────────────────────┘

Currently: No foreign keys (independent tables)

Future Enhancement:
┌──────────┐                    ┌──────────┐
│   user   │                    │   log    │
├──────────┤                    ├──────────┤
│ id (PK)  │◄───────────────────┤ user_id  │
│ email    │  Could add FK      │ (FK?)    │
│ pass     │  relationship      │ content  │
│ name     │                    │ time     │
│ dob      │                    │          │
└──────────┘                    └──────────┘
```

---

## 7. FRONTEND STRUCTURE

```
┌─────────────────────────────────────────────────────────────┐
│                   QT FRONTEND STRUCTURE                      │
└─────────────────────────────────────────────────────────────┘

                    ┌─────────────────┐
                    │   main.cpp      │
                    │   (Entry Point) │
                    └────────┬────────┘
                             │
                             │ creates
                             ▼
                    ┌─────────────────┐
                    │  MainWindow     │
                    │  ─────────────  │
                    │ • QStackedWidget│
                    │ • QTcpSocket    │
                    └────────┬────────┘
                             │
                             │ manages
                ┌────────────┼────────────┐
                │            │            │
                ▼            ▼            ▼
        ┌───────────┐ ┌───────────┐ ┌──────────┐
        │  Signin   │ │  Signup   │ │ (Future) │
        │  Form     │ │  Form     │ │  Home    │
        │  ───────  │ │  ───────  │ │  ──────  │
        │ • email   │ │ • email   │ │ • Profile│
        │ • password│ │ • password│ │ • Game   │
        │ • button  │ │ • name    │ │ • Logout │
        │           │ │ • dob     │ │          │
        │ • socket  │ │ • button  │ │          │
        │           │ │           │ │          │
        │ • signals │ │ • socket  │ │          │
        │   ↓       │ │           │ │          │
        │ showSignup│ │ • signals │ │          │
        │           │ │   ↓       │ │          │
        │           │ │ showSignin│ │          │
        └───────────┘ └───────────┘ └──────────┘


┌─────────────────────────────────────────────────────────────┐
│                   SIGNAL-SLOT MECHANISM                      │
└─────────────────────────────────────────────────────────────┘

    Signin Form                         MainWindow
        │                                    │
        │  emit showSignup()                 │
        ├───────────────────────────────────→│
        │                                    │ Switch to
        │                                    │ Signup widget
        │                                    │
        │                                    │
    Signup Form                              │
        │                                    │
        │  emit showSignin()                 │
        ├───────────────────────────────────→│
        │                                    │ Switch to
        │                                    │ Signin widget
        │                                    │


┌─────────────────────────────────────────────────────────────┐
│                 SOCKET COMMUNICATION FLOW                    │
└─────────────────────────────────────────────────────────────┘

    User Action        Qt Signal         Network             Server
        │                  │                 │                  │
        │ Click Login      │                 │                  │
        ├─────────────────→│                 │                  │
        │                  │ clicked()       │                  │
        │                  │                 │                  │
        │                  ├─ Prepare JSON   │                  │
        │                  ├─ connectToHost()│                  │
        │                  ├────────────────→│                  │
        │                  │                 │ TCP Connect      │
        │                  │                 ├─────────────────→│
        │                  │                 │                  │
        │                  │ connected()     │                  │
        │                  │◄────────────────┤                  │
        │                  │                 │                  │
        │                  ├─ write(data)    │                  │
        │                  ├────────────────→│ Send message     │
        │                  │                 ├─────────────────→│
        │                  │                 │                  │
        │                  │                 │  Server process  │
        │                  │                 │                  │
        │                  │                 │◄─────────────────┤
        │                  │ readyRead()     │  Response ready  │
        │                  │◄────────────────┤                  │
        │                  │                 │                  │
        │                  ├─ readAll()      │                  │
        │                  ├─ Parse response │                  │
        │                  ├─ Update UI      │                  │
        │                  │                 │                  │
        │ See Result       │                 │                  │
        │◄─────────────────┤                 │                  │
        │ "Login Success!" │                 │                  │
```

---

## 8. REQUEST-RESPONSE FLOW (Detailed)

```
┌─────────────────────────────────────────────────────────────┐
│          COMPLETE LOGIN REQUEST-RESPONSE CYCLE               │
└─────────────────────────────────────────────────────────────┘

TIME    CLIENT (Qt)              NETWORK         SERVER (C)        DATABASE
─────   ─────────────────────────────────────────────────────────────────────

T+0s    User enters:
        email: test@mail.com
        password: 123456
          │
T+1s    Click "Đăng nhập"
          │
T+2s    Create QJsonObject ─────────────────────────────────────────────────
        {                                                                    
          "email": "test@mail.com",                                          
          "password": "123456"                                               
        }                                                                    
          │
T+3s    Build message: ──────────────────────────────────────────────────── 
        "CONTROL LOGIN\n                                                     
        {"email":"test@mail.com","password":"123456"}"                       
          │
T+4s    tcpSocket->connectToHost("localhost", 8081)
          │
          ├─────────────────→ TCP SYN ────────────────────────────────────→
T+5s                                                                         
          ◄───────────────── TCP SYN-ACK ◄────────────────────────────────┤
                                                  accept() new connection   
T+6s                                              Create new thread         
                                                  Thread ID: 12345          
          │
T+7s    tcpSocket->write(message)
          │
          ├─────────────────→ TCP Data ──────────────────────────────────→
                             "CONTROL LOGIN\n                      read()   
                             {JSON...}"                              │      
T+8s                                                    Parse header:│      
                                                        type = "CONTROL"    
                                                        action = "LOGIN"    
                                                                     │      
                                                        Parse body:  │      
                                                        email = "test@..."  
                                                        pass = "123456"     
                                                                     │      
T+9s                                                    Route to:    │      
                                                        handle_login()      
                                                                     │      
T+10s                                                   Call service:│      
                                                        login(email, pass)  
                                                                     │      
T+11s                                                   Build query: │      
                                                        "SELECT * FROM      
                                                         user WHERE         
                                                         email='test@...'   
                                                         AND pass='123456'" 
                                                                     │      
T+12s                                                   mysql_query()│      
                                                                     ├────→
                                                                   Execute  
                                                                   query    
                                                                     │      
T+13s                                                                ◄────┤
                                                        Result: 1 row       
                                                        user_id: 42         
                                                                     │      
T+14s                                                   Log activity:│      
                                                        INSERT INTO log     
                                                        (...) VALUES (...)  
                                                                     ├────→
T+15s                                                                       
                                                        Build response:     
                                                        "NOTIFICATION       
                                                         LOGIN_SUCCESS      
                                                         2024-12-01...      
                                                        {"user_id":42}"     
                                                                     │      
T+16s                                                   write() to socket   
          ◄─────────────────  TCP Data  ◄────────────────────────────────┤
          "NOTIFICATION LOGIN_SUCCESS..."                                  
          │                                                                 
T+17s   readyRead signal
          │
T+18s   readAll()
        Parse response:
        - type: NOTIFICATION
        - action: LOGIN_SUCCESS
        - body: {"user_id": 42}
          │
T+19s   Update UI:
        label->setText("Đăng nhập thành công! User ID: 42")
        label->setStyleSheet("color: green")
          │
T+20s   UserData::instance().setUserData({"user_id": 42})
          │
T+21s   emit showHome()
          │
T+22s   [User sees home screen]
```

---

## 9. COMPARISON: CURRENT vs FUTURE ARCHITECTURE

```
┌─────────────────────────────────────────────────────────────┐
│            CURRENT (Phase 1) vs FUTURE (Phase 2+)            │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────┐      ┌───────────────────────────┐
│   CURRENT ARCHITECTURE  │      │   FUTURE ARCHITECTURE     │
└─────────────────────────┘      └───────────────────────────┘

┌──────────┐                     ┌──────────┐ ┌──────────┐
│ Client 1 │──┐                  │ Client 1 │─┤          │
└──────────┘  │                  └──────────┘ │          │
              │                               │ Load     │
┌──────────┐  │                  ┌──────────┐ │ Balancer │
│ Client 2 │──┼─→ ┌─────────┐    │ Client 2 │─┤          │
└──────────┘  │   │ Server  │    └──────────┘ │          │
              │   │ (Single)│                  └────┬─────┘
┌──────────┐  │   │         │                       │
│ Client N │──┘   │ 1 DB    │         ┌────────────┼────────────┐
└──────────┘      │ Conn    │         │            │            │
                  └─────────┘         ▼            ▼            ▼
                       │         ┌─────────┐ ┌─────────┐ ┌─────────┐
                       │         │Server 1 │ │Server 2 │ │Server N │
                       ▼         │ Event-  │ │ Event-  │ │ Event-  │
                  ┌─────────┐    │ Driven  │ │ Driven  │ │ Driven  │
                  │ MySQL   │    └────┬────┘ └────┬────┘ └────┬────┘
                  └─────────┘         │           │           │
                                      └─────┬─────┴──────┬────┘
                                            │            │
• Thread per conn                      ┌────▼────┐  ┌───▼─────┐
• No load balancing                    │  MySQL  │  │  Redis  │
• No caching                           │Connection│  │ (Cache) │
• Plaintext password                   │  Pool   │  └─────────┘
• No session                           └────┬────┘
• Simple protocol                           │
                                       ┌────▼──────┐
Features:                              │   MySQL   │
✅ Login/Signup                        │ (Master)  │
✅ Logging                             └───────────┘
❌ Sessions
❌ File transfer                       • Event-driven (epoll)
❌ Chat                                • Multiple servers
                                       • Connection pooling
                                       • Caching layer
                                       • Hashed passwords
                                       • Token-based auth
                                       • Binary protocol

                                       Features:
                                       ✅ Login/Signup
                                       ✅ Logging
                                       ✅ Sessions
                                       ✅ File transfer
                                       ✅ Real-time chat
                                       ✅ Scalable to 1000+ users
```

---

## 10. TIMELINE & MILESTONES

```
┌─────────────────────────────────────────────────────────────┐
│                   PROJECT TIMELINE                           │
└─────────────────────────────────────────────────────────────┘

PAST ───────────── NOW ─────────── FUTURE
  │                 │                  │
  │                 │                  │
  ▼                 ▼                  ▼

┌──────────┐   ┌──────────┐   ┌────────────┐   ┌──────────┐
│ Phase 0  │   │ Phase 1  │   │  Phase 2   │   │ Phase 3  │
│ Setup    │──→│ Auth     │──→│  Security  │──→│ Features │
│          │   │ ✅DONE   │   │  🔄 Next   │   │  📅Plan  │
└──────────┘   └──────────┘   └────────────┘   └──────────┘
                     │
          ┌──────────┼──────────┐
          │          │          │
          ▼          ▼          ▼
    ┌─────────┬─────────┬─────────┐
    │ Backend │Frontend │Protocol │
    ├─────────┼─────────┼─────────┤
    │✅Socket │✅Qt GUI │✅Custom │
    │✅pthread│✅Forms  │✅JSON   │
    │✅MVC    │✅Socket │✅3 Types│
    │✅MySQL  │✅Signals│         │
    │✅Logging│         │         │
    └─────────┴─────────┴─────────┘


PHASE 2 (Next 2 weeks):
┌────────────────────────────────┐
│ Week 1:                        │
│ □ bcrypt password hashing      │
│ □ Prepared statements          │
│ □ Input validation             │
│                                │
│ Week 2:                        │
│ □ Session tokens               │
│ □ Token-based auth             │
│ □ Remember me feature          │
└────────────────────────────────┘

PHASE 3 (Month 2):
┌────────────────────────────────┐
│ Week 3-4:                      │
│ □ File transfer (DATA protocol)│
│ □ Progress bars                │
│ □ Multiple file types          │
│                                │
│ Week 5-6:                      │
│ □ Real-time chat               │
│ □ User profiles                │
│ □ Friend list                  │
└────────────────────────────────┘

PHASE 4 (Future):
┌────────────────────────────────┐
│ □ Event-driven architecture    │
│ □ Load balancing               │
│ □ Redis caching                │
│ □ Horizontal scaling           │
│ □ Group chat                   │
│ □ Video/audio (stretch goal)   │
└────────────────────────────────┘
```

---

**END OF VISUAL DIAGRAMS**

*Use these diagrams in your PowerPoint presentation!*
*You can recreate them using draw.io, Lucidchart, or PowerPoint shapes*


