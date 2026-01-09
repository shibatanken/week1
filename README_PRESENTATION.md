# 📚 TÀI LIỆU THUYẾT TRÌNH - DỰ ÁN LẬP TRÌNH MẠNG

## 📋 DANH SÁCH TÀI LIỆU

Tôi đã tạo sẵn **4 tài liệu** để hỗ trợ bạn thuyết trình:

### 1. 📄 **SCRIPT_THUYET_TRINH.md** (Chi tiết nhất - 30+ trang)
**Mục đích:** Script thuyết trình hoàn chỉnh với mọi chi tiết kỹ thuật

**Nội dung:**
- ✅ 13 phần chính từ giới thiệu đến kết luận
- ✅ Code examples chi tiết
- ✅ Giải thích kỹ thuật sâu
- ✅ Luồng xử lý request-response
- ✅ Database schema
- ✅ Hướng dẫn demo từng bước
- ✅ Troubleshooting guide
- ✅ Q&A với câu trả lời mẫu
- ✅ Phụ lục và tài liệu tham khảo

**Khi nào dùng:** 
- Đọc trước 1-2 ngày để hiểu toàn bộ project
- Tham khảo khi chuẩn bị slides
- Reference khi có câu hỏi khó

**Thời lượng:** 15-20 phút nếu trình bày đầy đủ

---

### 2. 📊 **SLIDE_NOTES.md** (Notes cho slides - 15 trang)
**Mục đích:** Notes ngắn gọn cho từng slide thuyết trình

**Nội dung:**
- ✅ 22 slides với notes cụ thể
- ✅ Bullet points dễ nhớ
- ✅ Key messages cho mỗi slide
- ✅ Tips thuyết trình
- ✅ Câu mở đầu & kết thúc
- ✅ Xử lý sự cố khi demo

**Khi nào dùng:**
- Tham khảo khi tạo PowerPoint/slides
- Đọc lướt trước 1 giờ thuyết trình
- Notes khi thuyết trình (nếu cần)

**Thời lượng:** 10-15 phút

---

### 3. 📝 **CHEAT_SHEET.md** (Cheat sheet - 8 trang)
**Mục đích:** Tóm tắt ngắn gọn nhất, dễ nhớ, dễ tra cứu nhanh

**Nội dung:**
- ✅ Thông tin cơ bản (3 phút nhanh)
- ✅ Lệnh demo quan trọng
- ✅ Test cases cụ thể
- ✅ Protocol format
- ✅ Kiến trúc hệ thống (ASCII art)
- ✅ Q&A ngắn gọn
- ✅ Troubleshooting nhanh
- ✅ Checklist trước giờ thuyết trình
- ✅ Opening & Closing scripts
- ✅ Timeline thuyết trình

**Khi nào dùng:**
- **IN RA GIẤY** và mang theo khi thuyết trình
- Đọc 5 phút trước giờ G
- Tra cứu nhanh khi quên
- Nhìn lén khi thuyết trình (nếu cần)

**Thời lượng:** 5-10 phút (core content only)

---

### 4. 🧪 **test_demo.sh** (Automated test script)
**Mục đích:** Script tự động test toàn bộ hệ thống trước giờ demo

**Chức năng:**
- ✅ Check MySQL connection
- ✅ Check port 8081 availability
- ✅ Compile backend & frontend
- ✅ Check dependencies (MySQL lib, cJSON, Qt)
- ✅ Test server startup
- ✅ Summary report
- ✅ Manual demo instructions

**Cách dùng:**
```bash
# Chạy trước giờ thuyết trình 1 ngày để test
bash test_demo.sh

# Hoặc
chmod +x test_demo.sh
./test_demo.sh
```

**Output:** 
- Green ✓ = Pass
- Red ✗ = Fail
- Yellow ⚠ = Warning

---

## 🎯 HƯỚNG DẪN SỬ DỤNG

### 📅 TIMELINE CHUẨN BỊ

#### **1 TUẦN TRƯỚC:**
1. Đọc kỹ `SCRIPT_THUYET_TRINH.md` (toàn bộ)
2. Test code: chạy `test_demo.sh`
3. Fix bugs nếu có
4. Chuẩn bị slides dựa trên `SLIDE_NOTES.md`

#### **3 NGÀY TRƯỚC:**
1. Tạo PowerPoint slides
2. Practice thuyết trình với `SLIDE_NOTES.md`
3. Record video backup demo
4. Test lại với `test_demo.sh`

