#!/bin/bash
set -e

echo "=========================================="
echo "SETUP WEEK1 PROJECT IN WSL"
echo "=========================================="
echo ""

echo "[1/6] Updating package list..."
sudo apt update -qq

echo "[2/6] Installing GCC and Make..."
sudo apt install -y gcc make

echo "[3/6] Installing MySQL Server..."
sudo DEBIAN_FRONTEND=noninteractive apt install -y mysql-server

echo "[4/6] Installing development libraries..."
sudo apt install -y libmysqlclient-dev libcjson-dev pkg-config

echo "[5/6] Starting MySQL service..."
sudo service mysql start
sleep 2

echo "[6/6] Setting up database..."
sudo mysql -e "CREATE DATABASE IF NOT EXISTS quizz_db;"
sudo mysql -e "CREATE USER IF NOT EXISTS 'quizz'@'localhost' IDENTIFIED BY 'Quizz2003@';"
sudo mysql -e "GRANT ALL PRIVILEGES ON quizz_db.* TO 'quizz'@'localhost';"
sudo mysql -e "FLUSH PRIVILEGES;"

echo ""
echo "=========================================="
echo "✅ SETUP COMPLETED!"
echo "=========================================="
echo ""
echo "Next steps:"
echo "1. cd /mnt/f/LTM/week1/backend"
echo "2. make clean && make"
echo "3. make migrate"
echo "4. make seed"
echo "5. ./server"
echo ""


