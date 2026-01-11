-- Migration: Create log table
-- Description: Lưu lịch sử hoạt động của hệ thống

CREATE TABLE IF NOT EXISTS log (
    id INT AUTO_INCREMENT PRIMARY KEY,
    log_content TEXT NOT NULL,
    log_time DATETIME NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);