#### **1 NGÀY TRƯỚC:**
1. Practice thuyết trình 3-5 lần
2. Đọc lại `CHEAT_SHEET.md`
3. In `CHEAT_SHEET.md` ra giấy
4. Chuẩn bị backup (USB, video)
5. Check laptop, sạc pin

#### **1 GIỜ TRƯỚC:**
1. Đọc `CHEAT_SHEET.md` section "Checklist"
2. Chạy `test_demo.sh` lần cuối
3. Test demo thật 1 lần
4. Hít thơm sâu, tự tin!

#### **5 PHÚT TRƯỚC:**
1. Khởi động server
2. Test login/signup 1 lần
3. Đọc lại Opening script trong `CHEAT_SHEET.md`
4. Relax, you got this! 💪

---

## 🎤 CÁCH THUYẾT TRÌNH HIỆU QUẢ

### OPTION 1: Thuyết trình CHI TIẾT (15-20 phút)
**Dùng:** `SCRIPT_THUYET_TRINH.md`
- Trình bày đầy đủ mọi khía cạnh
- Phù hợp nếu có nhiều thời gian
- Demo chi tiết từng test case
- Q&A sâu

### OPTION 2: Thuyết trình TRUNG BÌNH (10-15 phút) ⭐ RECOMMENDED
**Dùng:** `SLIDE_NOTES.md`
- Focus vào key points
- Demo 2-3 test cases chính
- Technical details vừa phải
- Q&A cơ bản

### OPTION 3: Thuyết trình NHANH (5-10 phút)
**Dùng:** `CHEAT_SHEET.md`
- Intro + Kiến trúc + Demo + Kết luận
- Demo 1-2 test cases
- Chỉ highlight technical
- Q&A ngắn

---

## 📖 CẤU TRÚC THUYẾT TRÌNH MẪU (15 phút)

### **Phần 1: Mở đầu** (1 phút)
```
- Giới thiệu đề tài
- Tech stack
- Chức năng hoàn thành
→ Tham khảo: CHEAT_SHEET.md → Opening
```

### **Phần 2: Kiến trúc & Kỹ thuật** (4 phút)
```
- Mô hình Client-Server
- Backend: Socket, Multi-threading, MVC
- Frontend: Qt, TCP Client
- Protocol: Custom message format
→ Tham khảo: SLIDE_NOTES.md → Slides 2-7
```

### **Phần 3: Demo** (5 phút) ⭐ QUAN TRỌNG NHẤT
```
- Start server
- Start client
- Demo signup → success
- Demo login → success
- Demo login → failure (wrong password)
- Show logs (server log, database, file)
→ Tham khảo: CHEAT_SHEET.md → TEST CASES
```

### **Phần 4: Technical Highlights** (2 phút)
```
- Điểm mạnh của hệ thống
- Kiến thức đã áp dụng
- Architecture decisions
→ Tham khảo: CHEAT_SHEET.md → ĐIỂM MẠNH
```

### **Phần 5: Future Work** (2 phút)
```
- Hạn chế hiện tại
- Kế hoạch phát triển
- Timeline
→ Tham khảo: SLIDE_NOTES.md → Slides 17-18
```

### **Phần 6: Kết luận** (1 phút)
```
- Tóm tắt thành quả
- Lời cảm ơn
- Mở Q&A
→ Tham khảo: CHEAT_SHEET.md → Closing
```

---

## 🎬 HƯỚNG DẪN DEMO CHI TIẾT

### Chuẩn bị Terminal:

**Terminal 1: Server**
```bash
cd backend
./server
# → Expect: "Server listening on port 8081"
```

**Terminal 2: Client**
```bash
cd frontend
./frontend.app/Contents/MacOS/frontend
# → GUI xuất hiện
```

**Terminal 3: Database Monitor** (Optional)
```bash
mysql -u quizz -pQuizz2003@ -D quizz_db

# Trong MySQL:
SELECT * FROM user;
SELECT * FROM log ORDER BY id DESC LIMIT 5;
```

**Terminal 4: Log Monitor** (Optional)
```bash
tail -f backend/log.txt
```

### Demo Flow:

#### **Test 1: Đăng ký mới** ✅
```
1. Click "Đăng ký" (nếu đang ở signin)
2. Nhập:
   Email: demo@test.com
   Password: 123456
   Name: Demo User
   DOB: 2000-01-01
3. Click nút "Đăng ký"
4. → Message box: "Đăng ký thành công"
5. Check Terminal 1: "CONTROL SIGN_UP..."
6. Check Terminal 3: User mới trong database
```

