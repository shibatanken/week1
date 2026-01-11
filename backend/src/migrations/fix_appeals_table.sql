-- Fix appeals table - add missing columns for teacher_comment and read tracking
-- Check and add teacher_comment if not exists
SET @dbname = DATABASE();
SET @tablename = "appeals";
SET @columnname = "teacher_comment";
SET @preparedStatement = (SELECT IF(
  (SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS
   WHERE (table_name = @tablename)
   AND (table_schema = @dbname)
   AND (column_name = @columnname)) > 0,
  "SELECT 1",
  CONCAT("ALTER TABLE ", @tablename, " ADD COLUMN ", @columnname, " TEXT AFTER teacher_response")
));
PREPARE alterIfNotExists FROM @preparedStatement;
EXECUTE alterIfNotExists;
DEALLOCATE PREPARE alterIfNotExists;

-- Check and add teacher_read if not exists
SET @columnname = "teacher_read";
SET @preparedStatement = (SELECT IF(
  (SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS
   WHERE (table_name = @tablename)
   AND (table_schema = @dbname)
   AND (column_name = @columnname)) > 0,
  "SELECT 1",
  CONCAT("ALTER TABLE ", @tablename, " ADD COLUMN ", @columnname, " BOOLEAN DEFAULT FALSE AFTER resolved_at")
));
PREPARE alterIfNotExists FROM @preparedStatement;
EXECUTE alterIfNotExists;
DEALLOCATE PREPARE alterIfNotExists;

-- Update student_read default to FALSE if it exists (it's currently TRUE)
ALTER TABLE appeals MODIFY COLUMN student_read BOOLEAN DEFAULT FALSE;

