#!/bin/bash

# Script để test demo trước giờ thuyết trình
# Chạy: bash test_demo.sh

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}   DEMO TEST SCRIPT - LẬP TRÌNH MẠNG${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""

# Step 1: Check MySQL
echo -e "${YELLOW}[1/6] Checking MySQL...${NC}"
if command -v mysql &> /dev/null; then
    echo -e "${GREEN}✓ MySQL installed${NC}"
    
    # Try to connect
    if mysql -u quizz -pQuizz2003@ -e "USE quizz_db; SELECT 1;" &> /dev/null; then
        echo -e "${GREEN}✓ MySQL connection successful${NC}"
        
        # Check tables
        TABLE_COUNT=$(mysql -u quizz -pQuizz2003@ -D quizz_db -e "SHOW TABLES;" 2>/dev/null | wc -l)
        if [ $TABLE_COUNT -gt 1 ]; then
            echo -e "${GREEN}✓ Database tables exist${NC}"
        else
            echo -e "${RED}✗ Database tables not found${NC}"
            echo -e "${YELLOW}  Run migrations first!${NC}"
        fi
    else
        echo -e "${RED}✗ Cannot connect to MySQL${NC}"
        echo -e "${YELLOW}  Check credentials in backend/src/db/connect-db.c${NC}"
    fi
else
    echo -e "${RED}✗ MySQL not installed${NC}"
    exit 1
fi
echo ""

# Step 2: Check port availability
echo -e "${YELLOW}[2/6] Checking port 8081...${NC}"
if lsof -Pi :8081 -sTCP:LISTEN -t >/dev/null 2>&1; then
    echo -e "${RED}✗ Port 8081 is already in use${NC}"
    echo -e "${YELLOW}  Kill the process or use different port${NC}"
    lsof -i :8081
else
    echo -e "${GREEN}✓ Port 8081 is available${NC}"
fi
echo ""

# Step 3: Compile backend
echo -e "${YELLOW}[3/6] Compiling backend...${NC}"
cd backend
if make clean > /dev/null 2>&1 && make > /dev/null 2>&1; then
    echo -e "${GREEN}✓ Backend compiled successfully${NC}"
    if [ -f "server" ]; then
        echo -e "${GREEN}✓ Server executable created${NC}"
    fi
else
    echo -e "${RED}✗ Backend compilation failed${NC}"
    exit 1
fi
cd ..
echo ""

# Step 4: Compile frontend
echo -e "${YELLOW}[4/6] Compiling frontend...${NC}"
cd frontend
if make > /dev/null 2>&1; then
    echo -e "${GREEN}✓ Frontend compiled successfully${NC}"
    
    # Check executable (platform dependent)
    if [ -f "frontend.app/Contents/MacOS/frontend" ]; then
        echo -e "${GREEN}✓ Frontend executable created (macOS)${NC}"
    elif [ -f "frontend" ]; then
        echo -e "${GREEN}✓ Frontend executable created (Linux)${NC}"
    elif [ -f "frontend.exe" ]; then
        echo -e "${GREEN}✓ Frontend executable created (Windows)${NC}"
    else
        echo -e "${YELLOW}⚠ Frontend executable location unknown${NC}"
    fi
else
    echo -e "${RED}✗ Frontend compilation failed${NC}"
    echo -e "${YELLOW}  Make sure Qt is installed and qmake is in PATH${NC}"
fi
cd ..
echo ""

# Step 5: Check dependencies
echo -e "${YELLOW}[5/6] Checking dependencies...${NC}"

# Check MySQL lib
if ldconfig -p 2>/dev/null | grep libmysqlclient > /dev/null || [ -f "/usr/lib/libmysqlclient.so" ] || [ -f "/usr/local/lib/libmysqlclient.dylib" ]; then
    echo -e "${GREEN}✓ MySQL client library found${NC}"
else
    echo -e "${YELLOW}⚠ MySQL client library might be missing${NC}"
fi

# Check cJSON
if [ -f "/usr/include/cjson/cJSON.h" ] || [ -f "/usr/local/include/cjson/cJSON.h" ]; then
    echo -e "${GREEN}✓ cJSON library found${NC}"
else
    echo -e "${YELLOW}⚠ cJSON library might be missing${NC}"
fi

# Check Qt
if command -v qmake &> /dev/null; then
    QT_VERSION=$(qmake --version | grep "Qt version" | awk '{print $4}')
    echo -e "${GREEN}✓ Qt found: version $QT_VERSION${NC}"
else
    echo -e "${YELLOW}⚠ Qt/qmake not found in PATH${NC}"
fi
echo ""

# Step 6: Start server for quick test
echo -e "${YELLOW}[6/6] Testing server start...${NC}"
cd backend

# Start server in background
./server > /tmp/server_test.log 2>&1 &
SERVER_PID=$!
echo -e "${BLUE}  Server PID: $SERVER_PID${NC}"

# Wait a bit for server to start
sleep 2

# Check if server is running
if ps -p $SERVER_PID > /dev/null; then
    echo -e "${GREEN}✓ Server started successfully${NC}"
    
    # Check log
    if grep -q "Server listening on port 8081" /tmp/server_test.log; then
        echo -e "${GREEN}✓ Server is listening on port 8081${NC}"
    fi
    
    # Test connection with netcat
    if command -v nc &> /dev/null; then
        if timeout 1 bash -c "echo > /dev/tcp/localhost/8081" 2>/dev/null; then
            echo -e "${GREEN}✓ Server accepts connections${NC}"
        else
            echo -e "${YELLOW}⚠ Server connection test failed${NC}"
        fi
    fi
    
    # Kill test server
    echo -e "${BLUE}  Stopping test server...${NC}"
    kill $SERVER_PID 2>/dev/null
    wait $SERVER_PID 2>/dev/null
    echo -e "${GREEN}✓ Server stopped${NC}"
else
    echo -e "${RED}✗ Server failed to start${NC}"
    echo -e "${RED}  Check /tmp/server_test.log for errors${NC}"
    cat /tmp/server_test.log
fi

cd ..
echo ""

# Summary
echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}             TEST SUMMARY${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""
echo -e "${GREEN}✓ Items passed${NC}"
echo -e "${RED}✗ Items failed${NC}"
echo -e "${YELLOW}⚠ Items with warnings${NC}"
echo ""

# Demo instructions
echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}         MANUAL DEMO STEPS${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""
echo -e "${YELLOW}Terminal 1 - Start Server:${NC}"
echo "  cd backend"
echo "  ./server"
echo ""
echo -e "${YELLOW}Terminal 2 - Start Client:${NC}"
echo "  cd frontend"
echo "  # On macOS:"
echo "  ./frontend.app/Contents/MacOS/frontend"
echo "  # On Linux:"
echo "  ./frontend"
echo "  # On Windows:"
echo "  ./frontend.exe"
echo ""
echo -e "${YELLOW}Terminal 3 - Monitor Database:${NC}"
echo "  mysql -u quizz -pQuizz2003@ -D quizz_db"
echo "  SELECT * FROM user;"
echo "  SELECT * FROM log ORDER BY id DESC LIMIT 5;"
echo ""
echo -e "${YELLOW}Terminal 4 - Monitor Logs:${NC}"
echo "  tail -f backend/log.txt"
echo ""

# Test cases
echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}           TEST CASES${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""
echo -e "${GREEN}Test 1: Signup${NC}"
echo "  Email: demo@test.com"
echo "  Password: 123456"
echo "  Name: Demo User"
echo "  DOB: 2000-01-01"
echo "  → Expect: 'Đăng ký thành công'"
echo ""
echo -e "${GREEN}Test 2: Login Success${NC}"
echo "  Email: demo@test.com"
echo "  Password: 123456"
echo "  → Expect: 'Đăng nhập thành công! User ID: X'"
echo ""
echo -e "${GREEN}Test 3: Login Failure${NC}"
echo "  Email: demo@test.com"
echo "  Password: wrongpass"
echo "  → Expect: 'Đăng nhập thất bại'"
echo ""
echo -e "${GREEN}Test 4: Concurrent${NC}"
echo "  Open 2 clients, login both"
echo "  → Expect: Both work simultaneously"
echo ""

# Troubleshooting
echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}          TROUBLESHOOTING${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""
echo -e "${RED}Problem: Port 8081 in use${NC}"
echo "  Solution: netstat -tlnp | grep 8081"
echo "           kill -9 <PID>"
echo ""
echo -e "${RED}Problem: MySQL connection failed${NC}"
echo "  Solution: Check credentials in connect-db.c"
echo "           sudo service mysql start"
echo ""
echo -e "${RED}Problem: Compilation error${NC}"
echo "  Solution: make clean && make"
echo "           Check library installations"
echo ""

# Done
echo -e "${BLUE}============================================${NC}"
echo -e "${GREEN}   Test script completed!${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""
echo -e "${YELLOW}Ready for presentation? Good luck! 🍀${NC}"