#### **Test 2: Đăng nhập thành công** ✅
```
1. Click "Đã có tài khoản"
2. Nhập:
   Email: demo@test.com
   Password: 123456
3. Click "Đăng nhập"
4. → Label: "Đăng nhập thành công! User ID: X"
5. → Label màu xanh
6. Check Terminal 1: "CONTROL LOGIN..."
```

#### **Test 3: Đăng nhập thất bại** ❌
```
1. Nhập:
   Email: demo@test.com
   Password: wrongpassword
2. Click "Đăng nhập"
3. → Label: "Đăng nhập thất bại"
4. → Label màu đỏ
```

#### **Test 4: Concurrent** (Optional) ⚡
```
1. Mở client thứ 2
2. Đăng nhập cả 2 cùng lúc
3. → Cả 2 đều thành công
4. Giải thích: Multi-threading với pthread
```

---

## ❓ Q&A PREPARATION

### Top 10 câu hỏi có thể gặp:

1. **"Tại sao tự thiết kế protocol?"**
   → Xem `CHEAT_SHEET.md` → Q&A → Q1

2. **"Xử lý nhiều concurrent users thế nào?"**
   → Xem `CHEAT_SHEET.md` → Q&A → Q2

3. **"Password security?"**
   → Xem `CHEAT_SHEET.md` → Q&A → Q3

4. **"SQL injection prevention?"**
   → Xem `CHEAT_SHEET.md` → Q&A → Q4

5. **"Thread-safe?"**
   → Xem `CHEAT_SHEET.md` → Q&A → Q5

6. **"Tại sao dùng JSON?"**
   → Xem `CHEAT_SHEET.md` → Q&A → Q6

7. **"Server crash recovery?"**
   → Xem `CHEAT_SHEET.md` → Q&A → Q7

8. **"Database connection pool?"**
   → Xem `CHEAT_SHEET.md` → Q&A → Q8

9. **"Performance bottlenecks?"**
   → Xem `SCRIPT_THUYET_TRINH.md` → Section 13

10. **"Future improvements?"**
    → Xem `SLIDE_NOTES.md` → Slide 18

**Mẹo:** In ra phần Q&A từ `CHEAT_SHEET.md` để tham khảo nhanh!

---

## 🚨 XỬ LÝ SỰ CỐ KHI DEMO

### Sự cố 1: Server không start
```
Nguyên nhân: Port 8081 bị chiếm
Giải pháp: 
  netstat -tlnp | grep 8081
  kill -9 <PID>
  ./server
```

### Sự cố 2: Client không connect
```
Nguyên nhân: Server chưa chạy / Firewall block
Giải pháp:
  - Check server running: ps aux | grep server
  - Test: telnet localhost 8081
  - Nếu vẫn không được → Show video backup
```

### Sự cố 3: MySQL connection failed
```
Nguyên nhân: MySQL không chạy / Wrong credentials
Giải pháp:
  sudo service mysql start
  - Nếu vẫn lỗi → Show screenshots có sẵn
```

### Sự cố 4: Demo hoàn toàn thất bại
```
Giải pháp:
1. Giữ bình tĩnh, mỉm cười
2. Nói: "Em gặp technical issue, em xin phép dùng video backup"
3. Play video đã chuẩn bị
4. Giải thích trong khi video chạy
5. Sau đó tiếp tục phần technical presentation
```

**Lưu ý:** Giảng viên hiểu rằng demo có thể fail. Quan trọng là bạn hiểu code và giải thích được!

---

## ✅ CHECKLIST CUỐI CÙNG

### 1 Ngày Trước:
- [ ] Đọc xong SCRIPT_THUYET_TRINH.md
- [ ] Chạy test_demo.sh → All pass
- [ ] Slides đã hoàn thành
- [ ] Practice thuyết trình 3 lần
- [ ] Record video backup
- [ ] In CHEAT_SHEET.md ra giấy

### 3 Giờ Trước:
- [ ] Test demo 1 lần nữa
- [ ] Backup code lên USB
- [ ] Laptop đầy pin / có sạc
- [ ] MySQL đang chạy
- [ ] Port 8081 free

### 30 Phút Trước:
- [ ] Đọc CHEAT_SHEET.md lại 1 lần
- [ ] Kiểm tra slides
- [ ] Uống nước
- [ ] Hít thở sâu

