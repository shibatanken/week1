-- Add read status columns for appeal notifications
ALTER TABLE appeals 
ADD COLUMN student_read BOOLEAN DEFAULT FALSE,
ADD COLUMN teacher_read BOOLEAN DEFAULT FALSE,
ADD COLUMN teacher_comment TEXT;