### 5 Phút Trước:
- [ ] Start server thử
- [ ] Login 1 lần test
- [ ] Đọc Opening script
- [ ] SMILE 😊

---

## 💡 TIPS QUAN TRỌNG

### DO ✅
- Nói chậm, rõ ràng
- Tự tin với code của mình
- Nhìn vào audience
- Giải thích logic, không chỉ đọc code
- Show enthusiasm về project
- Tương tác với giảng viên
- Chuẩn bị backup plan

### DON'T ❌
- Nói quá nhanh
- Đọc từng chữ trên slide
- Quay lưng với audience lâu
- Panic khi có lỗi
- Skip demo (quan trọng nhất!)
- Nói quá kỹ thuật mà không giải thích
- Quên cảm ơn ở cuối

---

## 📞 LIÊN HỆ & HỖ TRỢ

Nếu có câu hỏi về tài liệu hoặc cần giải thích thêm:
- Đọc lại `SCRIPT_THUYET_TRINH.md` → Phần tương ứng
- Check `CHEAT_SHEET.md` → Q&A
- Run `test_demo.sh` để kiểm tra technical issues

---

## 📚 TÓM TẮT FILES

| File | Pages | Mục đích | Khi nào dùng |
|------|-------|----------|--------------|
| `SCRIPT_THUYET_TRINH.md` | 30+ | Chi tiết đầy đủ | Đọc trước 1-2 ngày |
| `SLIDE_NOTES.md` | 15 | Notes cho slides | Tham khảo khi làm slides |
| `CHEAT_SHEET.md` | 8 | Quick reference | **IN RA** mang theo |
| `test_demo.sh` | - | Auto test | Chạy trước giờ G |
| `README_PRESENTATION.md` | 10 | Hướng dẫn tổng | File này! |

---

## 🎯 RECOMMENDED WORKFLOW

```
┌─────────────────────────────────────────┐
│  1 TUẦN TRƯỚC                           │
│  └─ Đọc SCRIPT_THUYET_TRINH.md         │
│  └─ Chạy test_demo.sh                   │
│  └─ Fix bugs                            │
└────────────────┬────────────────────────┘
                 │
┌────────────────▼────────────────────────┐
│  3 NGÀY TRƯỚC                           │
│  └─ Tạo slides từ SLIDE_NOTES.md       │
│  └─ Practice 3-5 lần                    │
│  └─ Record video backup                 │
└────────────────┬────────────────────────┘
                 │
┌────────────────▼────────────────────────┐
│  1 NGÀY TRƯỚC                           │
│  └─ Đọc CHEAT_SHEET.md                 │
│  └─ IN RA GIẤY                         │
│  └─ Final test với test_demo.sh       │
└────────────────┬────────────────────────┘
                 │
┌────────────────▼────────────────────────┐
│  1 GIỜ TRƯỚC                            │
│  └─ Đọc lại CHEAT_SHEET.md            │
│  └─ Test demo thật                     │
│  └─ Relax, hít thở                     │
└────────────────┬────────────────────────┘
                 │
┌────────────────▼────────────────────────┐
│  SHOWTIME! 🎤                           │
│  └─ Mỉm cười, tự tin                    │
│  └─ You got this! 💪                    │
└─────────────────────────────────────────┘
```

---

## 🎊 LỜI KHUYÊN CUỐI

1. **Hiểu > Nhớ:** Hiểu logic của code quan trọng hơn nhớ từng dòng
2. **Demo > Slides:** 1 demo thành công = 1000 slides đẹp
3. **Confidence:** Bạn đã build được hệ thống này, hãy tự hào!
4. **Backup Plan:** Luôn có plan B (video, screenshots)
5. **Relax:** Giảng viên muốn thấy bạn thành công
6. **Enjoy:** Đây là cơ hội show off công sức của bạn!

---

## 🚀 SẴN SÀNG CHƯA?

- ✅ Đọc tài liệu? → Check
- ✅ Test code? → Check
- ✅ Chuẩn bị slides? → Check
- ✅ Practice? → Check
- ✅ Backup? → Check
- ✅ Tự tin? → **CHECK!** 💪

---

**CHÚC BẠN THUYẾT TRÌNH THÀNH CÔNG! 🎉🔥**

*Remember: You've built something amazing. Now go show it to the world!*

---

**END OF DOCUMENTATION**

*Made with ❤️ for your Network Programming presentation*


